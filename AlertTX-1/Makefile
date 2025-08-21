# Makefile for Alert TX-1
# Automates ringtone data generation, icon conversion, and Arduino build process

.PHONY: all clean ringtones icons build upload monitor help dev detect-board libraries python-deps gen-secrets

# Install required Python dependencies
python-deps:
	@echo "🐍 Installing required Python dependencies..."
	@pip3 install -r tools/requirements.txt --break-system-packages
	@echo "✅ Python dependencies installation complete"

# Install required Arduino libraries
libraries: python-deps
	@echo "📚 Installing required Arduino libraries..."
	@python3 tools/install_libraries.py
	@echo "✅ Library installation complete"

# Default target
all: libraries ringtones icons build

# Detect board connection and get port
detect-board:
	@echo "🔍 Checking board connection..."
	@python3 tools/detect_board.py

# Generate ringtone data from RTTTL files (with caching)
ringtones:
	@echo "🔧 Checking ringtone data..."
	@python3 tools/generate_ringtone_data.py
	@echo "✅ Ringtone data generation complete"

# Generate icon headers from PNG files
icons:
	@echo "🎨 Converting PNG icons to header files..."
	@mkdir -p src/icons
	@python3 tools/png_to_header.py data/icons --output src/icons
	@echo "✅ Icon conversion complete"

# Generate a build-time secrets header from environment or .env file (ignored by git)
gen-secrets:
	@echo "🔐 Generating build-time secrets header (generated_secrets.h)"
	@out="src/config/generated_secrets.h"; \
	if [ ! -d src/config ]; then mkdir -p src/config; fi; \
	if [ -f .env ]; then set -a; . ./.env; set +a; fi; \
	{ \
	  echo "// Auto-generated at build time. Do NOT commit."; \
	  echo "#pragma once"; \
	  printf "#define ALERTTX1_ENV_WIFI_SSID            \"%s\"\n" "$${WIFI_SSID:-}"; \
	  printf "#define ALERTTX1_ENV_WIFI_PASSWORD        \"%s\"\n" "$${WIFI_PASSWORD:-}"; \
	  printf "#define ALERTTX1_ENV_MQTT_BROKER          \"%s\"\n" "$${MQTT_BROKER:-localhost}"; \
	  printf "#define ALERTTX1_ENV_MQTT_PORT            %s\n" "$${MQTT_PORT:-1883}"; \
	  printf "#define ALERTTX1_ENV_MQTT_USERNAME        \"%s\"\n" "$${MQTT_USERNAME:-}"; \
	  printf "#define ALERTTX1_ENV_MQTT_PASSWORD        \"%s\"\n" "$${MQTT_PASSWORD:-}"; \
	  printf "#define ALERTTX1_ENV_MQTT_CLIENT_ID       \"%s\"\n" "$${MQTT_CLIENT_ID:-AlertTX1}"; \
	  printf "#define ALERTTX1_ENV_MQTT_SUBSCRIBE_TOPIC \"%s\"\n" "$${MQTT_TOPIC_SUBSCRIBE:-alerttx1/alerts}"; \
	  printf "#define ALERTTX1_ENV_MQTT_PUBLISH_TOPIC   \"%s\"\n" "$${MQTT_TOPIC_PUBLISH:-alerttx1/status}"; \
	} > "$$out" && echo "✅ Secrets header written" || { echo "❌ Failed to write secrets header"; exit 1; }

# Build Arduino project (always includes libraries, ringtones and icons)
build: libraries ringtones icons gen-secrets
	@echo "🔨 Building Arduino project..."
	@arduino-cli compile --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
	@echo "✅ Build complete"

# Upload to device (checks board connection first)
upload: detect-board build
	@echo "📤 Uploading to device..."
	@PORT=$$(python3 tools/detect_board.py --port-only 2>/dev/null) && \
	if [ -n "$$PORT" ]; then \
		arduino-cli upload --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft --port "$$PORT" .; \
	else \
		echo "❌ No valid port found for upload"; \
		exit 1; \
	fi
	@echo "✅ Upload complete"

# Monitor serial output (checks board connection first)
monitor: detect-board
	@echo "📺 Starting serial monitor..."
	@PORT=$$(python3 tools/detect_board.py --port-only 2>/dev/null) && \
	if [ -n "$$PORT" ]; then \
		arduino-cli monitor --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft --port "$$PORT" --config baudrate=115200; \
	else \
		echo "❌ No valid port found for monitoring"; \
		exit 1; \
	fi

# Dev mode: upload and automatically start monitor
dev: upload
	@echo "🚀 Starting dev mode (upload + monitor)..."
	@echo "Press Ctrl+C to stop monitoring"
	@PORT=$$(python3 tools/detect_board.py --port-only 2>/dev/null) && \
	if [ -n "$$PORT" ]; then \
		arduino-cli monitor --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft --port "$$PORT" --config baudrate=115200; \
	else \
		echo "❌ No valid port found for monitoring"; \
		exit 1; \
	fi

# Clean generated files
clean:
	@echo "🧹 Cleaning generated files..."
	@rm -f src/ringtones/ringtone_data.h
	@rm -f .ringtone_cache
	@find src/icons -type f -name '*.h' ! -name 'Icon.h' -delete
	@echo "✅ Clean complete"

# Show help
help:
	@echo "Alert TX-1 Makefile Commands:"
	@echo ""
	@echo "  make detect-board - Check if ESP32-S3 Feather is connected"
	@echo "  make python-deps - Install required Python dependencies"
	@echo "  make libraries   - Install required Arduino libraries"
	@echo "  make ringtones   - Generate ringtone data from RTTTL files (with caching)"
	@echo "  make icons       - Convert PNG icons to Arduino header files"
	@echo "  make build       - Build Arduino project (includes libraries, ringtones and icons)"
	@echo "  make upload      - Upload to device (includes board detection)"
	@echo "  make monitor     - Start serial monitor (includes board detection)"
	@echo "  make dev         - Upload and automatically start monitor (dev mode)"
	@echo "  make clean       - Remove generated files and cache"
	@echo "  make all         - Install Python deps, libraries, generate ringtones, icons, and build (default)"
	@echo "  make help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make detect-board # Check if board is connected and ready"
	@echo "  make dev         # Upload and start monitoring (recommended for development)"
	@echo "  make upload      # Generate ringtones, icons, build, and upload"
	@echo "  make icons       # Convert PNG icons to header files"
	@echo "  make monitor     # Start serial monitor for debugging"
	@echo "  make clean       # Clean before adding new ringtones or icons"

# Watch for changes and rebuild
watch:
	@echo "👀 Watching for changes..."
	@while true; do \
		inotifywait -r -e modify,create,delete data/ringtones/; \
		echo "🔄 Changes detected, regenerating ringtones..."; \
		make ringtones; \
	done 