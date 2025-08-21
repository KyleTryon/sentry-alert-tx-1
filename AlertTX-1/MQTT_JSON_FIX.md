# MQTT JSON Buffer Fix for Sentry Alerts

## Problem
The beeper was crashing when receiving Sentry alerts because:
1. The JSON buffer was only 512 bytes
2. Sentry messages are 1600+ bytes
3. This caused JSON parsing to fail and crash the board

## Solution
Increased buffer sizes from 512 to 2048 bytes to handle larger messages.

## Changes Made
1. `static char buffer[512]` → `static char buffer[2048]`
2. `StaticJsonDocument<512>` → `StaticJsonDocument<2048>`

## Compile and Upload

Using the Makefile [[memory:5999547]]:
```bash
cd AlertTX-1
make upload
```

Or if you need to specify the port:
```bash
make upload UPLOAD_PORT=/dev/ttyUSB0
```

## Testing
After uploading, trigger a Sentry alert. The beeper should:
1. Receive the message without crashing
2. Display the alert notification
3. Play the selected ringtone

## Memory Usage Note
The ESP32 has plenty of RAM (520KB), so increasing these buffers from 512 to 2048 bytes is safe and necessary for Sentry webhook compatibility.
