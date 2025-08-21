#!/bin/bash

# Remote Monitoring Script for Digital Ocean Deployment
# Edit the variables below with your deployment details

# ============================================
# EDIT THESE WITH YOUR DEPLOYMENT DETAILS
# ============================================

# Your Digital Ocean droplet IP or domain
DROPLET_IP="shoutify.org"  # e.g., "159.223.100.50"
# OR use your domain if you have SSL setup
# DROPLET_IP="your-domain.com"

# MQTT credentials (from your server's .env file)
MQTT_USERNAME="beeper-service"
MQTT_PASSWORD="devex"

# Use HTTPS if you have Caddy/SSL setup, otherwise HTTP
USE_HTTPS=true  # Set to true if using domain with SSL

# ============================================
# DON'T EDIT BELOW THIS LINE
# ============================================

# Set protocol based on SSL usage
if [ "$USE_HTTPS" = true ]; then
  PROTOCOL="https"
  SERVICE_PORT=""  # No port needed for HTTPS
else
  PROTOCOL="http"
  SERVICE_PORT=":3000"
fi

# Export environment variables
export SERVICE_URL="${PROTOCOL}://${DROPLET_IP}${SERVICE_PORT}"
export MQTT_BROKER_URL="mqtt://${DROPLET_IP}:1883"
export MQTT_USERNAME
export MQTT_PASSWORD

# Color output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

clear
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}🔍 Beeper Service Remote Monitor${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${GREEN}Service URL:${NC} $SERVICE_URL"
echo -e "${GREEN}MQTT Broker:${NC} $MQTT_BROKER_URL"
echo ""

# Quick health check
echo -e "${YELLOW}Checking service health...${NC}"
HEALTH_RESPONSE=$(curl -s "$SERVICE_URL/health" 2>/dev/null)

if [ $? -eq 0 ] && [ -n "$HEALTH_RESPONSE" ]; then
  STATUS=$(echo "$HEALTH_RESPONSE" | jq -r .status 2>/dev/null || echo "unknown")
  if [ "$STATUS" = "ok" ]; then
    echo -e "${GREEN}✓ Service is healthy${NC}"
    
    # Show MQTT status
    MQTT_CONNECTED=$(echo "$HEALTH_RESPONSE" | jq -r .mqtt.connected 2>/dev/null)
    MQTT_QUEUE=$(echo "$HEALTH_RESPONSE" | jq -r .mqtt.queuedMessages 2>/dev/null)
    
    if [ "$MQTT_CONNECTED" = "true" ]; then
      echo -e "${GREEN}✓ MQTT connected${NC} (Queue: $MQTT_QUEUE messages)"
    else
      echo -e "${RED}✗ MQTT disconnected${NC}"
    fi
  else
    echo -e "${RED}✗ Service is unhealthy (status: $STATUS)${NC}"
  fi
else
  echo -e "${RED}✗ Cannot reach service at $SERVICE_URL${NC}"
  echo -e "${YELLOW}Please check:${NC}"
  echo "  1. Is your droplet IP correct? ($DROPLET_IP)"
  echo "  2. Is the service running? (SSH in and run: docker ps)"
  echo "  3. Is port 3000 open? (Check firewall: sudo ufw status)"
  echo ""
  echo "Press Ctrl+C to exit, or wait to continue anyway..."
  sleep 5
fi

echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# Start interactive monitor
echo "Starting interactive monitor..."
echo "Commands: health, test, stats, quit"
echo ""

npm run monitor
