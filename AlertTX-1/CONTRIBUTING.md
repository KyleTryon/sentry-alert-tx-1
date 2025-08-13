# Contributing to Alert TX-1

Thanks for helping improve Alert TX-1! This guide keeps contributions fast and consistent.

## Prerequisites
- Python 3.9+
- `arduino-cli`
- ESP32-S3 Reverse TFT Feather (Adafruit) connected via USB

## Setup
```bash
make python-deps   # install Python deps
make libraries     # install Arduino libraries
```

## Build, upload, and monitor
```bash
make upload   # build + upload to device (auto-detects port)
make dev      # upload and start serial monitor
make monitor  # only serial monitor
```

Tips:
- If icons or ringtones change, `make icons` or `make ringtones` (both are included in build/upload).
- Use `make help` to list all commands.

## Coding standards (C++/Arduino)
- Follow existing architecture: `Component` → `Screen` → `ScreenManager`.
- Use `ThemeManager` for colors; avoid hardcoded RGB values.
- Use `DisplayConfig` for dimensions; no magic numbers.
- Prefer small, focused classes and fixed-size arrays where possible.
- Avoid deep nesting; use guard clauses and early returns.
- Document public headers with brief docstrings.

## Branches and PRs
- Create feature branches from `main` (e.g., `feat/ui-alerts-screen`, `fix/button-debounce`).
- Keep edits focused and small; include screenshots or short video/gif if UI changes.
- Update docs when behavior or APIs change.
- Use clear commit messages: `area: short description` (e.g., `ui: add SettingsScreen theme cycle`).

## Testing checklist
- Build succeeds: `make upload`.
- UI navigation works: A=Up, B=Down, C=Select, long press=Back.
- No flicker on splash; 30+ FPS feel on redraws.
- Memory within limits; no unexpected heap growth.

## Docs
- Keep docs concise and task-focused.
- Put user-facing notes in `docs/` and link from `docs/README.md`.
- Historical plans/status live as archived docs.

Thanks again! 🚀