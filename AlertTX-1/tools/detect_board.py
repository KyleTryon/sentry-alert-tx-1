#!/usr/bin/env python3
"""
Board Detection Script for Alert TX-1

This script detects when the Adafruit ESP32-S3 Reverse TFT Feather is connected
to the system and provides information about the detected board.

Usage:
    python3 tools/detect_board.py
    # or called from Makefile: make detect-board

Output:
    - Board detection status
    - Port information
    - Board details
"""

import sys
import json
import subprocess
from pathlib import Path

def run_command(cmd, capture_output=True, text=True):
    """Run a command and return the result"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=capture_output, text=text)
        return result.returncode == 0, result.stdout, result.stderr
    except Exception as e:
        return False, "", str(e)

def detect_esp32_boards():
    """Detect ESP32 boards using arduino-cli"""
    success, output, error = run_command("arduino-cli board list --format json")
    
    if not success:
        print(f"❌ Error running arduino-cli: {error}")
        return []
    
    try:
        data = json.loads(output)
        # Handle different JSON structures
        if isinstance(data, dict) and 'detected_ports' in data:
            return data['detected_ports']
        elif isinstance(data, list):
            return data
        else:
            return []
    except json.JSONDecodeError as e:
        print(f"❌ Error parsing arduino-cli output: {e}")
        return []

def find_esp32s3_feather(boards):
    """Find Adafruit ESP32-S3 Reverse TFT Feather board"""
    target_boards = [
        "Adafruit ESP32-S3 Reverse TFT Feather",
        "Adafruit Feather ESP32-S3 Reverse TFT",
        "ESP32-S3 Reverse TFT Feather"
    ]
    
    for board_data in boards:
        if isinstance(board_data, dict):
            # Check for matching_boards array in the port data
            matching_boards = board_data.get('matching_boards', [])
            if matching_boards:
                for board in matching_boards:
                    if isinstance(board, dict):
                        board_name = board.get('name', '')
                        fqbn = board.get('fqbn', '')
                        
                        # Check for target board names
                        for target in target_boards:
                            if target.lower() in board_name.lower():
                                return board_data
                        
                        # Also check FQBN for ESP32-S3
                        if 'esp32:esp32:adafruit_feather_esp32s3_reversetft' in fqbn:
                            return board_data
            
            # Check for boards array (older format)
            boards_list = board_data.get('boards', [])
            if boards_list:
                for board in boards_list:
                    if isinstance(board, dict):
                        board_name = board.get('name', '')
                        for target in target_boards:
                            if target.lower() in board_name.lower():
                                return board_data
            
            # Check if this is a direct board entry
            board_name = board_data.get('name', '')
            for target in target_boards:
                if target.lower() in board_name.lower():
                    return board_data
    
    return None

def detect_usb_ports():
    """Detect USB serial ports"""
    ports = []
    
    # Check for common USB serial patterns on macOS
    port_patterns = [
        '/dev/tty.usbserial-*',
        '/dev/tty.usbmodem*',
        '/dev/tty.SLAB_USBtoUART*',
        '/dev/tty.wchusbserial*'
    ]
    
    for pattern in port_patterns:
        try:
            from glob import glob
            found_ports = glob(pattern)
            ports.extend(found_ports)
        except Exception:
            pass
    
    return list(set(ports))  # Remove duplicates

def check_board_connection(silent=False):
    """Main function to check board connection"""
    if not silent:
        print("🔍 Detecting ESP32-S3 Feather board...")
    
    # Check if arduino-cli is available
    success, _, _ = run_command("arduino-cli version")
    if not success:
        if not silent:
            print("❌ arduino-cli not found or not in PATH")
            print("   Please install arduino-cli: brew install arduino-cli")
        return False, None
    
    # Detect boards
    boards = detect_esp32_boards()
    if not boards:
        if not silent:
            print("❌ No ports detected")
            print("   Please check USB connection and drivers")
        return False, None
    
    # Find ESP32-S3 Feather
    feather_board = find_esp32s3_feather(boards)
    if not feather_board:
        if not silent:
            print("❌ Adafruit ESP32-S3 Reverse TFT Feather not found")
            print("   Available ports:")
            for port_data in boards:
                if isinstance(port_data, dict):
                    port_info = port_data.get('port', {})
                    port_address = port_info.get('address', 'No address')
                    port_label = port_info.get('label', 'No label')
                    
                    # Check for matching boards
                    matching_boards = port_data.get('matching_boards', [])
                    if matching_boards:
                        for board in matching_boards:
                            if isinstance(board, dict):
                                board_name = board.get('name', 'Unknown')
                                fqbn = board.get('fqbn', '')
                                print(f"     - {board_name} ({port_address}) [{fqbn}]")
                    else:
                        boards_list = port_data.get('boards', [])
                        if boards_list:
                            for board in boards_list:
                                if isinstance(board, dict):
                                    board_name = board.get('name', 'Unknown')
                                    print(f"     - {board_name} ({port_address})")
                        else:
                            print(f"     - {port_label} ({port_address})")
        return False, None
    
    # Board found
    port_info = feather_board.get('port', {})
    port_address = port_info.get('address', 'No port')
    port_label = port_info.get('label', 'No label')
    port_protocol = port_info.get('protocol', 'Unknown')
    
    # Get board name and FQBN
    board_name = "Unknown"
    board_fqbn = ""
    
    matching_boards = feather_board.get('matching_boards', [])
    if matching_boards:
        for board in matching_boards:
            if isinstance(board, dict):
                board_name = board.get('name', 'Unknown')
                board_fqbn = board.get('fqbn', '')
                break
    else:
        boards_list = feather_board.get('boards', [])
        if boards_list:
            for board in boards_list:
                if isinstance(board, dict):
                    board_name = board.get('name', 'Unknown')
                    board_fqbn = board.get('fqbn', '')
                    break
    
    if not silent:
        print(f"✅ Board detected: {board_name}")
        print(f"   Port: {port_address}")
        print(f"   Label: {port_label}")
        print(f"   Protocol: {port_protocol}")
        if board_fqbn:
            print(f"   FQBN: {board_fqbn}")
        
        # Additional port detection
        usb_ports = detect_usb_ports()
        if usb_ports:
            print(f"   USB ports: {', '.join(usb_ports)}")
        
        # Check if board is ready for upload
        if port_address and port_address != 'No port' and not port_address.startswith('/dev/cu.Bluetooth'):
            print("🎯 Board is ready for upload/monitoring")
        else:
            print("⚠️  Board detected but no suitable port assigned")
            print("   Try disconnecting and reconnecting the USB cable")
    
    # Check if board is ready for upload
    if port_address and port_address != 'No port' and not port_address.startswith('/dev/cu.Bluetooth'):
        return True, feather_board
    else:
        return False, feather_board

def main():
    """Main entry point"""
    import sys
    
    # Check if --port-only flag is passed
    port_only = '--port-only' in sys.argv
    
    connected, board_info = check_board_connection(silent=port_only)
    
    if connected:
        if port_only:
            # Return just the port for use in other scripts
            port_info = board_info.get('port', {})
            port_address = port_info.get('address', '')
            if port_address:
                print(port_address)
                return 0
            else:
                return 1
        
        print("\n🚀 Ready for development!")
        print("   Use 'make upload' to upload code")
        print("   Use 'make monitor' to start serial monitor")
        print("   Use 'make dev' for upload + monitor")
        return 0
    else:
        if port_only:
            return 1
        
        print("\n🔧 Troubleshooting tips:")
        print("   1. Check USB cable connection")
        print("   2. Install drivers if needed (Silicon Labs CP210x)")
        print("   3. Try a different USB port")
        print("   4. Check if board appears in System Preferences > Network")
        print("\n⚠️  Board not connected - some commands may fail")
        return 0  # Return 0 for warnings, not errors

if __name__ == "__main__":
    sys.exit(main())
