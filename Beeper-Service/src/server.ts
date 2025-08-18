import Fastify, { FastifyInstance, FastifyRequest, FastifyReply } from 'fastify';
import { getConfig, type Config } from './config/environment.js';
import { logger } from './utils/logger.js';
import { verifySignature, validateTimestamp } from './utils/signature.js';
import { MQTTService } from './services/mqtt.js';
import { 
  BaseWebhookPayloadSchema, 
  SentryWebhookHeadersSchema,
  type SentryWebhookPayload 
} from './types/sentry.js';

interface WebhookRequest extends FastifyRequest {
  body: SentryWebhookPayload;
  headers: Record<string, string>;
}

export class BeeperServer {
  private fastify: FastifyInstance;
  private config: Config;
  private mqttService: MQTTService;
  private startTime: Date;

  constructor() {
    this.config = getConfig();
    this.mqttService = new MQTTService();
    this.startTime = new Date();
    
    this.fastify = Fastify({
      logger: false, // We use our custom logger
      bodyLimit: 1048576, // 1MB limit for webhook payloads
      trustProxy: true,
    });

    this.setupRoutes();
    this.setupErrorHandlers();
  }

  private setupRoutes(): void {
    // Health check endpoint
    this.fastify.get('/health', async (request: FastifyRequest, reply: FastifyReply) => {
      const startTime = Date.now();
      
      try {
        const mqttStatus = this.mqttService.getStatus();
        const uptime = Date.now() - this.startTime.getTime();
        
        const health = {
          status: 'ok',
          timestamp: new Date().toISOString(),
          uptime: uptime,
          version: process.env.npm_package_version || '1.0.0',
          environment: this.config.NODE_ENV,
          mqtt: mqttStatus,
        };

        const duration = Date.now() - startTime;
        logger.request('GET', '/health', 200, duration);
        
        return reply.code(200).send(health);
      } catch (error) {
        const duration = Date.now() - startTime;
        logger.request('GET', '/health', 500, duration);
        logger.error('Health check failed', {}, error as Error);
        
        return reply.code(500).send({
          status: 'error',
          message: 'Health check failed',
        });
      }
    });

    // Webhook endpoint
    this.fastify.post('/webhook', async (request: WebhookRequest, reply: FastifyReply) => {
      const startTime = Date.now();
      const requestId = request.headers['request-id'] || `req_${Date.now()}`;
      
      try {
        // Validate headers
        const headerValidation = this.validateWebhookHeaders(request.headers);
        if (!headerValidation.valid) {
          const duration = Date.now() - startTime;
          logger.request('POST', '/webhook', 400, duration, { requestId, error: headerValidation.error });
          
          return reply.code(400).send({
            error: 'Invalid headers',
            message: headerValidation.error,
          });
        }

        // Verify signature if secret is configured
        if (this.config.SENTRY_WEBHOOK_SECRET) {
          const signature = request.headers['sentry-hook-signature'];
          const rawBody = JSON.stringify(request.body);
          
          if (!verifySignature(rawBody, signature, this.config.SENTRY_WEBHOOK_SECRET)) {
            const duration = Date.now() - startTime;
            logger.request('POST', '/webhook', 401, duration, { requestId });
            logger.warn('Invalid webhook signature', { requestId });
            
            return reply.code(401).send({
              error: 'Unauthorized',
              message: 'Invalid signature',
            });
          }
        }

        // Validate timestamp to prevent replay attacks
        const timestamp = request.headers['sentry-hook-timestamp'];
        if (!validateTimestamp(timestamp)) {
          const duration = Date.now() - startTime;
          logger.request('POST', '/webhook', 400, duration, { requestId });
          logger.warn('Invalid webhook timestamp', { requestId, timestamp });
          
          return reply.code(400).send({
            error: 'Invalid timestamp',
            message: 'Webhook timestamp is too old or invalid',
          });
        }

        // Validate payload structure
        const payloadValidation = BaseWebhookPayloadSchema.safeParse(request.body);
        if (!payloadValidation.success) {
          const duration = Date.now() - startTime;
          logger.request('POST', '/webhook', 400, duration, { requestId });
          logger.warn('Invalid webhook payload', { 
            requestId, 
            errors: payloadValidation.error.errors 
          });
          
          return reply.code(400).send({
            error: 'Invalid payload',
            message: 'Webhook payload does not match expected schema',
            details: payloadValidation.error.errors,
          });
        }

        // Process the webhook
        await this.processWebhook(request.body, requestId);
        
        const duration = Date.now() - startTime;
        logger.request('POST', '/webhook', 200, duration, { 
          requestId,
          resource: request.headers['sentry-hook-resource'],
          action: request.body.action,
        });
        
        return reply.code(200).send({
          status: 'success',
          message: 'Webhook processed successfully',
          requestId,
        });
        
      } catch (error) {
        const duration = Date.now() - startTime;
        logger.request('POST', '/webhook', 500, duration, { requestId });
        logger.error('Error processing webhook', { requestId }, error as Error);
        
        return reply.code(500).send({
          error: 'Internal server error',
          message: 'Failed to process webhook',
          requestId,
        });
      }
    });

    // Root endpoint
    this.fastify.get('/', async (request: FastifyRequest, reply: FastifyReply) => {
      return reply.send({
        service: 'Beeper Service',
        version: process.env.npm_package_version || '1.0.0',
        description: 'MQTT-based message relay for Sentry alert webhooks',
        endpoints: {
          health: 'GET /health',
          webhook: 'POST /webhook',
        },
      });
    });
  }

