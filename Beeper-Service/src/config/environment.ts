import { z } from 'zod';

// Environment schema with validation
const EnvironmentSchema = z.object({
  // Server configuration
  PORT: z.string().default('3000').transform(Number),
  HOST: z.string().default('0.0.0.0'),
  NODE_ENV: z.enum(['development', 'production', 'test']).default('development'),
  
  // MQTT configuration
  MQTT_BROKER_URL: z.string().url(),
  MQTT_USERNAME: z.string().optional(),
  MQTT_PASSWORD: z.string().optional(),
  MQTT_CLIENT_ID: z.string().default('beeper-service'),
  MQTT_TOPIC_PREFIX: z.string().default('alerts'),
  MQTT_QOS: z.string().default('1').transform(Number).refine(val => [0, 1, 2].includes(val)),
  
  // Sentry configuration
  SENTRY_WEBHOOK_SECRET: z.string().optional(),
  SENTRY_ALLOWED_IPS: z.string().optional().transform(val => val?.split(',').map(ip => ip.trim())),
  
  // Logging configuration
  LOG_LEVEL: z.enum(['error', 'warn', 'info', 'debug']).default('info'),
  
  // Health check configuration
  HEALTH_CHECK_INTERVAL_MS: z.string().default('30000').transform(Number),
});

class ConfigurationError extends Error {
  constructor(message: string) {
    super(`Configuration Error: ${message}`);
    this.name = 'ConfigurationError';
  }
}

let config: z.infer<typeof EnvironmentSchema>;

export function loadConfig(): z.infer<typeof EnvironmentSchema> {
  if (config) {
    return config;
  }

  try {
    config = EnvironmentSchema.parse(process.env);
    return config;
  } catch (error) {
    if (error instanceof z.ZodError) {
      const missingRequired = error.errors
        .filter(err => err.code === 'invalid_type' && err.received === 'undefined')
        .map(err => err.path.join('.'));
      
      const invalidValues = error.errors
        .filter(err => err.code !== 'invalid_type' || err.received !== 'undefined')
        .map(err => `${err.path.join('.')}: ${err.message}`);

      let errorMessage = 'Invalid environment configuration:\n';
      
      if (missingRequired.length > 0) {
        errorMessage += `Missing required variables: ${missingRequired.join(', ')}\n`;
      }
      
      if (invalidValues.length > 0) {
        errorMessage += `Invalid values: ${invalidValues.join(', ')}`;
      }

      throw new ConfigurationError(errorMessage);
    }
    throw error;
  }
}

export function getConfig(): z.infer<typeof EnvironmentSchema> {
  if (!config) {
    throw new ConfigurationError('Configuration not loaded. Call loadConfig() first.');
  }
  return config;
}

export type Config = z.infer<typeof EnvironmentSchema>;
