#!/usr/bin/env python3
"""
Library Installation Script for Alert TX-1
Automatically installs all required Arduino libraries.
"""

import subprocess
import sys
import os
from pathlib import Path

def run_command(command):
    """Run a command and return success status."""
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            print(f"✅ {command}")
            return True
        else:
            print(f"❌ {command}")
            print(f"   Error: {result.stderr}")
            return False
    except Exception as e:
        print(f"❌ {command}")
        print(f"   Exception: {e}")
        return False

def install_libraries():
    """Install all required libraries."""
    libraries = [
        "GFX Library for Arduino",
        "AnyRtttl",
        "PubSubClient"
    ]
    
    print("📚 Installing Alert TX-1 Arduino Libraries")
    print("=" * 50)
    
    success_count = 0
    for library in libraries:
        print(f"\n📦 Installing {library}...")
        if run_command(f'arduino-cli lib install "{library}"'):
            success_count += 1
    
    print(f"\n📊 Installation Summary:")
    print(f"   ✅ Successfully installed: {success_count}/{len(libraries)}")
    print(f"   ❌ Failed: {len(libraries) - success_count}")
    
    if success_count == len(libraries):
        print("\n🎉 All libraries installed successfully!")
        return True
    else:
        print("\n⚠️  Some libraries failed to install. Check the errors above.")
        return False

def check_libraries():
    """Check which libraries are already installed."""
    print("🔍 Checking installed libraries...")
    result = subprocess.run("arduino-cli lib list", shell=True, capture_output=True, text=True)
    
    if result.returncode == 0:
        installed_libraries = result.stdout.lower()
        required_libraries = [
            "gfx library for arduino",
            "anyrtttl",
            "pubsubclient"
        ]
        
        print("\n📋 Library Status:")
        for library in required_libraries:
            if library in installed_libraries:
                print(f"   ✅ {library}")
            else:
                print(f"   ❌ {library} (not installed)")
    else:
        print("❌ Failed to check libraries")

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "--check":
        check_libraries()
    else:
        install_libraries()

if __name__ == "__main__":
    main()