  private validateWebhookHeaders(headers: Record<string, string>): { valid: boolean; error?: string } {
    try {
      // Normalize header keys to lowercase
      const normalizedHeaders = Object.keys(headers).reduce((acc, key) => {
        acc[key.toLowerCase()] = headers[key];
        return acc;
      }, {} as Record<string, string>);

      SentryWebhookHeadersSchema.parse(normalizedHeaders);
      return { valid: true };
    } catch (error) {
      return { 
        valid: false, 
        error: `Missing or invalid required headers: ${error}` 
      };
    }
  }

  private async processWebhook(payload: SentryWebhookPayload, requestId: string): Promise<void> {
    logger.info('Processing webhook', {
      requestId,
      action: payload.action,
      installationId: payload.installation.uuid,
    });

    try {
      // Forward to MQTT
      await this.mqttService.publishAlert(payload);
      
      logger.info('Webhook processed successfully', {
        requestId,
        action: payload.action,
      });
    } catch (error) {
      logger.error('Failed to process webhook', { requestId }, error as Error);
      throw error;
    }
  }

  private setupErrorHandlers(): void {
    this.fastify.setErrorHandler(async (error, request, reply) => {
      logger.error('Fastify error handler', {
        url: request.url,
        method: request.method,
      }, error);

      // Don't leak error details in production
      const isDev = this.config.NODE_ENV === 'development';
      
      return reply.code(500).send({
        error: 'Internal Server Error',
        message: isDev ? error.message : 'Something went wrong',
        ...(isDev && { stack: error.stack }),
      });
    });

    this.fastify.setNotFoundHandler(async (request, reply) => {
      logger.warn('Route not found', {
        url: request.url,
        method: request.method,
      });

      return reply.code(404).send({
        error: 'Not Found',
        message: `Route ${request.method} ${request.url} not found`,
      });
    });
  }

  async start(): Promise<void> {
    try {
      // Connect to MQTT first
      logger.info('Connecting to MQTT broker...');
      await this.mqttService.connect();
      
      // Start HTTP server
      logger.info('Starting HTTP server...');
      await this.fastify.listen({ 
        port: this.config.PORT, 
        host: this.config.HOST 
      });
      
      logger.info('Beeper Service started successfully', {
        port: this.config.PORT,
        host: this.config.HOST,
        environment: this.config.NODE_ENV,
      });
      
    } catch (error) {
      logger.error('Failed to start server', {}, error as Error);
      throw error;
    }
  }

  async stop(): Promise<void> {
    logger.info('Shutting down server...');
    
    try {
      await this.mqttService.disconnect();
      await this.fastify.close();
      logger.info('Server shutdown completed');
    } catch (error) {
      logger.error('Error during shutdown', {}, error as Error);
      throw error;
    }
  }

  getServer(): FastifyInstance {
    return this.fastify;
  }
}
