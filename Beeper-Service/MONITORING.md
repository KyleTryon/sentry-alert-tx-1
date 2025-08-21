# Beeper Service Monitoring Guide

This guide covers various methods to monitor your Sentry-to-Beeper bridge service and ensure messages are properly delivered.

## Table of Contents
- [Overview](#overview)
- [Quick Start](#quick-start)
- [Monitoring Tools](#monitoring-tools)
- [Troubleshooting Message Delivery](#troubleshooting-message-delivery)
- [Automated Monitoring](#automated-monitoring)
- [Metrics and Dashboards](#metrics-and-dashboards)

## Overview

The Beeper Service has multiple points where message delivery can be monitored:

1. **Webhook Reception** - Sentry → Beeper Service
2. **MQTT Publishing** - Beeper Service → MQTT Broker
3. **MQTT Delivery** - MQTT Broker → Beeper Device

## Quick Start

### 1. Interactive Monitor (Recommended)

The quickest way to monitor your service:

```bash
# Install dependencies
npm install mqtt axios chalk

# Run the interactive monitor
node monitor.js

# Commands:
# - health (h)  : Check service health
# - test (t)    : Send a test webhook
# - stats (s)   : View message statistics
# - clear (c)   : Clear screen
# - quit (q)    : Exit
```

### 2. MQTT Debug Subscriber

Monitor all MQTT messages in real-time:

```bash
# Subscribe to all alert topics
node mqtt-debug.js

# With custom broker
MQTT_BROKER_URL=mqtt://your-broker:1883 node mqtt-debug.js

# With authentication
MQTT_USERNAME=user MQTT_PASSWORD=pass node mqtt-debug.js
```

### 3. Check Service Health

```bash
# Using curl
curl http://localhost:3000/health

# Using the monitor
node monitor.js
> health
```

## Monitoring Tools

### 1. Built-in Health Endpoint

The service exposes a `/health` endpoint that provides:
- Service status
- Uptime
- MQTT connection status
- Queued messages count
- Reconnection attempts

Example response:
```json
{
  "status": "ok",
  "timestamp": "2024-01-15T10:30:00.000Z",
  "uptime": 3600000,
  "version": "1.0.0",
  "environment": "production",
  "mqtt": {
    "connected": true,
    "reconnectAttempts": 0,
    "queuedMessages": 0,
    "brokerUrl": "mqtt://mosquitto:1883",
    "clientId": "beeper-service"
  }
}
```

### 2. Docker Compose with Monitoring Stack

Start the full monitoring stack:

```bash
# Start with monitoring services
docker-compose -f docker-compose.yml -f docker-compose.monitoring.yml up -d

# Access monitoring tools:
# - MQTT Explorer: http://localhost:4000
# - Prometheus: http://localhost:9090
# - Grafana: http://localhost:3001 (admin/admin)
```

### 3. Service Logs

Monitor service logs in real-time:

```bash
# Docker logs
docker logs -f beeper-service

# Filter for specific events
docker logs beeper-service 2>&1 | grep "MQTT"
docker logs beeper-service 2>&1 | grep "webhook"
```

## Troubleshooting Message Delivery

### Step 1: Verify Webhook Reception

Check if Sentry webhooks are being received:

```bash
# Send a test webhook
curl -X POST http://localhost:3000/webhook \
  -H "Content-Type: application/json" \
  -H "sentry-hook-resource: event_alert" \
  -H "sentry-hook-timestamp: $(date +%s)" \
  -d '{
    "action": "event.alert",
    "installation": {"uuid": "test-123"},
    "data": {
      "event": {
        "title": "Test Alert",
        "message": "Testing message delivery",
        "level": "warning"
      }
    }
  }'
```

Expected response:
```json
{
  "status": "success",
  "message": "Webhook processed successfully",
  "requestId": "req_1234567890"
}
```

### Step 2: Verify MQTT Publishing

1. Start the MQTT debug subscriber:
   ```bash
   node mqtt-debug.js
   ```

2. Send a test webhook (as shown above)

3. You should see the message appear in the debug subscriber within seconds

### Step 3: Check MQTT Broker

Using mosquitto clients:

```bash
# Subscribe to all topics
mosquitto_sub -h localhost -p 1883 -t "alerts/#" -v

# With authentication
mosquitto_sub -h localhost -p 1883 -u username -P password -t "alerts/#" -v
```

### Step 4: Verify Device Subscription

Ensure your beeper device is:
1. Connected to the MQTT broker
2. Subscribed to the correct topics
3. Processing messages properly

## Automated Monitoring

### Continuous Health Checks

Run the monitor in automated mode:

```bash
# Run with auto health checks every 5 seconds
node monitor.js --auto

# Also send test webhooks every minute
AUTO_TEST=true node monitor.js --auto
```

### Systemd Service (Linux)

Create `/etc/systemd/system/beeper-monitor.service`:

```ini
[Unit]
Description=Beeper Service Monitor
After=network.target

[Service]
Type=simple
User=beeper
WorkingDirectory=/opt/beeper-service
ExecStart=/usr/bin/node /opt/beeper-service/monitor.js --auto
Restart=always
Environment="AUTO_TEST=true"
Environment="SERVICE_URL=http://localhost:3000"
Environment="MQTT_BROKER_URL=mqtt://localhost:1883"

[Install]
WantedBy=multi-user.target
```

Enable and start:
```bash
sudo systemctl enable beeper-monitor
sudo systemctl start beeper-monitor
sudo systemctl status beeper-monitor
```

## Metrics and Dashboards

### Prometheus Queries

Useful queries for monitoring:

```promql
# MQTT messages per minute
rate(beeper_mqtt_messages_total[1m])

# Failed webhook attempts
beeper_webhook_errors_total

# Service uptime
time() - beeper_service_start_time_seconds

# MQTT connection status
beeper_mqtt_connected
```

### Grafana Dashboard

Import the dashboard JSON from `monitoring/grafana/dashboards/beeper-service.json` or create your own with:

1. **Service Health Panel**
   - Uptime gauge
   - MQTT connection status
   - Webhook success rate

2. **Message Flow Panel**
   - Messages per minute
   - Average latency
   - Failed deliveries

3. **System Metrics Panel**
   - CPU usage
   - Memory usage
   - Network I/O

### Alerts

Configure alerts in Prometheus/Grafana for:

1. **Service Down**
   ```yaml
   - alert: BeeperServiceDown
     expr: up{job="beeper-service"} == 0
     for: 1m
   ```

2. **MQTT Disconnected**
   ```yaml
   - alert: MQTTDisconnected
     expr: beeper_mqtt_connected == 0
     for: 5m
   ```

3. **High Message Queue**
   ```yaml
   - alert: HighMessageQueue
     expr: beeper_mqtt_queued_messages > 100
     for: 5m
   ```

## Common Issues and Solutions

### Issue: Messages not appearing in MQTT

1. **Check service health:**
   ```bash
   curl http://localhost:3000/health | jq .mqtt
   ```

2. **Verify MQTT broker is running:**
   ```bash
   docker ps | grep mosquitto
   nc -zv localhost 1883
   ```

3. **Check service logs for errors:**
   ```bash
   docker logs beeper-service --tail 50
   ```

### Issue: Webhook returns 401 Unauthorized

1. Verify webhook secret is configured correctly
2. Check timestamp is recent (within 5 minutes)
3. Ensure signature is properly calculated

### Issue: High latency

1. Check network connectivity
2. Monitor MQTT broker performance
3. Check for message queue buildup
4. Verify device processing speed

## Best Practices

1. **Regular Health Checks**: Run automated health checks every 5-10 seconds
2. **Test Messages**: Send periodic test messages to verify end-to-end flow
3. **Log Monitoring**: Set up log aggregation for easier troubleshooting
4. **Metrics Collection**: Use Prometheus/Grafana for historical analysis
5. **Alerting**: Configure alerts for critical issues
6. **Documentation**: Keep track of any custom configurations or modifications

## Support

For issues or questions:
1. Check service logs first
2. Verify all components are running
3. Test each component individually
4. Review this monitoring guide
5. Check MQTT broker logs
