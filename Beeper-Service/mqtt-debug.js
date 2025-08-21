#!/usr/bin/env node

// Simple MQTT subscriber for debugging message delivery
import mqtt from 'mqtt';
import chalk from 'chalk';

const config = {
  brokerUrl: process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883',
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD,
  clientId: `debug-subscriber-${Date.now()}`,
  topicPrefix: process.env.MQTT_TOPIC_PREFIX || 'alerts',
};

const isRemote = !config.brokerUrl.includes('localhost');

console.log(chalk.bold('🔊 MQTT Debug Subscriber'));
console.log(chalk.gray('Broker:'), config.brokerUrl);
console.log(chalk.gray('Topics:'), `${config.topicPrefix}/#`);
if (isRemote) {
  console.log(chalk.yellow('Remote Mode: Connecting to Digital Ocean deployment'));
}
console.log('');

const options = {
  clientId: config.clientId,
  clean: true,
  ...(config.username && { username: config.username }),
  ...(config.password && { password: config.password }),
};

const client = mqtt.connect(config.brokerUrl, options);

client.on('connect', () => {
  console.log(chalk.green('✓'), 'Connected to MQTT broker');
  
  // Subscribe to all topics under the prefix
  const topic = `${config.topicPrefix}/#`;
  client.subscribe(topic, { qos: 1 }, (err) => {
    if (err) {
      console.error(chalk.red('✗'), 'Subscribe failed:', err.message);
      process.exit(1);
    }
    console.log(chalk.green('✓'), `Subscribed to ${topic}`);
    console.log(chalk.gray('\nWaiting for messages...\n'));
  });
});

client.on('message', (topic, message) => {
  console.log(chalk.blue('━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━'));
  console.log(chalk.cyan('Topic:'), topic);
  console.log(chalk.cyan('Time:'), new Date().toISOString());
  console.log(chalk.cyan('Size:'), message.length, 'bytes');
  
  try {
    const payload = JSON.parse(message.toString());
    console.log(chalk.cyan('Payload:'));
    console.log(JSON.stringify(payload, null, 2));
  } catch (error) {
    console.log(chalk.cyan('Raw:'), message.toString());
  }
  console.log('');
});

client.on('error', (error) => {
  console.error(chalk.red('✗'), 'MQTT error:', error.message);
});

client.on('offline', () => {
  console.warn(chalk.yellow('⚠'), 'MQTT client offline');
});

client.on('reconnect', () => {
  console.log(chalk.yellow('⚠'), 'Reconnecting to MQTT broker...');
});

// Handle shutdown
process.on('SIGINT', () => {
  console.log('\n\nShutting down...');
  client.end();
  process.exit(0);
});
