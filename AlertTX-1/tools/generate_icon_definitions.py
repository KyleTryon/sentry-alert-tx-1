#!/usr/bin/env python3
"""
Generate Icon Definitions Source File
Creates a single .cpp file that defines all icon variables to avoid multiple definition errors.
"""

import os
import glob
from pathlib import Path

def generate_icon_definitions():
    """Generate a single source file with all icon definitions."""
    
    # Find all icon header files
    icon_dir = Path("src/icons")
    header_files = list(icon_dir.glob("*.h"))
    
    if not header_files:
        print("❌ No icon header files found")
        return False
    
    # Generate the source file content
    source_content = """#include <Arduino.h>
#include "Icon.h"

// Include all icon headers
"""
    
    # Add includes for all icon headers (excluding Icon.h which is the structure definition)
    for header_file in sorted(header_files):
        if header_file.name != "Icon.h":  # Skip the Icon structure definition file
            source_content += f'#include "{header_file.name}"\n'
    
    source_content += "\n// Define all icon data arrays and structs\n"
    
    # Add definitions for each icon (excluding Icon.h)
    processed_icons = set()  # Track unique icons to avoid duplicates
    
    for header_file in sorted(header_files):
        if header_file.name != "Icon.h":  # Skip the Icon structure definition file
            # Parse filename to get clean icon name (removing ONLY size suffixes)
            filename = header_file.stem.replace('-', '_')
            
            # Only remove size suffixes that are at the end: _16, _32, _240x135, etc.
            # This preserves icon variant numbers like battery_1, cellular_signal_0
            import re
            
            # Check if this ends with a size suffix pattern
            size_match = re.search(r'_(\d+x?\d*)$', filename)
            if size_match:
                # Extract the size part and remove it
                size_part = size_match.group(1)
                # Only remove if it looks like a size (16, 32, 240x135, etc.)
                if (size_part.isdigit() and int(size_part) in [16, 32, 64, 128, 240]) or 'x' in size_part:
                    clean_icon_name = filename[:size_match.start()]
                else:
                    # Keep the number - it's part of the icon name (like battery_1)
                    clean_icon_name = filename
            else:
                clean_icon_name = filename
            
            # Skip if we've already processed this icon
            if clean_icon_name in processed_icons:
                continue
                
            processed_icons.add(clean_icon_name)
            
            source_content += f"""
// {clean_icon_name} (from {header_file.name})
const uint16_t* {clean_icon_name}_data_ptr = {clean_icon_name}_data;
const Icon {clean_icon_name} = {{ 0, 0, {clean_icon_name.upper()}_WIDTH, {clean_icon_name.upper()}_HEIGHT, {clean_icon_name}_data }};
"""
    
    # Write the source file
    output_file = icon_dir / "icon_definitions.cpp"
    with open(output_file, 'w') as f:
        f.write(source_content)
    
    print(f"✅ Generated {output_file}")
    return True

if __name__ == "__main__":
    generate_icon_definitions()
