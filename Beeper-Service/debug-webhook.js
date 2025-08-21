#!/usr/bin/env node

// Debug webhook receiver to see exactly what Sentry sends
import Fastify from 'fastify';
import chalk from 'chalk';

const fastify = Fastify({ 
  logger: false,
  bodyLimit: 1048576 
});

// Log all requests in detail
fastify.addHook('preHandler', async (request, reply) => {
  console.log(chalk.blue('━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━'));
  console.log(chalk.cyan('Incoming Request:'), request.method, request.url);
  console.log(chalk.cyan('Time:'), new Date().toISOString());
  console.log(chalk.cyan('Headers:'));
  
  // Log all headers
  Object.entries(request.headers).forEach(([key, value]) => {
    console.log(`  ${chalk.yellow(key)}: ${value}`);
  });
  
  console.log(chalk.cyan('Body:'));
  console.log(JSON.stringify(request.body, null, 2));
  console.log('');
});

// Accept any webhook
fastify.post('/webhook', async (request, reply) => {
  // Check which headers are missing
  const requiredHeaders = [
    'content-type',
    'sentry-hook-resource', 
    'sentry-hook-timestamp',
    'sentry-hook-signature',
    'request-id'
  ];
  
  const missingHeaders = requiredHeaders.filter(h => !request.headers[h]);
  
  if (missingHeaders.length > 0) {
    console.log(chalk.red('Missing headers:'), missingHeaders);
  }
  
  // Log specific Sentry headers
  console.log(chalk.green('Sentry Headers Found:'));
  Object.entries(request.headers).forEach(([key, value]) => {
    if (key.startsWith('sentry-')) {
      console.log(`  ${key}: ${value}`);
    }
  });
  
  return reply.code(200).send({ 
    status: 'success', 
    message: 'Debug webhook received',
    headers: request.headers,
    missingHeaders 
  });
});

// Health check
fastify.get('/health', async () => ({ status: 'ok', service: 'debug-webhook' }));

// Start server
const PORT = process.env.PORT || 3001;
fastify.listen({ port: PORT, host: '0.0.0.0' }, (err) => {
  if (err) {
    console.error(err);
    process.exit(1);
  }
  console.log(chalk.green(`🔍 Debug webhook receiver listening on port ${PORT}`));
  console.log(chalk.yellow(`\nTo use this locally:`));
  console.log(`1. Run: ngrok http ${PORT}`);
  console.log(`2. Update Sentry webhook URL to the ngrok URL + /webhook`);
  console.log(`3. Trigger a Sentry alert and watch the logs here\n`);
});
