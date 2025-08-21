#!/bin/bash

# Test with actual Sentry event types
SERVICE_URL="${SERVICE_URL:-https://shoutify.org}"

echo "🧪 Testing Sentry event types..."

for EVENT_TYPE in "error.created" "issue.unresolved" "issue.resolved"; do
  echo ""
  echo "Testing event type: $EVENT_TYPE"
  
  RESPONSE=$(curl -s -w "\nHTTP_STATUS:%{http_code}" -X POST "${SERVICE_URL}/webhook" \
    -H "Content-Type: application/json" \
    -H "sentry-hook-resource: ${EVENT_TYPE}" \
    -H "sentry-hook-timestamp: $(date +%s)" \
    -d "{
      \"action\": \"${EVENT_TYPE}\",
      \"installation\": {\"uuid\": \"12345678-1234-1234-1234-123456789012\"},
      \"data\": {
        \"event\": {
          \"title\": \"Test ${EVENT_TYPE}\",
          \"level\": \"error\"
        }
      }
    }")
    
  HTTP_STATUS=$(echo "$RESPONSE" | grep "HTTP_STATUS:" | cut -d: -f2)
  echo "Result: HTTP $HTTP_STATUS"
done
