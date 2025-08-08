# Makefile for Alert TX-1
# Automates ringtone data generation and Arduino build process

.PHONY: all clean ringtones build upload monitor help

# Default target
all: ringtones build

# Generate ringtone data from RTTTL files
ringtones:
	@echo "🔧 Generating ringtone data..."
	@python3 tools/generate_ringtone_data.py
	@echo "✅ Ringtone data generation complete"

# Build Arduino project
build: ringtones
	@echo "🔨 Building Arduino project..."
	@arduino-cli compile --fqbn esp32:esp32:esp32s3:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,CoreDebugLevel=0 .
	@echo "✅ Build complete"

# Upload to device
upload: build
	@echo "📤 Uploading to device..."
	@arduino-cli upload --fqbn esp32:esp32:esp32s3:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,CoreDebugLevel=0 .
	@echo "✅ Upload complete"

# Monitor serial output
monitor:
	@echo "📺 Starting serial monitor..."
	@arduino-cli monitor --fqbn esp32:esp32:esp32s3:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,CoreDebugLevel=0

# Clean generated files
clean:
	@echo "🧹 Cleaning generated files..."
	@rm -f src/ringtones/ringtone_data.h
	@echo "✅ Clean complete"

# Show help
help:
	@echo "Alert TX-1 Makefile Commands:"
	@echo ""
	@echo "  make ringtones  - Generate ringtone data from RTTTL files"
	@echo "  make build      - Build Arduino project (includes ringtones)"
	@echo "  make upload     - Upload to device (includes build)"
	@echo "  make monitor    - Start serial monitor"
	@echo "  make clean      - Remove generated files"
	@echo "  make all        - Generate ringtones and build (default)"
	@echo "  make help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make upload     # Generate ringtones, build, and upload"
	@echo "  make monitor    # Start serial monitor for debugging"
	@echo "  make clean      # Clean before adding new ringtones"

# Watch for changes and rebuild
watch:
	@echo "👀 Watching for changes..."
	@while true; do \
		inotifywait -r -e modify,create,delete data/ringtones/; \
		echo "🔄 Changes detected, regenerating ringtones..."; \
		make ringtones; \
	done 