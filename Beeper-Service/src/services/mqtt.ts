import mqtt, { MqttClient } from 'mqtt';
import { getConfig, type Config } from '../config/environment.js';
import { logger } from '../utils/logger.js';
import type { SentryWebhookPayload } from '../types/sentry.js';

export interface MQTTMessage {
  id: string;
  timestamp: string;
  source: 'sentry';
  type: string;
  priority: 'low' | 'medium' | 'high' | 'critical';
  data: {
    title: string;
    message?: string;
    level: string;
    url?: string;
    environment?: string;
    release?: string;
    project?: string;
    tags?: Record<string, string>;
  };
  raw: SentryWebhookPayload;
}

export class MQTTService {
  private client: MqttClient | null = null;
  private config: Config;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 10;
  private reconnectDelay = 1000;
  private isConnected = false;
  private messageQueue: Array<{ topic: string; message: string; options: any }> = [];

  constructor() {
    this.config = getConfig();
  }

  async connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      const connectOptions: mqtt.IClientOptions = {
        clientId: this.config.MQTT_CLIENT_ID,
        clean: true,
        connectTimeout: 10000,
        reconnectPeriod: 0, // We'll handle reconnection manually
        ...(this.config.MQTT_USERNAME && { username: this.config.MQTT_USERNAME }),
        ...(this.config.MQTT_PASSWORD && { password: this.config.MQTT_PASSWORD }),
      };

      logger.info('Connecting to MQTT broker', {
        brokerUrl: this.config.MQTT_BROKER_URL,
        clientId: this.config.MQTT_CLIENT_ID,
      });

      this.client = mqtt.connect(this.config.MQTT_BROKER_URL, connectOptions);

      this.client.on('connect', () => {
        this.isConnected = true;
        this.reconnectAttempts = 0;
        logger.info('Connected to MQTT broker');
        
        // Process any queued messages
        this.processMessageQueue();
        
        resolve();
      });

      this.client.on('error', (error) => {
        logger.error('MQTT connection error', { error: error.message }, error);
        
        if (!this.isConnected) {
          reject(error);
        } else {
          this.handleReconnection();
        }
      });

      this.client.on('disconnect', () => {
        this.isConnected = false;
        logger.warn('Disconnected from MQTT broker');
        this.handleReconnection();
      });

      this.client.on('offline', () => {
        this.isConnected = false;
        logger.warn('MQTT client went offline');
      });

      this.client.on('reconnect', () => {
        logger.info('Attempting to reconnect to MQTT broker');
      });
    });
  }

  private handleReconnection(): void {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      logger.error('Max reconnection attempts reached');
      return;
    }

    this.reconnectAttempts++;
    const delay = this.reconnectDelay * Math.pow(2, this.reconnectAttempts - 1);
    
    logger.info(`Reconnecting to MQTT broker in ${delay}ms (attempt ${this.reconnectAttempts})`);
    
    setTimeout(() => {
      this.connect().catch((error) => {
        logger.error('Reconnection failed', { error: error.message }, error);
      });
    }, delay);
  }

  private processMessageQueue(): void {
    if (!this.isConnected || this.messageQueue.length === 0) {
      return;
    }

    logger.info(`Processing ${this.messageQueue.length} queued messages`);
    
    while (this.messageQueue.length > 0) {
      const { topic, message, options } = this.messageQueue.shift()!;
      this.publishDirect(topic, message, options);
    }
  }

  private publishDirect(topic: string, message: string, options: any): void {
    if (!this.client || !this.isConnected) {
      throw new Error('MQTT client not connected');
    }

    this.client.publish(topic, message, options, (error) => {
      if (error) {
        logger.error('Failed to publish MQTT message', {
          topic,
          error: error.message,
        }, error);
      } else {
        logger.mqtt('Message published', topic, {
          messageLength: message.length,
          qos: options.qos,
        });
      }
    });
  }

  async publishAlert(webhookPayload: SentryWebhookPayload): Promise<void> {
    const message = this.transformWebhookToMQTTMessage(webhookPayload);
    const topic = this.buildTopic(message);
    const messageJson = JSON.stringify(message, null, 0);
    
    const publishOptions = {
      qos: this.config.MQTT_QOS as mqtt.QoS,
      retain: false,
    };

    logger.info('Publishing alert to MQTT', {
      topic,
      messageId: message.id,
      priority: message.priority,
      type: message.type,
    });

    if (this.isConnected && this.client) {
      this.publishDirect(topic, messageJson, publishOptions);
    } else {
      // Queue the message for later delivery
      logger.warn('MQTT client not connected, queueing message', {
        topic,
        messageId: message.id,
      });
      
      this.messageQueue.push({
        topic,
        message: messageJson,
        options: publishOptions,
      });
    }
  }

  private transformWebhookToMQTTMessage(payload: SentryWebhookPayload): MQTTMessage {
    const id = `sentry_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
    const timestamp = new Date().toISOString();
    
    // Extract common data from different webhook types
    let title = 'Sentry Alert';
    let message = '';
    let level = 'info';
    let url = '';
    let environment = '';
    let release = '';
    let project = '';
    let tags: Record<string, string> = {};
    let priority: MQTTMessage['priority'] = 'medium';

    // Handle different webhook types
    if ('data' in payload && payload.data) {
      if ('event' in payload.data && payload.data.event) {
        const event = payload.data.event;
        title = event.title || 'Sentry Error';
        message = event.message || '';
        level = event.level || 'error';
        url = event.web_url || event.url || '';
        environment = event.environment || '';
        release = event.release || '';
        
        // Convert tags array to object
        if (event.tags) {
          tags = Object.fromEntries(event.tags);
        }
        
        // Determine priority based on level
        switch (event.level) {
          case 'fatal':
            priority = 'critical';
            break;
          case 'error':
            priority = 'high';
            break;
          case 'warning':
            priority = 'medium';
            break;
          default:
            priority = 'low';
        }
      }
      
      if ('triggered_rule' in payload.data && payload.data.triggered_rule) {
        message = `Rule: ${payload.data.triggered_rule}`;
      }
    }

    return {
      id,
      timestamp,
      source: 'sentry',
      type: payload.action || 'unknown',
      priority,
      data: {
        title,
        message,
        level,
        url,
        environment,
        release,
        project,
        tags,
      },
      raw: payload,
    };
  }

  private buildTopic(message: MQTTMessage): string {
    const { MQTT_TOPIC_PREFIX } = this.config;
    return `${MQTT_TOPIC_PREFIX}/${message.priority}/${message.type}`;
  }

  async disconnect(): Promise<void> {
    if (this.client) {
      logger.info('Disconnecting from MQTT broker');
      
      return new Promise((resolve) => {
        this.client!.end(false, {}, () => {
          this.isConnected = false;
          logger.info('Disconnected from MQTT broker');
          resolve();
        });
      });
    }
  }

  isHealthy(): boolean {
    return this.isConnected && this.client !== null;
  }

  getStatus() {
    return {
      connected: this.isConnected,
      reconnectAttempts: this.reconnectAttempts,
      queuedMessages: this.messageQueue.length,
      brokerUrl: this.config.MQTT_BROKER_URL,
      clientId: this.config.MQTT_CLIENT_ID,
    };
  }
}
