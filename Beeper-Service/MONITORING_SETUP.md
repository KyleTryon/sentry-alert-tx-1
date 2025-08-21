# Setting Up Monitoring Tools

## Prerequisites

Before using the monitoring tools, install the required dependencies:

```bash
# Install monitoring dependencies
npm install

# If you get version conflicts, you can use:
npm install --force

# Or install specific packages:
npm install axios chalk@5.3.0
```

## Files Created for Monitoring

1. **monitor.js** - Interactive monitoring tool
   - Health checks
   - Send test webhooks
   - Track message delivery time
   - Works for both local and remote deployments

2. **mqtt-debug.js** - MQTT message viewer
   - Subscribe to all alert topics
   - View message payloads in real-time
   - Debug MQTT connectivity

3. **monitor-remote.sh** - Pre-configured remote monitoring
   - Edit once with your Digital Ocean details
   - Automatically sets up environment
   - Quick health check on startup

4. **test-webhook.sh** - Test webhook sender
   - Send manual test alerts
   - Works with local or remote deployments
   - Useful for debugging

## Quick Commands

```bash
# Monitor local development
npm run monitor

# Monitor remote deployment (after editing monitor-remote.sh)
npm run monitor:remote

# Debug MQTT messages
npm run mqtt:debug

# Send test webhook
npm run test:webhook
```

## Environment Variables

For remote monitoring, you can set these:

```bash
export SERVICE_URL="http://your-droplet-ip:3000"
export MQTT_BROKER_URL="mqtt://your-droplet-ip:1883"
export MQTT_USERNAME="beeper-service"
export MQTT_PASSWORD="your-password"
```

Or just edit `monitor-remote.sh` once and use that!
