#!/bin/bash

# Simple script to send a test webhook to the Beeper Service
# Works with both local and remote deployments

SERVICE_URL="${SERVICE_URL:-http://localhost:3000}"
TIMESTAMP=$(date +%s)

# Color output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Detect if remote
if [[ ! "$SERVICE_URL" =~ "localhost" ]]; then
  echo -e "${YELLOW}Remote deployment detected: ${SERVICE_URL}${NC}"
fi

echo "Sending test webhook to ${SERVICE_URL}/webhook..."

curl -X POST "${SERVICE_URL}/webhook" \
  -H "Content-Type: application/json" \
  -H "sentry-hook-resource: event_alert" \
  -H "sentry-hook-timestamp: ${TIMESTAMP}" \
  -H "request-id: test_${TIMESTAMP}" \
  -d '{
    "action": "event.alert",
    "installation": {
      "uuid": "test-installation-123"
    },
    "data": {
      "event": {
        "title": "Test Alert from test-webhook.sh",
        "message": "This is a test alert to verify the Sentry-to-Beeper bridge is working",
        "level": "warning",
        "environment": "test",
        "url": "https://sentry.io/test",
        "web_url": "https://sentry.io/organizations/test/issues/123456/",
        "tags": [
          ["source", "test-script"],
          ["type", "manual-test"]
        ]
      },
      "triggered_rule": "Test Rule"
    }
  }' \
  -w "\n\nHTTP Status: %{http_code}\nTime Total: %{time_total}s\n"

echo -e "\n\nCheck MQTT messages with: node mqtt-debug.js"
