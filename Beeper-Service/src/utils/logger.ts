import { getConfig } from '../config/environment.js';

export type LogLevel = 'error' | 'warn' | 'info' | 'debug';

interface LogEntry {
  level: LogLevel;
  message: string;
  timestamp: string;
  context?: Record<string, any>;
  error?: Error;
}

class Logger {
  private logLevel: LogLevel;
  private readonly logLevels: Record<LogLevel, number> = {
    error: 0,
    warn: 1,
    info: 2,
    debug: 3,
  };

  constructor() {
    try {
      const config = getConfig();
      this.logLevel = config.LOG_LEVEL;
    } catch {
      this.logLevel = 'info';
    }
  }

  private shouldLog(level: LogLevel): boolean {
    return this.logLevels[level] <= this.logLevels[this.logLevel];
  }

  private formatLogEntry(entry: LogEntry): string {
    const baseLog = {
      level: entry.level,
      timestamp: entry.timestamp,
      message: entry.message,
      ...(entry.context && { context: entry.context }),
      ...(entry.error && { 
        error: {
          name: entry.error.name,
          message: entry.error.message,
          stack: entry.error.stack,
        }
      }),
    };

    return JSON.stringify(baseLog);
  }

  private log(level: LogLevel, message: string, context?: Record<string, any>, error?: Error): void {
    if (!this.shouldLog(level)) {
      return;
    }

    const entry: LogEntry = {
      level,
      message,
      timestamp: new Date().toISOString(),
      context,
      error,
    };

    const formattedLog = this.formatLogEntry(entry);

    // In production, use structured JSON logging
    // In development, use console methods for better readability
    if (process.env.NODE_ENV === 'production') {
      console.log(formattedLog);
    } else {
      const consoleMethod = level === 'error' ? console.error : 
                           level === 'warn' ? console.warn : 
                           console.log;
      
      consoleMethod(`[${entry.timestamp}] ${level.toUpperCase()}: ${message}`, 
                   context ? context : '', 
                   error ? error : '');
    }
  }

  error(message: string, context?: Record<string, any>, error?: Error): void {
    this.log('error', message, context, error);
  }

  warn(message: string, context?: Record<string, any>): void {
    this.log('warn', message, context);
  }

  info(message: string, context?: Record<string, any>): void {
    this.log('info', message, context);
  }

  debug(message: string, context?: Record<string, any>): void {
    this.log('debug', message, context);
  }

  // Convenience method for logging HTTP requests
  request(method: string, path: string, statusCode: number, duration: number, context?: Record<string, any>): void {
    this.info('HTTP Request', {
      method,
      path,
      statusCode,
      duration,
      ...context,
    });
  }

  // Convenience method for logging MQTT events
  mqtt(event: string, topic?: string, context?: Record<string, any>): void {
    this.debug('MQTT Event', {
      event,
      topic,
      ...context,
    });
  }
}

export const logger = new Logger();
