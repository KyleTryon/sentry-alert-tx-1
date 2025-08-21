# Remote Monitoring Guide - Digital Ocean Deployment

This guide shows how to monitor your Beeper Service deployed on Digital Ocean from your local machine.

## 🚀 Quick Start

### 1. Set Your Remote URLs

First, export your Digital Ocean droplet details:

```bash
# Using IP address (no SSL)
export SERVICE_URL="http://YOUR-DROPLET-IP:3000"
export MQTT_BROKER_URL="mqtt://YOUR-DROPLET-IP:1883"

# OR using domain (with SSL via Caddy)
export SERVICE_URL="https://your-domain.com"
export MQTT_BROKER_URL="mqtt://your-domain.com:1883"

# Set MQTT credentials (from your .env on the server)
export MQTT_USERNAME="beeper-service"
export MQTT_PASSWORD="your-mqtt-password"
```

### 2. Monitor Service Health

```bash
# Quick health check
curl $SERVICE_URL/health | jq

# Run interactive monitor
npm run monitor

# Or directly:
SERVICE_URL=$SERVICE_URL MQTT_BROKER_URL=$MQTT_BROKER_URL npm run monitor
```

### 3. Monitor MQTT Messages

```bash
# Subscribe to all MQTT messages from your beeper
npm run mqtt:debug

# With credentials:
MQTT_BROKER_URL=$MQTT_BROKER_URL \
MQTT_USERNAME=$MQTT_USERNAME \
MQTT_PASSWORD=$MQTT_PASSWORD \
npm run mqtt:debug
```

### 4. Send Test Webhook

```bash
# Using the test script
SERVICE_URL=$SERVICE_URL npm run test:webhook

# Or manually with curl
curl -X POST "$SERVICE_URL/webhook" \
  -H "Content-Type: application/json" \
  -H "sentry-hook-resource: event_alert" \
  -H "sentry-hook-timestamp: $(date +%s)" \
  -d '{
    "action": "event.alert",
    "installation": {"uuid": "test-123"},
    "data": {
      "event": {
        "title": "Remote Test Alert",
        "message": "Testing from local machine",
        "level": "warning"
      }
    }
  }'
```

## 📊 Monitoring Dashboard Setup

### Option 1: SSH Tunnel (Recommended for Security)

If you have monitoring services (Grafana, Prometheus) running on your droplet but not exposed publicly:

```bash
# Create SSH tunnel to access monitoring services
ssh -L 3001:localhost:3001 \
    -L 9090:localhost:9090 \
    -L 4000:localhost:4000 \
    deployer@YOUR-DROPLET-IP

# Now access locally:
# - Grafana: http://localhost:3001
# - Prometheus: http://localhost:9090
# - MQTT Explorer: http://localhost:4000
```

### Option 2: Direct Access (Less Secure)

If you've exposed the monitoring ports:

```bash
# Access directly (ensure firewall allows these ports)
# - Grafana: http://YOUR-DROPLET-IP:3001
# - Prometheus: http://YOUR-DROPLET-IP:9090
# - MQTT Explorer: http://YOUR-DROPLET-IP:4000
```

## 🔍 Remote Debugging Commands

### Check Service Logs

```bash
# SSH into your droplet
ssh deployer@YOUR-DROPLET-IP

# View service logs
docker logs -f beeper-service --tail 100

# View MQTT logs
docker logs -f mosquitto --tail 100

# Check all containers
docker ps
docker stats
```

### Test MQTT Connection

From your local machine:

```bash
# Test MQTT connection with mosquitto_sub
mosquitto_sub -h YOUR-DROPLET-IP -p 1883 \
  -u $MQTT_USERNAME -P $MQTT_PASSWORD \
  -t "alerts/#" -v

# Test publishing
mosquitto_pub -h YOUR-DROPLET-IP -p 1883 \
  -u $MQTT_USERNAME -P $MQTT_PASSWORD \
  -t "alerts/test/ping" -m "Hello from local"
```

## 📝 Create Monitoring Script

Save this as `monitor-remote.sh` for easy monitoring:

```bash
#!/bin/bash

# Your Digital Ocean deployment details
export SERVICE_URL="http://YOUR-DROPLET-IP:3000"
export MQTT_BROKER_URL="mqtt://YOUR-DROPLET-IP:1883"
export MQTT_USERNAME="beeper-service"
export MQTT_PASSWORD="your-mqtt-password"

echo "🔍 Monitoring Beeper Service at $SERVICE_URL"
echo ""

# Check health
echo "📊 Service Health:"
curl -s $SERVICE_URL/health | jq .

echo ""
echo "🚀 Starting interactive monitor..."
echo ""

# Start monitor
npm run monitor
```

