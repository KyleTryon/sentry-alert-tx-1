# 🚀 Remote Monitoring Quick Start

Monitor your Beeper Service on Digital Ocean in 3 easy steps!

## Step 1: Configure Your Connection

Edit `monitor-remote.sh` with your deployment details:

```bash
nano monitor-remote.sh
```

Change these lines:
```bash
DROPLET_IP="YOUR-DROPLET-IP"          # Your droplet IP (e.g., "159.223.100.50")
MQTT_PASSWORD="your-mqtt-password-here" # Your MQTT password from server .env
```

## Step 2: Run the Monitor

```bash
./monitor-remote.sh
```

This will:
- ✅ Check if your service is healthy
- ✅ Verify MQTT connection
- ✅ Start the interactive monitor

## Step 3: Test Your Setup

In the monitor, type:
```
test
```

This sends a test alert. You should see:
1. HTTP 200 response (webhook accepted)
2. Message delivered via MQTT
3. Your beeper device should receive it!

---

## 🔍 Monitor MQTT Messages

In a separate terminal:

```bash
# Set your droplet details
export MQTT_BROKER_URL="mqtt://YOUR-DROPLET-IP:1883"
export MQTT_USERNAME="beeper-service"
export MQTT_PASSWORD="your-mqtt-password"

# Watch all messages
npm run mqtt:debug
```

---

## 🆘 Troubleshooting

**Can't connect?**
```bash
# SSH into your droplet and check:
ssh deployer@YOUR-DROPLET-IP
docker ps                      # Is beeper-service running?
docker logs beeper-service -n 50  # Check for errors
```

**MQTT authentication failed?**
```bash
# On your droplet, verify MQTT is running:
docker ps | grep mosquitto
docker logs mosquitto -n 20
```

**Need the webhook URL for Sentry?**
```
http://YOUR-DROPLET-IP:3000/webhook
```
Or with SSL:
```
https://your-domain.com/webhook
```

---

## 📱 Pro Tips

1. **Keep monitor running** during testing - instant feedback!
2. **Use two terminals**: One for monitor, one for MQTT debug
3. **Save your config**: The `monitor-remote.sh` remembers your settings
4. **Check latency**: Monitor shows message delivery time

Need more help? See the full guide: `REMOTE_MONITORING.md`
