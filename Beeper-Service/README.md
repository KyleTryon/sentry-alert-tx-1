# Beeper-Service

## Overview

**Beeper-Service** is a lightweight MQTT-based message relay designed to bridge Sentry alert webhooks to IoT devices (such as the AlertTX-1 beeper). It acts as a cloud-based queue and delivery system, ensuring that critical alerts from Sentry are reliably pushed to subscribed devices in real time.

## Features

- **Receives Sentry Webhooks:** Accepts incoming HTTP POST requests from Sentry's alerting system.
- **MQTT Message Queue:** Publishes alert payloads to an MQTT topic for consumption by devices.
- **Container-friendly Deployment:** Runs as a long-lived Node.js service (Docker, Docker Compose, Kubernetes, Fly.io, Railway, Render, etc.).
- **Built-in MQTT Broker:** Ships with Eclipse Mosquitto (via Docker Compose) and optional WSS via Caddy.
- **Queueing & Reliability:** Ensures messages are queued and delivered even if devices are temporarily offline.
- **Simple REST API:** Exposes endpoints for health checks and (optionally) manual message injection/testing.

## Architecture

```
[Sentry] --(webhook)--> [Beeper-Service] --(MQTT)--> [AlertTX-1 Device(s)]
```

- **Sentry**: Sends alert webhooks (JSON) to Beeper-Service.
- **Beeper-Service**: Receives, validates, and queues the alert as an MQTT message.
- **AlertTX-1**: Subscribes to the MQTT topic and receives alerts in real time.

## Deployment

Beeper-Service is designed to run as a persistent process. Recommended options:

### Docker Compose (recommended)
- Use the provided `docker-compose.yml` to run the Beeper-Service together with an embedded Mosquitto broker and Caddy for SSL.
- Configure environment via `.env` (see `env.template`). Set `DOMAIN` for SSL.

### Docker
- Build the Docker image using the provided `Dockerfile` and run it on your platform of choice.

### Kubernetes
- Deploy as a `Deployment`/`Service` pair and point it at your MQTT broker (managed or self-hosted).

### Managed app platforms
- Works well on long-running container platforms like Fly.io, Railway, Render, DigitalOcean Apps, or a simple VM.

### DigitalOcean Deployment
- Recommended to run on a small Droplet (e.g., 1GB RAM). Open ports 80/443 (Caddy) and 1883 (raw MQTT). Use the compose stack as-is.

### Note on serverless platforms
- Traditional serverless/FaaS (e.g., Cloudflare Workers, Vercel Serverless Functions) are generally not suitable for MQTT publishing because they are short-lived and do not allow persistent TCP connections. This project targets standard Node.js/container environments.

## Example Workflow

1. **Sentry Alert Triggered:**
   - Sentry sends a webhook (HTTP POST) to the Beeper-Service endpoint.
2. **Message Queued:**
   - Beeper-Service parses the payload and publishes it to an MQTT topic (e.g., `alerts/critical`).
3. **Device Receives Alert:**
   - AlertTX-1 (or any MQTT client) receives the message and notifies the user.

## Configuration

- **MQTT Broker:**
  - Set via environment variable (e.g., `MQTT_BROKER_URL`).
- **MQTT Topic:**
  - Default: `alerts/critical` (configurable).
- **Authentication:**
  - Use MQTT username/password or token as required by your broker.
- **Webhook Secret:**
  - (Optional) Validate incoming webhooks with a shared secret.

## Endpoints

- `POST /webhook` — Receives Sentry webhook payloads and queues them to MQTT.
- `GET /health` — Health check endpoint for monitoring/deployment.

## Example Sentry Webhook Payload
```json
{
  "event": {
    "title": "New Error: NullReferenceException",
    "url": "https://sentry.io/organizations/example/issues/123456/",
    "level": "error",
    "timestamp": 1680000000
  },
  "project": "my-app",
  "triggered_rule": "Critical Errors"
}
```

## Security
- Validate incoming requests (IP allowlist, secret, or signature).
- Never expose MQTT credentials in client-side code.
- Use TLS/SSL for all MQTT and webhook traffic.

## Monitoring

The service includes comprehensive monitoring tools to ensure reliable message delivery:

- **Interactive Monitor** - Real-time health checks and test messages
- **MQTT Debug Client** - View all messages flowing through the system  
- **Remote Monitoring** - Monitor your Digital Ocean deployment from anywhere

Quick start monitoring:
```bash
# For local development
npm run monitor

# For remote deployment (edit monitor-remote.sh first)
./monitor-remote.sh
```

## Documentation

- [Deployment Guide](DEPLOYMENT.md) - Production deployment instructions
- [MQTT Setup](MQTT_SETUP.md) - Detailed MQTT configuration
- [Docker Optimization](DOCKER_OPTIMIZATION.md) - Container optimization details
- [Monitoring Guide](MONITORING.md) - Comprehensive monitoring documentation
- [Remote Monitoring](REMOTE_MONITORING.md) - Monitor Digital Ocean deployments
- [Quick Start Remote Monitoring](MONITORING_REMOTE_QUICKSTART.md) - Get monitoring in 3 steps

## Development

- Written in TypeScript/JavaScript (Node.js runtime).
- Uses a lightweight MQTT client (`mqtt.js`).
- Minimal dependencies.

## License

MIT License