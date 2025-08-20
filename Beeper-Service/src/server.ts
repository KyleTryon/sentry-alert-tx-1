import Fastify, { FastifyInstance, FastifyRequest, FastifyReply } from 'fastify';
import { getConfig, type Config } from './config/environment.js';
import { verifySignature, validateTimestamp } from './utils/signature.js';
import { createMQTTService } from './services/mqtt.js';
import { 
  BaseWebhookPayloadSchema, 
  SentryWebhookHeadersSchema,
  type SentryWebhookPayload 
} from './types/sentry.js';

type ServerContext = {
  fastify: FastifyInstance;
  config: Config;
  mqtt: ReturnType<typeof createMQTTService>;
  startTime: Date;
};

function validateWebhookHeaders(headers: Record<string, unknown>): { valid: boolean; error?: string } {
  try {
    const normalized = Object.keys(headers).reduce((acc, key) => {
      const value = headers[key];
      acc[key.toLowerCase()] = Array.isArray(value) ? value.join(',') : String(value ?? '');
      return acc;
    }, {} as Record<string, string>);
    SentryWebhookHeadersSchema.parse(normalized);
    return { valid: true };
  } catch (error) {
    return { valid: false, error: `Missing or invalid required headers: ${error}` };
  }
}

async function processWebhook(ctx: ServerContext, payload: SentryWebhookPayload, requestId: string): Promise<void> {
  console.log('Processing webhook', { requestId, action: payload.action, installationId: payload.installation.uuid });
  try {
    await ctx.mqtt.publishAlert(payload);
    console.log('Webhook processed successfully', { requestId, action: payload.action });
  } catch (error) {
    console.error('Failed to process webhook', { requestId }, error as Error);
    throw error;
  }
}

function setupRoutes(ctx: ServerContext): void {
  const { fastify, config, mqtt, startTime } = ctx;

  fastify.get('/health', async (_request: FastifyRequest, reply: FastifyReply) => {
    const t0 = Date.now();
    try {
      const health = {
        status: 'ok',
        timestamp: new Date().toISOString(),
        uptime: Date.now() - startTime.getTime(),
        version: process.env.npm_package_version || '1.0.0',
        environment: config.NODE_ENV,
        mqtt: mqtt.getStatus(),
      };
      const duration = Date.now() - t0;
      console.log('HTTP Request', { method: 'GET', path: '/health', statusCode: 200, duration });
      return reply.code(200).send(health);
    } catch (error) {
      const duration = Date.now() - t0;
      console.log('HTTP Request', { method: 'GET', path: '/health', statusCode: 500, duration });
      console.error('Health check failed', error as Error);
      return reply.code(500).send({ status: 'error', message: 'Health check failed' });
    }
  });

  fastify.post<{ Body: SentryWebhookPayload; Headers: Record<string, string> }>(
    '/webhook',
    async (request, reply: FastifyReply) => {
      const t0 = Date.now();
      const rid = request.headers['request-id'];
      const requestId = typeof rid === 'string' ? rid : `req_${Date.now()}`;

      try {
        const headerValidation = validateWebhookHeaders(request.headers as Record<string, unknown>);
        if (!headerValidation.valid) {
          const duration = Date.now() - t0;
          console.log('HTTP Request', { method: 'POST', path: '/webhook', statusCode: 400, duration, requestId, error: headerValidation.error });
          return reply.code(400).send({ error: 'Invalid headers', message: headerValidation.error });
        }

        if (config.SENTRY_WEBHOOK_SECRET) {
          const sigHeader = request.headers['sentry-hook-signature'];
          const signature = typeof sigHeader === 'string' ? sigHeader : '';
          const rawBody = JSON.stringify(request.body);
          if (!verifySignature(rawBody, signature, config.SENTRY_WEBHOOK_SECRET)) {
            const duration = Date.now() - t0;
            console.log('HTTP Request', { method: 'POST', path: '/webhook', statusCode: 401, duration, requestId });
            console.warn('Invalid webhook signature', { requestId });
            return reply.code(401).send({ error: 'Unauthorized', message: 'Invalid signature' });
          }
        }

        const tsHeader = request.headers['sentry-hook-timestamp'];
        const timestamp = typeof tsHeader === 'string' ? tsHeader : '';
        if (!validateTimestamp(timestamp)) {
          const duration = Date.now() - t0;
          console.log('HTTP Request', { method: 'POST', path: '/webhook', statusCode: 400, duration, requestId });
          console.warn('Invalid webhook timestamp', { requestId, timestamp });
          return reply.code(400).send({ error: 'Invalid timestamp', message: 'Webhook timestamp is too old or invalid' });
        }

        const payloadValidation = BaseWebhookPayloadSchema.safeParse(request.body);
        if (!payloadValidation.success) {
          const duration = Date.now() - t0;
          console.log('HTTP Request', { method: 'POST', path: '/webhook', statusCode: 400, duration, requestId });
          console.warn('Invalid webhook payload', { requestId, errors: payloadValidation.error.errors });
          return reply.code(400).send({ error: 'Invalid payload', message: 'Webhook payload does not match expected schema', details: payloadValidation.error.errors });
        }

        await processWebhook(ctx, request.body, requestId);
        const duration = Date.now() - t0;
        console.log('HTTP Request', { method: 'POST', path: '/webhook', statusCode: 200, duration, requestId, resource: request.headers['sentry-hook-resource'], action: request.body.action });
        return reply.code(200).send({ status: 'success', message: 'Webhook processed successfully', requestId });
      } catch (error) {
        const duration = Date.now() - t0;
        console.log('HTTP Request', { method: 'POST', path: '/webhook', statusCode: 500, duration, requestId });
        console.error('Error processing webhook', { requestId }, error as Error);
        return reply.code(500).send({ error: 'Internal server error', message: 'Failed to process webhook', requestId });
      }
    }
  );

  fastify.get('/', async (_request: FastifyRequest, reply: FastifyReply) => {
    return reply.send({
      service: 'Beeper Service',
      version: process.env.npm_package_version || '1.0.0',
      description: 'MQTT-based message relay for Sentry alert webhooks',
      endpoints: { health: 'GET /health', webhook: 'POST /webhook' },
    });
  });
}

