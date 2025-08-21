# Build System Documentation

The AlertTX-1 project uses a sophisticated Makefile-based build system that automates compilation, asset generation, and deployment. This guide covers all build commands and customization options.

## 📋 Overview

The build system provides:
- **Automatic board detection** for ESP32-S3 Feather
- **Asset generation** for ringtones and icons
- **Library management** with automatic installation
- **Cross-platform support** (macOS, Linux, Windows via WSL)
- **Intelligent caching** for faster rebuilds

## 🔨 Core Commands

### Essential Commands

```bash
# Show all available commands
make help

# Build the project
make build

# Build and upload to device
make upload

# Upload and open serial monitor (recommended for development)
make dev

# Clean all generated files
make clean
```

### Quick Reference

| Command | Description | Usage |
|---------|-------------|-------|
| `make build` | Compile without uploading | Testing compilation |
| `make upload` | Build and flash to device | Deploying code |
| `make dev` | Upload + serial monitor | Active development |
| `make monitor` | Serial monitor only | Debugging |
| `make clean` | Remove build artifacts | Fresh build |

## 🎵 Asset Generation

### Ringtone System

The build system automatically converts RTTTL files to embedded C++ arrays:

```bash
# Generate ringtone data (with caching)
make ringtones

# Force regenerate (bypass cache)
make clean-ringtones ringtones
```

**Process**:
1. Scans `data/ringtones/*.rtttl.txt`
2. Checks SHA-256 cache for changes
3. Generates three formats:
   - Binary RTTTL (memory efficient)
   - Text RTTTL (for AnyRtttl)
   - BeeperHero track data
4. Creates `src/ringtones/ringtone_data.h`

### Icon System

Convert PNG icons to RGB565 header files:

```bash
# Convert all PNG icons
make icons

# Clean and regenerate
make clean-icons icons
```

**Process**:
1. Scans `data/icons/*.png`
2. Converts to RGB565 format
3. Generates individual headers in `src/icons/`
4. Handles transparency → black conversion

## 📚 Library Management

### Installing Libraries

```bash
# Install all required Arduino libraries
make libraries

# Install Python dependencies only
make python-deps

# Force reinstall all libraries
make clean-libs libraries
```

### Required Libraries
- Adafruit GFX Library (1.11.5+)
- Adafruit ST7789 (1.10.0+)
- ArduinoJson (6.21.3+)
- PubSubClient (2.8+)
- Adafruit NeoPixel (1.11.0+)

## ⚙️ Configuration

### Environment Variables

```bash
# Override Arduino CLI path
ARDUINO_CLI=/custom/path/arduino-cli make build

# Specify port explicitly
PORT=/dev/ttyUSB0 make upload

# Custom board FQBN
BOARD_FQBN=esp32:esp32:custom_board make build

# Parallel jobs for faster builds
MAKEFLAGS="-j8" make build
```

### Makefile Variables

Key variables you can override:

```makefile
# Board configuration
BOARD_FQBN ?= esp32:esp32:adafruit_feather_esp32s3_reversetft
UPLOAD_SPEED ?= 921600

# Tool paths
ARDUINO_CLI ?= arduino-cli
PYTHON ?= python3

# Project structure
SKETCH_DIR ?= .
BUILD_DIR ?= build
DATA_DIR ?= data
SRC_DIR ?= src
TOOLS_DIR ?= tools
```

## 🚀 Advanced Usage

### Development Workflow

```bash
# Full development cycle
make clean          # Start fresh
make python-deps    # Ensure tools installed
make ringtones      # Generate audio data
make icons          # Generate graphics
make dev           # Build, upload, monitor
```

### Continuous Integration

```bash
# CI-friendly commands
make check-deps     # Verify dependencies
make lint          # Run code quality checks
make test          # Run unit tests (if available)
make build         # Compile only
```

### Production Build

```bash
# Optimized production build
make clean
make BUILD_TYPE=release build
make verify        # Verify binary
make upload
```

## 🔧 Build System Features

### Automatic Board Detection

The Makefile automatically detects connected ESP32-S3 Feather boards:

