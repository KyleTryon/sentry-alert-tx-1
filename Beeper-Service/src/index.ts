import { loadConfig } from './config/environment.js';
import { createBeeperServer } from './server.js';

async function main(): Promise<void> {
  try {
    // Load and validate configuration
    console.log('Loading configuration...');
    loadConfig();
    console.log('Configuration loaded successfully');

    // Create and start server
    const server = createBeeperServer();
    
    // Handle graceful shutdown
    const gracefulShutdown = async (signal: string): Promise<void> => {
      console.log(`Received ${signal}, starting graceful shutdown...`);
      
      try {
        await server.stop();
        console.log('Graceful shutdown completed');
        process.exit(0);
      } catch (error) {
        console.error('Error during graceful shutdown', error);
        process.exit(1);
      }
    };

    // Setup signal handlers
    process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
    process.on('SIGINT', () => gracefulShutdown('SIGINT'));
    
    // Handle uncaught exceptions
    process.on('uncaughtException', (error) => {
      console.error('Uncaught exception', error);
      process.exit(1);
    });

    process.on('unhandledRejection', (reason, promise) => {
      console.error('Unhandled rejection', { 
        reason: String(reason),
        promise: String(promise) 
      });
      process.exit(1);
    });

    // Start the server
    await server.start();
    
  } catch (error) {
    console.error('Failed to start application', error as Error);
    process.exit(1);
  }
}

// Start the application
main().catch((error) => {
  console.error('Fatal error:', error);
  process.exit(1);
});
