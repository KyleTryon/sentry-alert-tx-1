#!/bin/bash

# Test script that mimics actual Sentry webhook format
# This sends only the headers that Sentry actually provides

SERVICE_URL="${SERVICE_URL:-https://shoutify.org}"
TIMESTAMP=$(date +%s)

echo "🧪 Testing Sentry-compatible webhook to ${SERVICE_URL}/webhook..."
echo ""
echo "Sending webhook with minimal headers (like Sentry does)..."

# Send webhook with only required headers that Sentry actually sends
RESPONSE=$(curl -s -w "\n\nHTTP_STATUS:%{http_code}" -X POST "${SERVICE_URL}/webhook" \
  -H "Content-Type: application/json" \
  -H "sentry-hook-resource: error" \
  -H "sentry-hook-timestamp: ${TIMESTAMP}" \
  -d '{
    "action": "error.created",
    "installation": {
      "uuid": "12345678-1234-1234-1234-123456789012"
    },
    "data": {
      "error": {
        "id": "1234567890",
        "title": "TypeError: Cannot read property of undefined",
        "culprit": "app.js in handleSubmit",
        "level": "error",
        "platform": "javascript",
        "tags": [
          ["environment", "production"],
          ["release", "1.0.0"]
        ]
      }
    }
  }')

# Extract HTTP status
HTTP_STATUS=$(echo "$RESPONSE" | grep "HTTP_STATUS:" | cut -d: -f2)
BODY=$(echo "$RESPONSE" | sed '/HTTP_STATUS:/d')

if [ "$HTTP_STATUS" = "200" ]; then
  echo "✅ Success! Webhook accepted (HTTP $HTTP_STATUS)"
  echo "Response: $BODY"
  echo ""
  echo "🎉 The fix worked! Sentry webhooks should now be processed correctly."
else
  echo "❌ Failed! HTTP Status: $HTTP_STATUS"
  echo "Response: $BODY"
  echo ""
  echo "The service may not be updated yet. Please deploy the fix first."
fi

echo ""
echo "📊 To verify MQTT delivery, run:"
echo "npm run mqtt:debug"