```makefile
# Auto-detection logic
PORT ?= $(shell $(ARDUINO_CLI) board list | grep -E "Feather ESP32-S3|ttyACM|ttyUSB|usbserial|usbmodem" | head -1 | awk '{print $$1}')
```

### Dependency Tracking

The build system tracks dependencies:
- Source file changes trigger rebuilds
- Asset changes regenerate only affected files
- Library updates are detected

### Caching System

#### Ringtone Cache
```bash
# Cache file: .ringtone_cache
{
  "file_hashes": {
    "mario.rtttl.txt": "sha256_hash",
    "digimon.rtttl.txt": "sha256_hash"
  },
  "ringtone_count": 16,
  "timestamp": "2024-12-01T10:00:00"
}
```

#### Benefits
- Builds 10x faster when no changes
- Incremental regeneration
- Portable cache format

## 🐛 Troubleshooting

### Common Issues

#### "Board not found"
```bash
# List connected boards
arduino-cli board list

# Manually specify port
PORT=/dev/ttyACM0 make upload
```

#### "Library not found"
```bash
# Update library index
arduino-cli lib update-index

# Reinstall libraries
make clean-libs libraries
```

#### "Permission denied"
```bash
# Linux: Add user to dialout group
sudo usermod -a -G dialout $USER
# Then logout/login

# macOS: No action needed usually
```

#### Build Errors
```bash
# Verbose build for debugging
VERBOSE=1 make build

# Clean and rebuild
make clean build
```

### Debug Commands

```bash
# Show detected configuration
make info

# Test ringtone generation
make test-ringtones

# Verify setup
make check-deps
```

## 📊 Performance Tips

### Faster Builds
```bash
# Use parallel compilation
make -j8 build

# Skip asset generation if unchanged
make BUILD_ONLY=1 upload

# Use ccache if available
CCACHE=1 make build
```

### Development Optimizations
```bash
# Quick iteration (skip ringtone check)
make SKIP_ASSETS=1 dev

# Upload only (no build)
make upload-only
```

## 🔗 Integration

### VS Code Tasks

`.vscode/tasks.json`:
```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build AlertTX-1",
      "type": "shell",
      "command": "make build",
      "group": {
        "kind": "build",
        "isDefault": true
      }
    },
    {
      "label": "Upload and Monitor",
      "type": "shell", 
      "command": "make dev",
      "problemMatcher": []
    }
  ]
}
```

### Git Hooks

`.git/hooks/pre-commit`:
```bash
#!/bin/bash
# Generate assets before commit
make ringtones icons
git add src/ringtones/ringtone_data.h
git add src/icons/*.h
```

## 📁 Build Artifacts

### Generated Files
```
AlertTX-1/
├── build/                    # Compilation output
│   └── esp32.esp32.adafruit_feather_esp32s3_reversetft/
├── .ringtone_cache          # Ringtone generation cache
├── src/
│   ├── ringtones/
│   │   └── ringtone_data.h  # Generated ringtone data
│   └── icons/
│       └── *.h              # Generated icon headers
```

### Clean Targets
```bash
make clean           # Remove build directory
make clean-ringtones # Remove ringtone data
make clean-icons     # Remove icon headers
make clean-cache     # Remove all caches
make clean-all       # Complete clean
```

## 🚀 Extending the Build System

### Adding Custom Targets

```makefile
# In Makefile.local (gitignored)
.PHONY: my-target
my-target:
	@echo "Running custom target"
	# Your commands here

# Include in main build
build: my-target
```

### Custom Asset Generation

```makefile
# Generate custom assets
CUSTOM_ASSETS := $(wildcard data/custom/*.dat)
CUSTOM_HEADERS := $(patsubst data/custom/%.dat,src/custom/%.h,$(CUSTOM_ASSETS))

src/custom/%.h: data/custom/%.dat
	$(PYTHON) tools/custom_converter.py $< -o $@

custom-assets: $(CUSTOM_HEADERS)
build: custom-assets
```

## 📚 Related Documentation

- [Software Installation](../setup/software-installation.md) - Tool setup
- [Quick Start](../setup/quick-start.md) - Getting started
- [Contributing](../../CONTRIBUTING.md) - Development guidelines

---

**Build System Version**: 2.0  
**Last Updated**: December 2024