Make it executable:
```bash
chmod +x monitor-remote.sh
./monitor-remote.sh
```

## 🛠️ Troubleshooting Remote Issues

### Connection Refused

```bash
# Check if service is running on droplet
ssh deployer@YOUR-DROPLET-IP
docker ps | grep beeper-service

# Check firewall
sudo ufw status

# Ensure ports are open
sudo ufw allow 3000/tcp  # HTTP API
sudo ufw allow 1883/tcp  # MQTT
```

### MQTT Authentication Failed

```bash
# On the droplet, check MQTT passwords
docker exec -it $(docker ps -q -f name=mosquitto) cat /mosquitto/config/passwd

# Reset password if needed
docker exec -it $(docker ps -q -f name=mosquitto) mosquitto_passwd /mosquitto/config/passwd beeper-service
```

### SSL/TLS Issues

If using Caddy with HTTPS:

```bash
# Check Caddy logs
ssh deployer@YOUR-DROPLET-IP
docker logs caddy --tail 50

# Verify DNS is pointing to droplet
nslookup your-domain.com

# Test HTTPS endpoint
curl -v https://your-domain.com/health
```

## 🔔 Setting Up Alerts

### Email Alerts (using ssmtp)

On your droplet:

```bash
# Install ssmtp
sudo apt-get install ssmtp mailutils

# Configure /etc/ssmtp/ssmtp.conf
# Add monitoring script that sends email on failure
```

### Slack/Discord Webhook Alerts

Create a monitoring script on the droplet:

```bash
#!/bin/bash
# /home/deployer/monitor-health.sh

HEALTH=$(curl -s http://localhost:3000/health)
STATUS=$(echo $HEALTH | jq -r .status)

if [ "$STATUS" != "ok" ]; then
  # Send alert to Slack/Discord
  curl -X POST https://hooks.slack.com/services/YOUR/WEBHOOK/URL \
    -H 'Content-type: application/json' \
    -d "{\"text\":\"⚠️ Beeper Service is unhealthy: $HEALTH\"}"
fi
```

Add to crontab:
```bash
*/5 * * * * /home/deployer/monitor-health.sh
```

## 📈 Performance Monitoring

### Monitor from Local Machine

```bash
# Continuous latency monitoring
while true; do
  START=$(date +%s%N)
  curl -s $SERVICE_URL/health > /dev/null
  END=$(date +%s%N)
  LATENCY=$((($END - $START) / 1000000))
  echo "Health check latency: ${LATENCY}ms"
  sleep 5
done
```

### Monitor Message Delivery Time

Use the interactive monitor to track end-to-end message delivery:

```bash
npm run monitor
> test  # Sends test message and tracks delivery time
```

## 🔐 Security Best Practices

1. **Use SSH Tunnels**: Don't expose monitoring ports publicly
2. **Strong MQTT Passwords**: Use complex passwords for MQTT
3. **Firewall Rules**: Only open necessary ports
4. **VPN Access**: Consider using a VPN for monitoring
5. **Read-Only MQTT User**: Create a separate MQTT user for monitoring

```bash
# On droplet: Create read-only MQTT user for monitoring
docker exec -it $(docker ps -q -f name=mosquitto) mosquitto_passwd /mosquitto/config/passwd monitor-readonly
```

## 📱 Mobile Monitoring

### Using Termux (Android)

```bash
# Install Termux from F-Droid
# In Termux:
pkg install nodejs mosquitto-clients curl jq
npm install mqtt axios chalk

# Copy monitoring scripts and run
SERVICE_URL="http://YOUR-DROPLET-IP:3000" node monitor.js
```

### Using SSH Client Apps

- **iOS**: Terminus, Blink Shell
- **Android**: JuiceSSH, Termux

Connect to your droplet and run monitoring commands directly.

## 🚨 Emergency Commands

If something goes wrong:

```bash
# Quick restart everything
ssh deployer@YOUR-DROPLET-IP
cd sentry_alert_tx-1/Beeper-Service
docker-compose restart

# Force rebuild and restart
docker-compose down
docker-compose up -d --build

# Check disk space
df -h

# Check memory
free -m

# View all logs
docker-compose logs --tail 100
```

Remember to replace `YOUR-DROPLET-IP` with your actual Digital Ocean droplet IP address!
