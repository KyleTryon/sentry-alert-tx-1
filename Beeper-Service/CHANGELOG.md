# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2024-01-XX

### Added
- ✨ Initial release of Beeper-Service
- 🔐 Sentry webhook signature verification with HMAC-SHA256
- 📡 MQTT client with automatic reconnection and message queuing
- 🏥 Health check endpoints with MQTT status monitoring
- 🐳 Optimized Docker configuration with multi-stage builds
- 📝 Comprehensive TypeScript types for Sentry webhooks
- 🛡️ Request validation with Zod schemas
- 📊 Structured JSON logging with configurable levels
- 🔄 Graceful shutdown handling
- 🏗️ Production-ready error handling and monitoring
- 📖 Complete documentation and examples

### Technical Details
- Built with Fastify for high-performance HTTP handling
- MQTT.js client with exponential backoff reconnection
- TypeScript with strict type checking
- PNPM for efficient package management
- Alpine Linux base for minimal container size
- Non-root container execution for security
