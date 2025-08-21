#!/bin/bash

# Quick deployment script to fix the webhook header issue

echo "🔧 Fixing Sentry webhook header validation issue..."
echo ""
echo "This script will:"
echo "1. Commit the header fix"
echo "2. Push to your repository" 
echo "3. Rebuild on your Digital Ocean droplet"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Your droplet details
DROPLET_IP="64.225.14.137"
DROPLET_USER="ubuntu"  # or "deployer" - adjust as needed

echo -e "${YELLOW}Step 1: Committing the fix...${NC}"
git add src/types/sentry.ts
git commit -m "fix: make request-id and signature headers optional for Sentry webhooks" || echo "Already committed"

echo -e "${YELLOW}Step 2: Pushing to repository...${NC}"
git push origin main || echo "Push failed - you may need to pull first"

echo -e "${YELLOW}Step 3: SSH commands to run on your droplet:${NC}"
echo ""
echo "SSH into your droplet and run these commands:"
echo -e "${GREEN}ssh $DROPLET_USER@$DROPLET_IP${NC}"
echo ""
echo "# Once connected, run:"
echo "cd sentry_alert_tx-1/Beeper-Service"
echo "git pull origin main"
echo "docker-compose down"
echo "docker-compose up -d --build"
echo ""
echo "# Check logs to confirm it's working:"
echo "docker logs -f beeper-service"
echo ""
echo -e "${YELLOW}Alternative: Run this one-liner (if you have SSH key access):${NC}"
echo ""
echo "ssh $DROPLET_USER@$DROPLET_IP 'cd sentry_alert_tx-1/Beeper-Service && git pull && docker-compose down && docker-compose up -d --build'"
echo ""
echo -e "${GREEN}After deployment, test with:${NC}"
echo "curl https://shoutify.org/health | jq"
