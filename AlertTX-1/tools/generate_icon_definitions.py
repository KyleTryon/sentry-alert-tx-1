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
    for header_file in sorted(header_files):
        if header_file.name != "Icon.h":  # Skip the Icon structure definition file
            icon_name = header_file.stem.replace('-', '_')
            source_content += f"""
// {icon_name}
const uint16_t* {icon_name}_data_ptr = {icon_name}_data;
const Icon {icon_name} = {{ 0, 0, {icon_name.upper()}_WIDTH, {icon_name.upper()}_HEIGHT, {icon_name}_data }};
"""
    
    # Write the source file
    output_file = icon_dir / "icon_definitions.cpp"
    with open(output_file, 'w') as f:
        f.write(source_content)
    
    print(f"✅ Generated {output_file}")
    return True

if __name__ == "__main__":
    generate_icon_definitions()
