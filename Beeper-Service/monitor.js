#!/usr/bin/env node

import mqtt from 'mqtt';
import axios from 'axios';
import { createInterface } from 'readline';
import chalk from 'chalk';

// Configuration
const config = {
  // Service endpoints
  serviceUrl: process.env.SERVICE_URL || 'http://localhost:3000',
  
  // MQTT configuration
  mqttBrokerUrl: process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883',
  mqttUsername: process.env.MQTT_USERNAME,
  mqttPassword: process.env.MQTT_PASSWORD,
  mqttClientId: `monitor-${Date.now()}`,
  mqttTopicPrefix: process.env.MQTT_TOPIC_PREFIX || 'alerts',
  
  // Monitoring settings
  checkInterval: 5000, // 5 seconds
  webhookTestInterval: 60000, // 1 minute
};

// Add remote monitoring flag
config.isRemote = !config.serviceUrl.includes('localhost');

// Display configuration on startup
console.log(chalk.bold('🔍 Beeper Service Monitor'));
console.log(chalk.gray('  Service URL:'), config.serviceUrl);
console.log(chalk.gray('  MQTT Broker:'), config.mqttBrokerUrl);
console.log(chalk.gray('  Remote Mode:'), config.isRemote ? chalk.yellow('Yes') : chalk.green('No'));
if (config.isRemote) {
  console.log(chalk.yellow('  ⚠️  Monitoring remote deployment'));
}
console.log('');

// Track message flow
const messageTracking = new Map();
let mqttClient = null;
let isConnected = false;

// Console output helpers
const log = {
  info: (msg) => console.log(chalk.blue('ℹ'), msg),
  success: (msg) => console.log(chalk.green('✓'), msg),
  error: (msg) => console.log(chalk.red('✗'), msg),
  warn: (msg) => console.log(chalk.yellow('⚠'), msg),
  mqtt: (msg) => console.log(chalk.magenta('MQTT'), msg),
  http: (msg) => console.log(chalk.cyan('HTTP'), msg),
};

// Connect to MQTT broker
async function connectMQTT() {
  return new Promise((resolve, reject) => {
    log.info(`Connecting to MQTT broker at ${config.mqttBrokerUrl}...`);
    
    const options = {
      clientId: config.mqttClientId,
      clean: true,
      ...(config.mqttUsername && { username: config.mqttUsername }),
      ...(config.mqttPassword && { password: config.mqttPassword }),
    };
    
    mqttClient = mqtt.connect(config.mqttBrokerUrl, options);
    
    mqttClient.on('connect', () => {
      isConnected = true;
      log.success('Connected to MQTT broker');
      
      // Subscribe to all alert topics
      const topic = `${config.mqttTopicPrefix}/+/+`;
      mqttClient.subscribe(topic, { qos: 1 }, (err) => {
        if (err) {
          log.error(`Failed to subscribe to ${topic}: ${err.message}`);
          reject(err);
        } else {
          log.mqtt(`Subscribed to ${topic}`);
          resolve();
        }
      });
    });
    
    mqttClient.on('message', (topic, message) => {
      try {
        const payload = JSON.parse(message.toString());
        log.mqtt(`Received message on ${topic}`);
        console.log(chalk.gray('  Message ID:'), payload.id);
        console.log(chalk.gray('  Priority:'), payload.priority);
        console.log(chalk.gray('  Title:'), payload.data.title);
        console.log(chalk.gray('  Timestamp:'), payload.timestamp);
        
        // Track message receipt
        if (messageTracking.has(payload.id)) {
          const tracking = messageTracking.get(payload.id);
          tracking.mqttReceived = Date.now();
          tracking.mqttLatency = tracking.mqttReceived - tracking.webhookSent;
          log.success(`Message delivered in ${tracking.mqttLatency}ms`);
        }
      } catch (error) {
        log.error(`Failed to parse MQTT message: ${error.message}`);
      }
    });
    
    mqttClient.on('error', (error) => {
      log.error(`MQTT error: ${error.message}`);
      isConnected = false;
    });
    
    mqttClient.on('offline', () => {
      log.warn('MQTT client went offline');
      isConnected = false;
    });
    
    mqttClient.on('reconnect', () => {
      log.info('Attempting to reconnect to MQTT broker...');
    });
  });
}

// Check service health
async function checkHealth() {
  try {
    const response = await axios.get(`${config.serviceUrl}/health`);
    const health = response.data;
    
    console.log('\n' + chalk.bold('Service Health Check:'));
    console.log(chalk.gray('  Status:'), health.status === 'ok' ? chalk.green(health.status) : chalk.red(health.status));
    console.log(chalk.gray('  Uptime:'), `${Math.floor(health.uptime / 1000)}s`);
    console.log(chalk.gray('  Environment:'), health.environment);
    console.log(chalk.gray('  MQTT Connected:'), health.mqtt.connected ? chalk.green('Yes') : chalk.red('No'));
    console.log(chalk.gray('  MQTT Queue:'), health.mqtt.queuedMessages, 'messages');
    
    if (health.mqtt.reconnectAttempts > 0) {
      log.warn(`MQTT reconnection attempts: ${health.mqtt.reconnectAttempts}`);
    }
    
    return health;
  } catch (error) {
    log.error(`Health check failed: ${error.message}`);
    return null;
  }
}

