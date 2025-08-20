# Deployment Guide

## 🔐 Security-First Deployment

This guide ensures secrets are never committed to the repository while maintaining easy deployment.

## 📁 Files Safe to Commit

✅ **These files are safe to commit (no secrets):**
```
├── src/                    # All source code
├── Dockerfile             # Container build instructions
├── docker-compose.yml     # Development setup
├── env.template           # Environment variable template
├── mosquitto.conf         # MQTT broker config (dev mode)
├── package.json           # Dependencies
├── README.md              # Documentation
└── .gitignore            # Excludes secrets
```

🚫 **These files are automatically excluded:**
```
.env                       # Your actual secrets
.env.*                     # Any environment files
secrets/                   # Secret files directory
*.key, *.pem              # Certificate files
```

## 🚀 Deployment Methods

### Digital Ocean Droplet (Ubuntu 25)

#### **1. Initial Server Setup**
```bash
# Create droplet (via DO control panel or doctl CLI)
# Recommended: Basic $6/month droplet (1GB RAM, 1 vCPU, 25GB SSD)

# SSH into your droplet
ssh root@your-droplet-ip

# Update system
apt update && apt upgrade -y

# Install Docker
curl -fsSL https://get.docker.com -o get-docker.sh
sh get-docker.sh
systemctl enable docker
systemctl start docker

# Install Docker Compose
apt install docker-compose-plugin -y

# Create non-root user
adduser deployer
usermod -aG docker deployer
usermod -aG sudo deployer
```

#### **2. Security Setup**
```bash
# Configure firewall
ufw enable
ufw allow ssh
ufw allow 22/tcp
ufw allow 80/tcp      # HTTP (Caddy)
ufw allow 443/tcp     # HTTPS (Caddy)
ufw allow 1883/tcp    # MQTT (raw TCP)
ufw status

# Optional: Change SSH port and disable root login
nano /etc/ssh/sshd_config
# Set: Port 2222, PermitRootLogin no
systemctl restart ssh
ufw allow 2222/tcp
ufw delete allow 22/tcp
```

#### **3. Deploy Application**
```bash
# Switch to deployer user
su - deployer

# Clone repository
git clone https://github.com/your-username/sentry_alert_tx-1.git
cd sentry_alert_tx-1/Beeper-Service

# Create production environment file
cp env.template .env
nano .env
```

#### **4. Production Environment Configuration**
```bash
# .env file for production:
NODE_ENV=production
PORT=3000
HOST=0.0.0.0

# Domain used by Caddy (required for SSL)
DOMAIN=your-domain.com

# MQTT Configuration
MQTT_BROKER_URL=mqtt://mosquitto:1883
MQTT_USERNAME=beeper-service
MQTT_PASSWORD=your-strong-password-here
MQTT_CLIENT_ID=beeper-service-prod
MQTT_TOPIC_PREFIX=alerts
MQTT_QOS=1

# Sentry Configuration
SENTRY_WEBHOOK_SECRET=your-webhook-secret-from-sentry
SENTRY_ALLOWED_IPS=

```

> Notes on MQTT networking:
> - The broker in this stack is the `mosquitto` service defined in `docker-compose.yml`.
> - Inside Docker Compose, use `MQTT_BROKER_URL=mqtt://mosquitto:1883` (service name on the bridge network).
> - Do NOT use `localhost` from within the beeper-service container for the broker.
> - External devices (ESP32) should connect to the Docker host’s IP or DNS name on port `1883`.
> - If using WebSockets via Caddy, ensure Mosquitto is listening on `9001` (websockets) and Caddy proxies `ws.<domain>` to `mosquitto:9001`.

#### **5. Choose Deployment Mode**

**Option A: Simple deployment (HTTP only)**
```bash
# Start everything (good for testing)
docker compose up -d

# Check status
docker ps
```

**Option B: With SSL (Caddy) — recommended**
```bash
# Set your domain in .env (e.g., DOMAIN=beeper.example.com)
nano .env

# Bring up the full stack (Beeper-Service, Mosquitto broker, Caddy)
docker compose up -d

# Check status
docker ps
```

#### **6. Configure MQTT Authentication**
```bash
# Get MQTT container ID
docker ps | grep mosquitto

# Set up MQTT users
docker exec -it <mosquitto-container-id> sh
mosquitto_passwd -c /mosquitto/config/passwd beeper-service
mosquitto_passwd /mosquitto/config/passwd alerttx-device
mosquitto_passwd /mosquitto/config/passwd admin
exit

# Restart MQTT broker
docker restart <mosquitto-container-id>
```

#### **7. Configure Sentry Webhook**

**Simple HTTP deployment:**
```bash
# Your webhook URL will be:
http://your-droplet-ip:3000/webhook

# Test the endpoint:
curl -X GET http://your-droplet-ip:3000/health
```

**Production SSL deployment (Caddy):**
```bash
# Your webhook URLs will be:
https://$DOMAIN/webhook         # Main API
wss://ws.$DOMAIN/               # WebSocket MQTT

# Test the endpoints:
curl -X GET https://$DOMAIN/health
```

#### **8. Production Monitoring & Maintenance**
```bash
# View service logs
docker compose logs -f beeper-service
docker compose logs -f mosquitto

# Monitor resource usage
docker stats

# Check service health
curl http://localhost:3000/health

# Restart services if needed
docker compose restart

# Update application
git pull origin main
docker compose down
docker compose up -d --build
```

