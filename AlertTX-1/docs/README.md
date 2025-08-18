# Alert TX-1 Docs

Concise, task-focused guides for users and contributors.

## Quick start

1) Hardware setup: see [hardware-setup.md](hardware-setup.md) (board, wiring, pins)
2) Install tools:
```bash
make python-deps
make libraries
```
3) Build/upload and monitor:
```bash
make upload   # build + upload (auto-detect board)
make dev      # upload + serial monitor
```

## Read this first
- UI framework overview: [UI_FRAMEWORK_OVERVIEW.md](UI_FRAMEWORK_OVERVIEW.md)
- Display issues? [display-troubleshooting.md](display-troubleshooting.md)
- Power/battery: [power-management.md](power-management.md)
 - Input routing: centralized input behavior (long-press back, select-on-release, auto-repeat) described in the UI Framework Overview

## Guides by area
- Hardware: [hardware-setup.md](hardware-setup.md), [pinout-reference.md](pinout-reference.md)
- UI framework: [UI_FRAMEWORK_OVERVIEW.md](UI_FRAMEWORK_OVERVIEW.md)
- Audio: [anyrtttl-integration.md](anyrtttl-integration.md), [ringtone-build-system.md](ringtone-build-system.md)
- Icons: [ICON_SYSTEM_SUMMARY.md](ICON_SYSTEM_SUMMARY.md)
 - Games: Pong demo lives in `src/ui/games/PongScreen.*` and demonstrates efficient partial redraws, score header, and screen ownership (ephemeral app)

## Controls
- Button A (GPIO0): Up/Previous
- Button B (GPIO1): Down/Next
- Button C (GPIO2): Select/Enter
- Long press any button (1500 ms): Back

## Contributing
See [../CONTRIBUTING.md](../CONTRIBUTING.md) for setup, coding standards, and PR process.

## History and archived docs
These are kept for historical reference and are no longer the main guides:
- `UI_FRAMEWORK_EXPANSION_PLAN.md` (archived)
- `PHASE1_IMPLEMENTATION_COMPLETE.md` (archived)
- `PHASE2_IMPLEMENTATION_COMPLETE.md` (archived)
- `NAVIGATION_SYSTEM_COMPLETE.md` (archived)
- `UI_FRAMEWORK_GUIDE.md` (legacy, archived)
