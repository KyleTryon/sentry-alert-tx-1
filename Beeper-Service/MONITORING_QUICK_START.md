# Beeper Service Monitoring - Quick Start Guide 🚀

## 🎯 Quick Commands to Monitor Your Service

### 1. Interactive Monitor (Best for Testing)
```bash
# Install dependencies first
npm install

# Run interactive monitor
npm run monitor

# Available commands:
# health - Check if service is healthy
# test   - Send a test alert
# stats  - View message statistics
# quit   - Exit
```

### 2. Real-time MQTT Messages
```bash
# See ALL messages going through MQTT
npm run mqtt:debug
```

### 3. Send Test Alert
```bash
# Quick test webhook
npm run test:webhook

# Or use the test command in monitor
npm run monitor
> test
```

### 4. Check Service Health
```bash
# Quick health check
curl http://localhost:3000/health | jq
```

### 5. View Logs
```bash
# Service logs
docker logs -f beeper-service

# MQTT broker logs
docker logs -f mosquitto
```

## 🔍 What to Look For

### ✅ Everything is Working
- Health endpoint returns `"status": "ok"`
- MQTT shows `"connected": true`
- Test messages appear in mqtt-debug within 1-2 seconds
- No errors in service logs

### ❌ Common Issues

**No MQTT Messages?**
```bash
# 1. Check MQTT connection
curl http://localhost:3000/health | jq .mqtt

# 2. Check if broker is running
docker ps | grep mosquitto

# 3. Check service logs
docker logs beeper-service --tail 20
```

**Webhook Errors?**
```bash
# Check recent logs for webhook errors
docker logs beeper-service 2>&1 | grep -i error | tail -10
```

## 🚨 Full Monitoring Stack (Optional)

For production monitoring with dashboards:
```bash
# Start with monitoring services
docker-compose -f docker-compose.yml -f docker-compose.monitoring.yml up -d

# Access:
# - MQTT Explorer: http://localhost:4000
# - Prometheus: http://localhost:9090  
# - Grafana: http://localhost:3001 (admin/admin)
```

## 📱 Testing End-to-End

1. **Start MQTT Debug** (in terminal 1):
   ```bash
   npm run mqtt:debug
   ```

2. **Send Test Alert** (in terminal 2):
   ```bash
   npm run test:webhook
   ```

3. **You should see**:
   - HTTP 200 response from webhook
   - Message appear in MQTT debug within seconds
   - Your beeper should receive the alert!

## 🆘 Still Having Issues?

1. Check the full monitoring guide: `MONITORING.md`
2. Verify your beeper is subscribed to: `alerts/+/+`
3. Check MQTT authentication if using username/password
4. Ensure all services are running: `docker-compose ps`

---

**Pro Tip**: Keep `npm run monitor` running during testing - it gives you real-time feedback and can send test messages on demand!
