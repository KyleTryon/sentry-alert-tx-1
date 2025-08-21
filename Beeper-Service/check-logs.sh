#!/bin/bash

echo "📋 Commands to check server logs for webhook errors:"
echo ""
echo "ssh ubuntu@64.225.14.137"
echo ""
echo "# Check recent webhook attempts (last 20 minutes):"
echo 'docker logs beeper-service --since 20m 2>&1 | grep -A5 -B5 "webhook\|400\|POST /webhook"'
echo ""
echo "# Check for specific error messages:"
echo 'docker logs beeper-service --since 20m 2>&1 | grep -A10 "Invalid\|Error\|error"'
echo ""
echo "# Follow live logs to catch the next webhook:"
echo "docker logs -f beeper-service --tail 100"
echo ""
echo "# Check if the service restarted recently:"
echo "docker ps --format 'table {{.Names}}\t{{.Status}}' | grep beeper"
echo ""
echo "# View all HTTP requests:"
echo 'docker logs beeper-service --since 20m 2>&1 | grep "HTTP Request"'
