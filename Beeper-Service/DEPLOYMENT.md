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
ufw allow 3000/tcp    # Beeper-Service HTTP
ufw allow 1883/tcp    # MQTT
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

# Production Logging
LOG_LEVEL=warn
```

#### **5. Choose Deployment Mode**

**Option A: Simple deployment (HTTP only)**
```bash
# Start everything (good for testing)
docker compose up -d

# Check status
docker ps
```

**Option B: Production with SSL (recommended)**
```bash
# Copy production environment
cp env.prod.template .env
nano .env  # Set DOMAIN=your-domain.com

# Start with Caddy SSL
docker compose -f docker-compose.prod.yml up -d

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

**Production SSL deployment:**
```bash
# Your webhook URLs will be:
https://your-domain.com/webhook         # Main API
wss://ws.your-domain.com/               # WebSocket MQTT

# Test the endpoints:
curl -X GET https://your-domain.com/health
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

#### **9. Optional: SSL/Domain Setup**

If you want to use a domain name and SSL:

```bash
# Install nginx and certbot
apt install nginx certbot python3-certbot-nginx -y

# Configure nginx reverse proxy
nano /etc/nginx/sites-available/beeper-service
```

**Nginx configuration:**
```nginx
server {
    listen 80;
    server_name your-domain.com;

    location / {
        proxy_pass http://localhost:3000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

```bash
# Enable site
ln -s /etc/nginx/sites-available/beeper-service /etc/nginx/sites-enabled/
nginx -t
systemctl reload nginx

# Get SSL certificate
certbot --nginx -d your-domain.com

# Update UFW for HTTPS
ufw allow 80/tcp
ufw allow 443/tcp
ufw delete allow 3000/tcp  # Remove direct access
```

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

### Cloud Providers

#### AWS ECS
```bash
# Store secrets in AWS Systems Manager
aws ssm put-parameter \
  --name "/beeper-service/mqtt-password" \
  --value "your-password" \
  --type "SecureString"

# Reference in ECS task definition
"secrets": [
  {
    "name": "MQTT_PASSWORD",
    "valueFrom": "/beeper-service/mqtt-password"
  }
]
```

#### Google Cloud Run
```bash
# Store secrets in Secret Manager
echo "your-secret" | gcloud secrets create sentry-webhook-secret --data-file=-

# Deploy with secrets
gcloud run deploy beeper-service \
  --image=gcr.io/PROJECT/beeper-service \
  --set-secrets="SENTRY_WEBHOOK_SECRET=sentry-webhook-secret:latest"
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
