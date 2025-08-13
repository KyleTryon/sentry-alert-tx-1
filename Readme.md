# Alert TX-1: Retro 80's Beeper

Alert TX-1 is a retro-inspired beeper built on the [Adafruit ESP32-S3 Reverse TFT Feather](https://www.adafruit.com/product/5691). It receives alerts from Sentry.io (via an MQTT bridge) and displays them with sound and LED feedback.

## Quick start

```bash
cd AlertTX-1
make python-deps   # install Python deps
make libraries     # install Arduino libraries
make upload        # build + upload (auto-detect board)
make dev           # optional: upload + serial monitor
```

Configure Wi‑Fi/MQTT in `AlertTX-1/src/config/settings.h`.

## Docs

See `AlertTX-1/docs/` for guides:
- [README.md](AlertTX-1/docs/README.md) – index and quick links
- [hardware-setup.md](AlertTX-1/docs/hardware-setup.md), [pinout-reference.md](AlertTX-1/docs/pinout-reference.md)
- [UI_FRAMEWORK_OVERVIEW.md](AlertTX-1/docs/UI_FRAMEWORK_OVERVIEW.md)
- [display-troubleshooting.md](AlertTX-1/docs/display-troubleshooting.md)
- [ringtone-build-system.md](AlertTX-1/docs/ringtone-build-system.md), [anyrtttl-integration.md](AlertTX-1/docs/anyrtttl-integration.md)

An external service in `Beeper-Service/` bridges Sentry webhooks to MQTT.

## Contributing

See [CONTRIBUTING.md](AlertTX-1/CONTRIBUTING.md) for coding standards and workflow.
