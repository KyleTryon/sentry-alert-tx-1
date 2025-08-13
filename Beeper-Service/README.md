# Beeper-Service

## Overview

**Beeper-Service** is a lightweight MQTT-based message relay designed to bridge Sentry alert webhooks to IoT devices (such as the AlertTX-1 beeper). It acts as a cloud-based queue and delivery system, ensuring that critical alerts from Sentry are reliably pushed to subscribed devices in real time.

## Features

- **Receives Sentry Webhooks:** Accepts incoming HTTP POST requests from Sentry's alerting system.
- **MQTT Message Queue:** Publishes alert payloads to an MQTT topic for consumption by devices.
- **Cloud-Native Deployment:** Designed for serverless platforms like Cloudflare Workers or Vercel Serverless Functions.
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

Beeper-Service is designed for modern serverless/cloud platforms:

### Cloudflare Workers
- **Recommended** for global low-latency delivery and edge compute.
- Deploy using Wrangler CLI or Cloudflare dashboard.
- Integrate with a managed MQTT broker (e.g., CloudMQTT, HiveMQ, EMQX Cloud).

### Vercel Serverless Functions
- Alternative for easy deployment and integration with Vercel projects.
- Deploy via Vercel CLI or GitHub integration.
- Use environment variables for MQTT broker credentials.

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

## Development

- Written in TypeScript/JavaScript (Node.js runtime for Vercel, Service Worker API for Cloudflare).
- Uses a lightweight MQTT client (e.g., `mqtt.js`).
- Minimal dependencies for fast cold starts.

## License

MIT License