#!/bin/bash

# Digital Ocean Deployment Script for Beeper-Service
# Ubuntu 25 with Docker Compose

set -e

echo "🚀 Starting Digital Ocean deployment setup..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_step() {
    echo -e "${GREEN}[STEP]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
if [[ $EUID -eq 0 ]]; then
    print_warning "This script should be run as a non-root user with sudo privileges"
    print_warning "If this is your first time, create a user first:"
    echo "  adduser deployer"
    echo "  usermod -aG sudo deployer"
    echo "  su - deployer"
    echo ""
fi

# Update system
print_step "Updating system packages..."
sudo apt update && sudo apt upgrade -y

# Install Docker if not already installed
if ! command -v docker &> /dev/null; then
    print_step "Installing Docker..."
    curl -fsSL https://get.docker.com -o get-docker.sh
    sudo sh get-docker.sh
    sudo systemctl enable docker
    sudo systemctl start docker
    sudo usermod -aG docker $USER
    rm get-docker.sh
    print_warning "You may need to log out and back in for Docker group membership to take effect"
else
    print_step "Docker already installed ✓"
fi

# Install Docker Compose Plugin
if ! docker compose version &> /dev/null; then
    print_step "Installing Docker Compose Plugin..."
    sudo apt install docker-compose-plugin -y
else
    print_step "Docker Compose already installed ✓"
fi

# Configure firewall
print_step "Configuring UFW firewall..."
sudo ufw --force enable
sudo ufw allow ssh
sudo ufw allow 3000/tcp  # Beeper-Service
sudo ufw allow 1883/tcp  # MQTT
print_warning "Firewall configured. Consider changing SSH port for additional security."

# Check if repository exists
if [ ! -d "sentry_alert_tx-1" ]; then
    print_step "Repository not found. Please clone it first:"
    echo "  git clone https://github.com/your-username/sentry_alert_tx-1.git"
    echo "  cd sentry_alert_tx-1/Beeper-Service"
    echo "  ./deploy-digital-ocean.sh"
    exit 1
fi

# Navigate to Beeper-Service directory
cd sentry_alert_tx-1/Beeper-Service

# Create environment file if it doesn't exist
if [ ! -f ".env" ]; then
    print_step "Creating environment configuration..."
    cp env.template .env
    
    # Generate a random webhook secret
    WEBHOOK_SECRET=$(openssl rand -hex 32)
    sed -i "s/your-webhook-secret-here/$WEBHOOK_SECRET/" .env
    
    print_warning "Environment file created at .env"
    print_warning "Please edit .env and set your MQTT passwords:"
    echo "  nano .env"
    echo ""
    echo "Generated webhook secret: $WEBHOOK_SECRET"
    echo "Use this in your Sentry webhook configuration."
    echo ""
else
    print_step "Environment file already exists ✓"
fi

# Build and start services
print_step "Building and starting services..."
docker compose up -d --build

# Wait for containers to start
print_step "Waiting for containers to start..."
sleep 10

# Check container status
print_step "Checking container status..."
docker ps

# Display MQTT setup instructions
echo ""
print_step "🔐 MQTT Authentication Setup Required"
echo "Run these commands to set up MQTT authentication:"
echo ""
echo "1. Get MQTT container ID:"
echo "   docker ps | grep mosquitto"
echo ""
echo "2. Set up MQTT users:"
echo "   docker exec -it <mosquitto-container-id> sh"
echo "   mosquitto_passwd -c /mosquitto/config/passwd beeper-service"
echo "   mosquitto_passwd /mosquitto/config/passwd alerttx-device"
echo "   mosquitto_passwd /mosquitto/config/passwd admin"
echo "   exit"
echo ""
echo "3. Restart MQTT container:"
echo "   docker restart <mosquitto-container-id>"
echo ""
echo "4. Update .env file with the passwords you just created"
echo "   nano .env"
echo ""

# Display webhook URL
DROPLET_IP=$(curl -s ifconfig.me 2>/dev/null || echo "YOUR-DROPLET-IP")
print_step "🌐 Webhook URL for Sentry:"
echo "   http://$DROPLET_IP:3000/webhook"
echo ""

# Display health check
print_step "🏥 Testing health endpoint..."
if curl -s http://localhost:3000/health > /dev/null; then
    echo "✓ Health endpoint responding"
else
    print_error "Health endpoint not responding. Check logs:"
    echo "  docker logs beeper-service"
fi

echo ""
print_step "✅ Deployment script completed!"
print_warning "Next steps:"
echo "1. Set up MQTT authentication (see commands above)"
echo "2. Configure your Sentry webhook URL: http://$DROPLET_IP:3000/webhook"
echo "3. Test with: curl http://$DROPLET_IP:3000/health"
echo ""
print_warning "For production, consider:"
echo "- Setting up a domain name and SSL certificate"
echo "- Changing SSH port and disabling root login"
echo "- Setting up automated backups"
echo "- Monitoring with logging and alerting"
