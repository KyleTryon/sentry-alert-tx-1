#!/usr/bin/env python3
"""
PNG to Arduino Header Converter
Converts PNG icons to Arduino-compatible header files with RGB565 format.
"""

import argparse
import os
import sys
from pathlib import Path
from PIL import Image
import re

def rgb888_to_rgb565(r, g, b):
    """Convert RGB888 to RGB565 format."""
    r = (r >> 3) & 0x1F
    g = (g >> 2) & 0x3F
    b = (b >> 3) & 0x1F
    return (r << 11) | (g << 5) | b

def convert_png_to_header(png_path, output_dir, width=None, height=None):
    """Convert a PNG file to an Arduino header file."""
    
    # Load and process the image
    try:
        img = Image.open(png_path)
        
        # Convert to RGBA if not already
        if img.mode != 'RGBA':
            img = img.convert('RGBA')
        
        # Resize if specified
        if width and height:
            img = img.resize((width, height), Image.Resampling.LANCZOS)
        elif width:
            # Maintain aspect ratio
            ratio = width / img.width
            height = int(img.height * ratio)
            img = img.resize((width, height), Image.Resampling.LANCZOS)
        elif height:
            # Maintain aspect ratio
            ratio = height / img.height
            width = int(img.width * ratio)
            img = img.resize((width, height), Image.Resampling.LANCZOS)
        
        img_width, img_height = img.size
        
        # Convert to RGB565
        rgb565_data = []
        for y in range(img_height):
            for x in range(img_width):
                r, g, b, a = img.getpixel((x, y))
                
                # Handle transparency - use black for transparent pixels
                if a < 128:
                    rgb565 = 0x0000  # Black for transparent
                else:
                    rgb565 = rgb888_to_rgb565(r, g, b)
                
                rgb565_data.append(rgb565)
        
        # Generate header filename
        icon_name = png_path.stem
        header_filename = f"{icon_name}.h"
        header_path = output_dir / header_filename
        
        # Convert hyphens to underscores for C identifiers
        c_icon_name = icon_name.replace('-', '_')
        
        # Check if image is 16x16
        if img_width != 16 or img_height != 16:
            print(f"❌ Error: {png_path} is {img_width}x{img_height}, but must be 16x16 pixels")
            return False
        
        # Generate the header content
        header_content = f"""#ifndef {c_icon_name.upper()}_H
#define {c_icon_name.upper()}_H

// AUTO-GENERATED FILE - DO NOT MODIFY
// Generated from: {png_path.name}
// Size: {img_width}x{img_height} pixels

#include "Icon.h"

// Icon dimensions
#define {c_icon_name.upper()}_WIDTH {img_width}
#define {c_icon_name.upper()}_HEIGHT {img_height}

// Icon data in RGB565 format
const uint16_t {c_icon_name}_data[] PROGMEM = {{
"""
        
        # Add the RGB565 data
        for i, pixel in enumerate(rgb565_data):
            if i % 8 == 0:
                header_content += "    "
            header_content += f"0x{pixel:04X}"
            if i < len(rgb565_data) - 1:
                header_content += ", "
            if i % 8 == 7 or i == len(rgb565_data) - 1:
                header_content += "\n"
        
        header_content += f"""}};

// Icon struct instance
extern const Icon {c_icon_name};
const Icon {c_icon_name} = {{ 0, 0, {c_icon_name.upper()}_WIDTH, {c_icon_name.upper()}_HEIGHT, {c_icon_name}_data }};

#endif // {c_icon_name.upper()}_H
"""
        
        # Write the header file
        with open(header_path, 'w') as f:
            f.write(header_content)
        
        print(f"✅ Generated {header_path} ({img_width}x{img_height})")
        return True
        
    except Exception as e:
        print(f"❌ Error converting {png_path}: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Convert PNG icons to Arduino header files')
    parser.add_argument('input', help='Input PNG file or directory')
    parser.add_argument('--output', '-o', help='Output directory for header files')
    parser.add_argument('--width', '-w', type=int, help='Resize width (maintains aspect ratio if height not specified)')
    parser.add_argument('--height', type=int, help='Resize height (maintains aspect ratio if width not specified)')
    parser.add_argument('--recursive', '-r', action='store_true', help='Process subdirectories recursively')
    
    args = parser.parse_args()
    
    input_path = Path(args.input)
    output_dir = Path(args.output) if args.output else Path("src/icons")
    
    # Create output directory if it doesn't exist
    output_dir.mkdir(parents=True, exist_ok=True)
    
    if input_path.is_file():
        # Single file
        if input_path.suffix.lower() == '.png':
            success = convert_png_to_header(input_path, output_dir, args.width, args.height)
            sys.exit(0 if success else 1)
        else:
            print(f"❌ {input_path} is not a PNG file")
            sys.exit(1)
    
    elif input_path.is_dir():
        # Directory - find all PNG files
        pattern = "**/*.png" if args.recursive else "*.png"
        png_files = list(input_path.glob(pattern))
        
        if not png_files:
            print(f"❌ No PNG files found in {input_path}")
            sys.exit(1)
        
        success_count = 0
        for png_file in png_files:
            if convert_png_to_header(png_file, output_dir, args.width, args.height):
                success_count += 1
        
        print(f"\n✅ Converted {success_count}/{len(png_files)} files successfully")
        sys.exit(0 if success_count == len(png_files) else 1)
    
    else:
        print(f"❌ {input_path} does not exist")
        sys.exit(1)

if __name__ == "__main__":
    main()
