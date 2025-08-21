import mqtt, { MqttClient, IClientOptions, IClientPublishOptions } from 'mqtt';
import { getConfig, type Config } from '../config/environment.js';
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

export function createMQTTService() {
  let client: MqttClient | null = null;
  const config: Config = getConfig();
  let reconnectAttempts = 0;
  const maxReconnectAttempts = 10;
  const reconnectDelay = 1000;
  let isConnected = false;
  const messageQueue: Array<{ topic: string; message: string; options: IClientPublishOptions }> = [];

  function publishDirect(topic: string, message: string, options: IClientPublishOptions): void {
    if (!client || !isConnected) {
      throw new Error('MQTT client not connected');
    }
    client.publish(topic, message, options, (error) => {
      if (error) {
        console.error('Failed to publish MQTT message', { topic, error: error.message }, error);
      } else {
        console.debug('MQTT Event', { event: 'Message published', topic, messageLength: message.length, qos: options.qos });
      }
    });
  }

  function processMessageQueue(): void {
    if (!isConnected || messageQueue.length === 0) return;
    console.log(`Processing ${messageQueue.length} queued messages`);
    while (messageQueue.length > 0) {
      const { topic, message, options } = messageQueue.shift()!;
      publishDirect(topic, message, options);
    }
  }

  function handleReconnection(): void {
    if (reconnectAttempts >= maxReconnectAttempts) {
      console.error('Max reconnection attempts reached');
      return;
    }
    reconnectAttempts++;
    const delay = reconnectDelay * Math.pow(2, reconnectAttempts - 1);
    console.log(`Reconnecting to MQTT broker in ${delay}ms (attempt ${reconnectAttempts})`);
    setTimeout(() => {
      connect().catch((error) => {
        console.error('Reconnection failed', { error: (error as Error).message }, error);
      });
    }, delay);
  }

  function transformWebhookToMQTTMessage(payload: SentryWebhookPayload): MQTTMessage {
    const id = `sentry_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
    const timestamp = new Date().toISOString();
    let title = 'Sentry Alert';
    let message = '';
    let level = 'info';
    let url = '';
    let environment = '';
    let release = '';
    let project = '';
    let tags: Record<string, string> = {};
    let priority: MQTTMessage['priority'] = 'medium';

    if ('data' in payload && payload.data) {
      if ('event' in payload.data && payload.data.event) {
        const event = payload.data.event;
        title = event.title || 'Sentry Error';
        message = event.message || '';
        level = event.level || 'error';
        url = event.web_url || event.url || '';
        environment = event.environment || '';
        release = event.release || '';
        if (event.tags) {
          tags = Object.fromEntries(event.tags);
        }
        // Always use high priority for Sentry alerts to ensure they trigger the beeper
        priority = 'high';
      }
      if ('triggered_rule' in payload.data && payload.data.triggered_rule) {
        message = `Rule: ${payload.data.triggered_rule}`;
      }
    }

    return { id, timestamp, source: 'sentry', type: payload.action || 'unknown', priority, data: { title, message, level, url, environment, release, project, tags }, raw: payload };
  }

  function buildTopic(_message: MQTTMessage): string {
    // Send all messages to the base alerts topic to ensure the beeper receives them
    // The beeper subscribes to "alerts/#" so this should work
    return config.MQTT_TOPIC_PREFIX;
  }

  async function connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      const connectOptions: IClientOptions = {
        clientId: config.MQTT_CLIENT_ID,
        clean: true,
        connectTimeout: 10000,
        reconnectPeriod: 0,
        ...(config.MQTT_USERNAME && { username: config.MQTT_USERNAME }),
        ...(config.MQTT_PASSWORD && { password: config.MQTT_PASSWORD }),
      };
      console.log('Connecting to MQTT broker', { brokerUrl: config.MQTT_BROKER_URL, clientId: config.MQTT_CLIENT_ID });
      client = mqtt.connect(config.MQTT_BROKER_URL, connectOptions);
      client.on('connect', () => {
        isConnected = true;
        reconnectAttempts = 0;
        console.log('Connected to MQTT broker');
        processMessageQueue();
        resolve();
      });
      client.on('error', (error) => {
        console.error('MQTT connection error', { error: error.message }, error);
        if (!isConnected) {
          reject(error);
        } else {
          handleReconnection();
        }
      });
      client.on('disconnect', () => {
        isConnected = false;
        console.warn('Disconnected from MQTT broker');
        handleReconnection();
      });
      client.on('offline', () => {
        isConnected = false;
        console.warn('MQTT client went offline');
      });
      client.on('reconnect', () => {
        console.log('Attempting to reconnect to MQTT broker');
      });
    });
  }

  async function publishAlert(webhookPayload: SentryWebhookPayload): Promise<void> {
    const message = transformWebhookToMQTTMessage(webhookPayload);
    const topic = buildTopic(message);
    const messageJson = JSON.stringify(message, null, 0);
    const publishOptions: IClientPublishOptions = { qos: (config.MQTT_QOS as 0 | 1 | 2), retain: false };
    console.log('Publishing alert to MQTT', { topic, messageId: message.id, priority: message.priority, type: message.type });
    if (isConnected && client) {
      publishDirect(topic, messageJson, publishOptions);
    } else {
      console.warn('MQTT client not connected, queueing message', { topic, messageId: message.id });
      messageQueue.push({ topic, message: messageJson, options: publishOptions });
    }
  }

  async function disconnect(): Promise<void> {
    if (client) {
      console.log('Disconnecting from MQTT broker');
      return new Promise((resolve) => {
        client!.end(false, {}, () => {
          isConnected = false;
          console.log('Disconnected from MQTT broker');
          resolve();
        });
      });
    }
  }

  function isHealthy(): boolean {
    return isConnected && client !== null;
  }

  function getStatus() {
    return { connected: isConnected, reconnectAttempts, queuedMessages: messageQueue.length, brokerUrl: config.MQTT_BROKER_URL, clientId: config.MQTT_CLIENT_ID };
  }

  return { connect, disconnect, publishAlert, isHealthy, getStatus };
}