function setupErrorHandlers(ctx: ServerContext): void {
  const { fastify, config } = ctx;
  fastify.setErrorHandler(async (error, request, reply) => {
    console.error('Fastify error handler', { url: request.url, method: request.method }, error);
    const isDev = config.NODE_ENV === 'development';
    return reply.code(500).send({ error: 'Internal Server Error', message: isDev ? error.message : 'Something went wrong', ...(isDev && { stack: error.stack }) });
  });
  fastify.setNotFoundHandler(async (request, reply) => {
    console.warn('Route not found', { url: request.url, method: request.method });
    return reply.code(404).send({ error: 'Not Found', message: `Route ${request.method} ${request.url} not found` });
  });
}

export function createBeeperServer() {
  const config = getConfig();
  const mqtt = createMQTTService();
  const fastify = Fastify({ logger: false, bodyLimit: 1048576, trustProxy: true });
  const startTime = new Date();
  const ctx: ServerContext = { fastify, config, mqtt, startTime };

  setupRoutes(ctx);
  setupErrorHandlers(ctx);

  async function start(): Promise<void> {
    try {
      console.log('Connecting to MQTT broker...');
      await mqtt.connect();
      console.log('Starting HTTP server...');
      await fastify.listen({ port: config.PORT, host: config.HOST });
      console.log('Beeper Service started successfully', { port: config.PORT, host: config.HOST, environment: config.NODE_ENV });
    } catch (error) {
      console.error('Failed to start server', error as Error);
      throw error;
    }
  }

  async function stop(): Promise<void> {
    console.log('Shutting down server...');
    try {
      await mqtt.disconnect();
      await fastify.close();
      console.log('Server shutdown completed');
    } catch (error) {
      console.error('Error during shutdown', error as Error);
      throw error;
    }
  }

  function getServer(): FastifyInstance {
    return fastify;
  }

  return { start, stop, getServer };
}