// Send test webhook
async function sendTestWebhook() {
  const testPayload = {
    action: 'event.alert',
    installation: {
      uuid: 'test-installation-123'
    },
    data: {
      event: {
        title: 'Monitor Test Alert',
        message: `Test alert from monitor at ${new Date().toISOString()}`,
        level: 'warning',
        environment: 'test',
        url: 'https://sentry.io/test',
        tags: [
          ['source', 'monitor'],
          ['test', 'true']
        ]
      }
    }
  };
  
  const messageId = `test_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
  
  try {
    log.http('Sending test webhook...');
    
    // Track the message
    messageTracking.set(messageId, {
      webhookSent: Date.now(),
      mqttReceived: null,
      mqttLatency: null
    });
    
    const response = await axios.post(`${config.serviceUrl}/webhook`, testPayload, {
      headers: {
        'Content-Type': 'application/json',
        'sentry-hook-resource': 'event_alert',
        'sentry-hook-timestamp': Math.floor(Date.now() / 1000).toString(),
        'request-id': messageId
      }
    });
    
    if (response.status === 200) {
      log.success(`Test webhook accepted (${response.data.requestId})`);
    } else {
      log.error(`Test webhook failed: ${response.status} ${response.statusText}`);
    }
    
    // Wait a bit and check if message was received
    setTimeout(() => {
      const tracking = messageTracking.get(messageId);
      if (!tracking.mqttReceived) {
        log.error(`Test message not received via MQTT after 5 seconds!`);
        log.warn('Possible issues:');
        log.warn('  - MQTT broker not forwarding messages');
        log.warn('  - Service not publishing to MQTT');
        log.warn('  - Network connectivity issues');
      }
      messageTracking.delete(messageId);
    }, 5000);
    
  } catch (error) {
    log.error(`Failed to send test webhook: ${error.message}`);
    if (error.response) {
      console.log(chalk.gray('  Response:'), error.response.data);
    }
  }
}

// Interactive CLI
function startCLI() {
  const rl = createInterface({
    input: process.stdin,
    output: process.stdout,
    prompt: chalk.green('monitor> ')
  });
  
  console.log(chalk.bold('\nBeeper Service Monitor'));
  console.log(chalk.gray('Commands: health, test, stats, clear, quit\n'));
  
  rl.prompt();
  
  rl.on('line', async (line) => {
    const command = line.trim().toLowerCase();
    
    switch (command) {
      case 'health':
      case 'h':
        await checkHealth();
        break;
        
      case 'test':
      case 't':
        await sendTestWebhook();
        break;
        
      case 'stats':
      case 's':
        console.log(chalk.bold('\nMessage Statistics:'));
        console.log(chalk.gray('  Tracked messages:'), messageTracking.size);
        console.log(chalk.gray('  MQTT connected:'), isConnected ? chalk.green('Yes') : chalk.red('No'));
        let totalLatency = 0;
        let deliveredCount = 0;
        messageTracking.forEach(tracking => {
          if (tracking.mqttLatency) {
            totalLatency += tracking.mqttLatency;
            deliveredCount++;
          }
        });
        if (deliveredCount > 0) {
          console.log(chalk.gray('  Average latency:'), `${Math.round(totalLatency / deliveredCount)}ms`);
        }
        break;
        
      case 'clear':
      case 'c':
        console.clear();
        console.log(chalk.bold('Beeper Service Monitor'));
        console.log(chalk.gray('Commands: health, test, stats, clear, quit\n'));
        break;
        
      case 'quit':
      case 'q':
      case 'exit':
        console.log('Goodbye!');
        process.exit(0);
        
      default:
        if (command) {
          console.log(chalk.gray('Unknown command. Available: health, test, stats, clear, quit'));
        }
    }
    
    rl.prompt();
  });
  
  rl.on('close', () => {
    console.log('\nGoodbye!');
    process.exit(0);
  });
}

// Auto health checks
function startAutoChecks() {
  // Health check every 5 seconds
  setInterval(async () => {
    const health = await checkHealth();
    if (health && health.status !== 'ok') {
      log.error('Service is not healthy!');
    }
  }, config.checkInterval);
  
  // Test webhook every minute (optional)
  if (process.env.AUTO_TEST === 'true') {
    setInterval(() => {
      sendTestWebhook();
    }, config.webhookTestInterval);
  }
}

// Main
async function main() {
  try {
    // Connect to MQTT first
    await connectMQTT();
    
    // Initial health check
    await checkHealth();
    
    // Start auto checks if not in interactive mode
    if (process.argv.includes('--auto')) {
      log.info('Starting automated monitoring...');
      startAutoChecks();
    } else {
      // Start interactive CLI
      startCLI();
    }
    
  } catch (error) {
    log.error(`Failed to start monitor: ${error.message}`);
    process.exit(1);
  }
}

// Handle shutdown
process.on('SIGINT', () => {
  console.log('\nShutting down monitor...');
  if (mqttClient) {
    mqttClient.end();
  }
  process.exit(0);
});

// Start the monitor
main();