#### **9. Caddy and Domain Setup (SSL)**

Use Caddy (already included in `docker-compose.yml`) for automatic HTTPS via Let’s Encrypt.

```bash
# 1) Set your domain in .env
DOMAIN=your-domain.com

# 2) Create DNS A records pointing to your droplet IP
#    - A  your-domain.com   -> <droplet-ip>
#    - A  ws.your-domain.com -> <droplet-ip>
#    Wait for DNS to propagate (often within minutes).

# 3) Ensure firewall allows 80, 443, and 1883
ufw allow 80/tcp
ufw allow 443/tcp
ufw allow 1883/tcp

# 4) Start the stack
docker compose up -d

# 5) Verify that Caddy obtained certificates
docker compose logs -f caddy | sed -n '1,120p'

# 6) Validate endpoints
curl -I https://$DOMAIN/health
```

> About `DOMAIN` usage:
> - `docker-compose.yml` passes `DOMAIN` to the Caddy container.
> - `Caddyfile` uses the `{$DOMAIN}` placeholder to configure the virtual hosts for both `https://$DOMAIN` and `wss://ws.$DOMAIN`.
> - The Node.js app does not use `DOMAIN`; it listens on `HOST:PORT` and is reverse-proxied by Caddy.

#### **10. Backup Strategy**
```bash
# Backup MQTT configuration and passwords
tar -czf mqtt-backup-$(date +%Y%m%d).tar.gz passwd acl mosquitto.conf

# Store backups securely (e.g., Digital Ocean Spaces)
# Consider automated backups for production
```

### Development (Local)

1. **Create your secrets file:**
   ```bash
   cd Beeper-Service
   cp env.template .env
   nano .env  # Add your real secrets
   ```

2. **Start with Docker Compose:**
   ```bash
   docker-compose up -d
   ```

> Development tips:
> - Inside Compose, `beeper-service` connects to `mosquitto` via `mqtt://mosquitto:1883`.
> - Your ESP32 or other clients should connect to your machine’s IP (not `mosquitto`) on port `1883`.
> - Avoid `localhost` as broker from devices; it refers to the device itself, not your host.

### Production (Docker)

1. **Build the image (no secrets needed):**
   ```bash
   docker build -t beeper-service:latest .
   ```

2. **Run with environment variables:**
   ```bash
   docker run -d \
     --name beeper-service \
     -p 3000:3000 \
     -e MQTT_BROKER_URL="mqtt://your-broker:1883" \
     -e MQTT_USERNAME="your-username" \
     -e MQTT_PASSWORD="your-password" \
     -e SENTRY_WEBHOOK_SECRET="your-secret" \
     beeper-service:latest
   ```

3. **Or use an external secrets file:**
   ```bash
   docker run -d \
     --name beeper-service \
     -p 3000:3000 \
     --env-file /secure/path/.env \
     beeper-service:latest
   ```

### Production (Kubernetes)

1. **Create Secret:**
   ```yaml
   apiVersion: v1
   kind: Secret
   metadata:
     name: beeper-service-secrets
   type: Opaque
   stringData:
     MQTT_BROKER_URL: "mqtt://your-broker:1883"
     MQTT_USERNAME: "your-username"
     MQTT_PASSWORD: "your-password"
     SENTRY_WEBHOOK_SECRET: "your-secret"
   ```

2. **Deploy with Secrets:**
   ```yaml
   apiVersion: apps/v1
   kind: Deployment
   metadata:
     name: beeper-service
   spec:
     template:
       spec:
         containers:
         - name: beeper-service
           image: beeper-service:latest
           envFrom:
           - secretRef:
               name: beeper-service-secrets
   ```

## 🔍 Security Verification

### Before Committing
```bash
# Check for accidentally committed secrets
git log --patch | grep -E "(password|secret|key).*="

# Scan for potential secrets in files
grep -r -E "(password|secret|key).*=.*['\"][^'\"\s]{8,}" src/
```

### After Deployment
```bash
# Verify no secrets in logs
docker logs beeper-service 2>&1 | grep -E "(password|secret|key)"

# Test endpoint security
curl -H "Content-Type: application/json" \
     -d '{"test": "invalid"}' \
     http://localhost:3000/webhook
# Should return 400/401, not expose internals
```

## 📋 Required Secrets

### Minimum Required
- `MQTT_BROKER_URL` - Your MQTT broker connection string

### Production Recommended
- `MQTT_USERNAME` - MQTT authentication username
- `MQTT_PASSWORD` - MQTT authentication password  
- `SENTRY_WEBHOOK_SECRET` - Webhook signature verification

### Optional Security
- `SENTRY_ALLOWED_IPS` - Comma-separated IP allowlist
- `LOG_LEVEL=warn` - Reduce log verbosity in production

## 🛡️ Security Best Practices

1. **Never commit secrets** - Use environment variables
2. **Rotate secrets** - Change passwords/keys regularly
3. **Least privilege** - Only give required MQTT permissions
4. **Network security** - Use TLS for MQTT (mqtts://)
5. **Monitor access** - Watch for failed authentication attempts

## 🚨 If Secrets Are Accidentally Committed

1. **Immediately rotate** all exposed secrets
2. **Remove from git history:**
   ```bash
   git filter-branch --force --index-filter \
     'git rm --cached --ignore-unmatch .env' \
     --prune-empty --tag-name-filter cat -- --all
   ```
3. **Force push** (if safe): `git push --force-with-lease`
4. **Update all deployments** with new secrets
