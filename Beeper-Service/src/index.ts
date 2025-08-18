import { loadConfig } from './config/environment.js';
import { logger } from './utils/logger.js';
import { BeeperServer } from './server.js';

async function main(): Promise<void> {
  try {
    // Load and validate configuration
    logger.info('Loading configuration...');
    loadConfig();
    logger.info('Configuration loaded successfully');

    // Create and start server
    const server = new BeeperServer();
    
    // Handle graceful shutdown
    const gracefulShutdown = async (signal: string): Promise<void> => {
      logger.info(`Received ${signal}, starting graceful shutdown...`);
      
      try {
        await server.stop();
        logger.info('Graceful shutdown completed');
        process.exit(0);
      } catch (error) {
        logger.error('Error during graceful shutdown', {}, error as Error);
        process.exit(1);
      }
    };

    // Setup signal handlers
    process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
    process.on('SIGINT', () => gracefulShutdown('SIGINT'));
    
    // Handle uncaught exceptions
    process.on('uncaughtException', (error) => {
      logger.error('Uncaught exception', {}, error);
      process.exit(1);
    });

    process.on('unhandledRejection', (reason, promise) => {
      logger.error('Unhandled rejection', { 
        reason: String(reason),
        promise: String(promise) 
      });
      process.exit(1);
    });

    // Start the server
    await server.start();
    
  } catch (error) {
    logger.error('Failed to start application', {}, error as Error);
    process.exit(1);
  }
}

// Start the application
main().catch((error) => {
  console.error('Fatal error:', error);
  process.exit(1);
});
