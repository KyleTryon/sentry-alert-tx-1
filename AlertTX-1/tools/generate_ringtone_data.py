#!/usr/bin/env python3
"""
RTTTL Data Generator for Alert TX-1

This script scans the data/ringtones/ directory and generates a C++ header file
containing all RTTTL ringtone data in multiple formats:
- Binary RTTTL (compact)
- Text RTTTL (for AnyRtttl nonblocking playback)
- BeeperHero Track data (optimized binary for rhythm gameplay)

Features:
- Automatic caching: Only regenerates when RTTTL files change
- Multiple formats: Flexible at runtime (audio + gameplay)
- Hash-based detection: Fast change detection

Usage:
    python3 tools/generate_ringtone_data.py

Output:
    src/ringtones/ringtone_data.h - Generated header file with embedded RTTTL and track data
    .ringtone_cache - Cache file for change detection
"""

import os
import re
import sys
import hashlib
import json
from pathlib import Path
from datetime import datetime

def sanitize_name(filename):
    """Convert filename to valid C++ identifier"""
    # Remove extension and replace non-alphanumeric chars with underscore
    name = re.sub(r'[^a-zA-Z0-9_]', '_', filename.replace('.txt', '').replace('.rtttl.txt', ''))
    # Ensure it starts with a letter
    if name and name[0].isdigit():
        name = 'ringtone_' + name
    return name

def extract_rtttl_name(content):
    """Extract the RTTTL name from the content (before the first colon)"""
    if ':' in content:
        return content.split(':')[0].strip()
    return "Unknown"

def calculate_file_hash(file_path):
    """Calculate SHA-256 hash of a file"""
    hash_sha256 = hashlib.sha256()
    with open(file_path, 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_sha256.update(chunk)
    return hash_sha256.hexdigest()

def load_cache(cache_file):
    """Load cache from file"""
    if cache_file.exists():
        try:
            with open(cache_file, 'r') as f:
                return json.load(f)
        except (json.JSONDecodeError, FileNotFoundError):
            pass
    return {}

def save_cache(cache_file, cache_data):
    """Save cache to file"""
    cache_data['timestamp'] = datetime.now().isoformat()
    cache_data['version'] = '1.0'
    with open(cache_file, 'w') as f:
        json.dump(cache_data, f, indent=2)

def check_cache_validity(ringtone_dir, cache_data):
    """
    Check if cache is still valid by comparing file hashes
    Returns (is_valid, changed_files, current_hashes)
    """
    current_hashes = {}
    changed_files = []
    
    # Get all RTTTL files
    ringtone_files = list(ringtone_dir.glob("*.rtttl.txt"))
    
    if not ringtone_files:
        print("  No RTTTL files found in directory")
        return False, [], {}
    
    # Check all RTTTL files
    for file_path in ringtone_files:
        file_hash = calculate_file_hash(file_path)
        current_hashes[file_path.name] = file_hash
        
        # Check if file has changed or is new
        cached_hash = cache_data.get('file_hashes', {}).get(file_path.name)
        if cached_hash != file_hash:
            changed_files.append(file_path.name)
            if cached_hash is None:
                print(f"  New file detected: {file_path.name}")
            else:
                print(f"  File changed: {file_path.name}")
    
    # Check for deleted files
    cached_files = set(cache_data.get('file_hashes', {}).keys())
    current_files = set(current_hashes.keys())
    deleted_files = cached_files - current_files
    
    if deleted_files:
        changed_files.extend(deleted_files)
        print(f"  Detected deleted files: {', '.join(deleted_files)}")
    
    # Cache is valid if no changes detected and we have files
    is_valid = len(changed_files) == 0 and len(current_hashes) > 0
    
    return is_valid, changed_files, current_hashes

def get_relative_path(file_path, base_path):
    """Convert absolute path to relative path from base"""
    try:
        return str(file_path.relative_to(base_path))
    except ValueError:
        return str(file_path)

def parse_rtttl_to_binary(rtttl_text):
    """
    Parse RTTTL text and convert to binary format (10-bit)
    Based on AnyRtttl library binary format specification
    """
    if not rtttl_text or ':' not in rtttl_text:
        return None
    
    # Split into sections
    parts = rtttl_text.split(':', 2)
    if len(parts) < 3:
        return None
    
    name = parts[0]
    defaults = parts[1]
    notes = parts[2]
    
    # Parse defaults
    default_duration = 4
    default_octave = 5
    default_bpm = 160
    
    for default in defaults.split(','):
        if 'd=' in default:
            default_duration = int(default.split('=')[1])
        elif 'o=' in default:
            default_octave = int(default.split('=')[1])
        elif 'b=' in default:
            default_bpm = int(default.split('=')[1])
    
    # Convert to binary format
    binary_data = []
    
    # Header: song name (11 bytes) + default values (2 bytes)
    # Song name (padded to 11 bytes)
    name_bytes = name.encode('ascii')[:11].ljust(11, b'\0')
    binary_data.extend(name_bytes)
    
    # Default values (duration, octave, BPM)
    # Duration index: 1->0, 2->1, 4->2, 8->3, 16->4, 32->5
    duration_map = {1: 0, 2: 1, 4: 2, 8: 3, 16: 4, 32: 5}
    duration_idx = duration_map.get(default_duration, 2)
    
    # Octave index: 4->0, 5->1, 6->2, 7->3
    octave_idx = max(0, min(3, default_octave - 4))
    
    # BPM index: map to predefined BPM values
    bpm_values = [25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90, 100, 112, 125, 140, 160, 180, 200, 225, 250, 285, 320, 355, 400, 450, 500, 565, 635, 715, 800, 900]
    bpm_idx = 16  # Default to 160 BPM
    for i, bpm in enumerate(bpm_values):
        if bpm >= default_bpm:
            bpm_idx = i
            break
    
    # Pack default values into 2 bytes
    default_byte1 = (duration_idx & 0x07) | ((octave_idx & 0x03) << 3)
    default_byte2 = bpm_idx & 0x1F
    binary_data.extend([default_byte1, default_byte2])
    
    # Parse notes
    note_letters = {'c': 0, 'd': 1, 'e': 2, 'f': 3, 'g': 4, 'a': 5, 'b': 6, 'p': 7}
    
    for note in notes.split(','):
        note = note.strip()
        if not note:
            continue
        
        # Parse note components
        duration = default_duration
        note_letter = 'p'
        octave = default_octave
        is_sharp = False
        is_dotted = False
        
        # Extract duration
        duration_match = re.match(r'(\d+)', note)
        if duration_match:
            duration = int(duration_match.group(1))
            note = note[len(duration_match.group(1)):]
        
        # Extract note letter
        if note and note[0].lower() in note_letters:
            note_letter = note[0].lower()
            note = note[1:]
        
        # Extract sharp
        if note.startswith('#'):
            is_sharp = True
            note = note[1:]
        
        # Extract octave
        octave_match = re.match(r'(\d+)', note)
        if octave_match:
            octave = int(octave_match.group(1))
            note = note[len(octave_match.group(1)):]
        
        # Check for dotted
        if note.startswith('.'):
            is_dotted = True
        
        # Pack note into 2 bytes (10-bit format)
        duration_idx = duration_map.get(duration, 2)
        note_idx = note_letters.get(note_letter, 7)
        octave_idx = max(0, min(3, octave - 4))
        
        note_byte1 = (duration_idx & 0x07) | ((note_idx & 0x07) << 3) | (is_sharp << 6) | (is_dotted << 7)
        note_byte2 = (octave_idx & 0x03)
        
        binary_data.extend([note_byte1, note_byte2])
    
    return binary_data

def parse_rtttl_to_track(rtttl_text):
    """
    Build BeeperHero track data (BPHR format) from an RTTTL string.
    Returns a list of ints (bytes) or None on failure.
    """
    if not rtttl_text or ':' not in rtttl_text:
        return None

    parts = rtttl_text.split(':', 2)
    if len(parts) < 3:
        return None

    song_name = parts[0].strip()
    defaults = parts[1]
    notes_text = parts[2]

    # Defaults
    default_duration = 4
    default_octave = 5
    default_bpm = 160
    for d in defaults.split(','):
        d = d.strip()
        if d.startswith('d='):
            try:
                default_duration = int(d.split('=')[1])
            except Exception:
                pass
        elif d.startswith('o='):
            try:
                default_octave = int(d.split('=')[1])
            except Exception:
                pass
        elif d.startswith('b='):
            try:
                default_bpm = int(d.split('=')[1])
            except Exception:
                pass

    whole_note_ms = int(60000 * 4 / max(1, default_bpm))

    def lane_from_octave(octave: int) -> int:
        if octave <= 4:
            return 0
        elif octave == 5:
            return 1
        else:
            return 2

    parsed_notes = []  # (start_ms, duration_ms, lane, flags)
    current_time = 0
    for raw in notes_text.split(','):
        note = raw.strip()
        if not note:
            continue

        duration = default_duration
        octave = default_octave
        letter = 'p'
        is_dotted = False

        m = re.match(r'^(\d+)', note)
        if m:
            duration = int(m.group(1))
            note = note[m.end():]

        if note and note[0].lower() in 'cdefgabp':
            letter = note[0].lower()
            note = note[1:]

        if note.startswith('#'):
            # we ignore sharp for lane mapping
            note = note[1:]

        m = re.match(r'^(\d+)', note)
        if m:
            octave = int(m.group(1))
            note = note[m.end():]

        if note.startswith('.'):
            is_dotted = True

        base_div = max(1, duration)
        dur_ms = int(whole_note_ms / base_div)
        if is_dotted:
            dur_ms = int(dur_ms * 1.5)

        if letter == 'p':
            current_time += dur_ms
            continue

        lane = lane_from_octave(octave)
        flags = 0
        parsed_notes.append((current_time, dur_ms, lane, flags))
        current_time += dur_ms

    # Build header (BPHR)
    magic = b'BPHR'
    version = 1
    song_name_bytes = song_name.encode('ascii', errors='ignore')
    if len(song_name_bytes) > 63:
        song_name_bytes = song_name_bytes[:63]
    song_name_length = len(song_name_bytes)
    note_count = len(parsed_notes)
    song_duration = current_time
    bpm = max(1, min(1000, default_bpm))
    reserved = 0

    blob = []
    blob.extend(list(magic))
    blob.append(version & 0xFF)
    blob.append(song_name_length & 0xFF)
    blob.extend([note_count & 0xFF, (note_count >> 8) & 0xFF])
    blob.extend([
        song_duration & 0xFF,
        (song_duration >> 8) & 0xFF,
        (song_duration >> 16) & 0xFF,
        (song_duration >> 24) & 0xFF,
    ])
    blob.extend([bpm & 0xFF, (bpm >> 8) & 0xFF])
    blob.extend([reserved & 0xFF, (reserved >> 8) & 0xFF])

    # name + null terminator
    blob.extend(list(song_name_bytes))
    blob.append(0)

    # notes
    for start_ms, dur_ms, lane, flags in parsed_notes:
        blob.extend([
            start_ms & 0xFF,
            (start_ms >> 8) & 0xFF,
            (start_ms >> 16) & 0xFF,
            (start_ms >> 24) & 0xFF,
            dur_ms & 0xFF,
            (dur_ms >> 8) & 0xFF,
            lane & 0xFF,
            flags & 0xFF,
        ])

    return blob

def generate_header_file(ringtone_files):
    """Generate the C++ header file with embedded RTTTL data"""
    
    header_content = f"""// Auto-generated by tools/generate_ringtone_data.py
// Do not edit this file manually - it will be overwritten!

#ifndef RINGTONE_DATA_H
#define RINGTONE_DATA_H

#include <Arduino.h>

// RTTTL ringtone data - embedded at compile time (multiple formats)
// Generated from {len(ringtone_files)} files in data/ringtones/
// Includes: Binary RTTTL, Text RTTTL, and BeeperHero Track data

"""
    
    # Add ringtone data arrays (all formats)
    ringtone_names = []
    for filename, content, rtttl_name in ringtone_files:
        cpp_name = sanitize_name(filename)
        ringtone_names.append((cpp_name, rtttl_name))
        
        # Generate binary data
        binary_data = parse_rtttl_to_binary(content)
        # Text RTTTL
        safe_text = content.replace('"', '\\"')
        # BeeperHero Track
        track_data = parse_rtttl_to_track(content)
        
        header_content += f"""// {rtttl_name} - from {filename}
"""
        
        if binary_data:
            # Convert binary data to C++ array
            binary_array = ', '.join([f'0x{b:02X}' for b in binary_data])
            header_content += f"""const unsigned char {cpp_name}_binary[] PROGMEM = {{{binary_array}}};
const size_t {cpp_name}_binary_size = {len(binary_data)};
"""
        else:
            header_content += f"""// Binary conversion failed for {rtttl_name}
const unsigned char* {cpp_name}_binary = nullptr;
const size_t {cpp_name}_binary_size = 0;
"""
        
        # Text RTTTL string
        header_content += f"""const char {cpp_name}_text[] PROGMEM = "{safe_text}";
"""

        # Track data
        if track_data:
            track_array = ', '.join([f'0x{b:02X}' for b in track_data])
            header_content += f"""const uint8_t {cpp_name}_track[] PROGMEM = {{{track_array}}};
const size_t {cpp_name}_track_size = {len(track_data)};
"""
        else:
            header_content += f"""const uint8_t* {cpp_name}_track = nullptr;
const size_t {cpp_name}_track_size = 0;
"""

        header_content += "\n"
    
    # Add ringtone registry
    header_content += f"""
// Ringtone registry - maps names to all formats
struct RingtoneEntry {{
    const char* name;
    const unsigned char* binary_data;
    size_t binary_size;
    const char* text_data;
    const uint8_t* track_data;
    size_t track_size;
    const char* filename;
}};

static const RingtoneEntry RINGTONE_REGISTRY[] = {{
"""
    
    for cpp_name, rtttl_name in ringtone_names:
        header_content += f'    {{"{rtttl_name}", {cpp_name}_binary, {cpp_name}_binary_size, {cpp_name}_text, {cpp_name}_track, {cpp_name}_track_size, "{cpp_name}"}},\n'
    
    header_content += f"""    {{nullptr, nullptr, 0, nullptr}}  // End marker
}};

// Total number of ringtones
static const int RINGTONE_COUNT = {len(ringtone_files)};

// Helper functions - Binary RTTTL
inline const unsigned char* getBinaryRTTTL(const char* name) {{
    for (int i = 0; i < RINGTONE_COUNT; i++) {{
        if (strcmp(RINGTONE_REGISTRY[i].name, name) == 0) {{
            return RINGTONE_REGISTRY[i].binary_data;
        }}
    }}
    return nullptr;
}}

inline const unsigned char* getBinaryRTTTL(int index) {{
    if (index >= 0 && index < RINGTONE_COUNT) {{
        return RINGTONE_REGISTRY[index].binary_data;
    }}
    return nullptr;
}}

inline size_t getBinaryRTTTLSize(const char* name) {{
    for (int i = 0; i < RINGTONE_COUNT; i++) {{
        if (strcmp(RINGTONE_REGISTRY[i].name, name) == 0) {{
            return RINGTONE_REGISTRY[i].binary_size;
        }}
    }}
    return 0;
}}

inline size_t getBinaryRTTTLSize(int index) {{
    if (index >= 0 && index < RINGTONE_COUNT) {{
        return RINGTONE_REGISTRY[index].binary_size;
    }}
    return 0;
}}

inline const char* getRingtoneName(int index) {{
    if (index >= 0 && index < RINGTONE_COUNT) {{
        return RINGTONE_REGISTRY[index].name;
    }}
    return nullptr;
}}

inline int findRingtoneIndex(const char* name) {{
    for (int i = 0; i < RINGTONE_COUNT; i++) {{
        if (strcmp(RINGTONE_REGISTRY[i].name, name) == 0) {{
            return i;
        }}
    }}
    return -1;
}}

// Text RTTTL helpers
inline const char* getTextRTTTL(int index) {{
    if (index >= 0 && index < RINGTONE_COUNT) {{
        return RINGTONE_REGISTRY[index].text_data;
    }}
    return nullptr;
}}

inline const char* getTextRTTTL(const char* name) {{
    for (int i = 0; i < RINGTONE_COUNT; i++) {{
        if (strcmp(RINGTONE_REGISTRY[i].name, name) == 0) {{
            return RINGTONE_REGISTRY[i].text_data;
        }}
    }}
    return nullptr;
}}

// BeeperHero Track helpers
inline const uint8_t* getBeeperHeroTrackData(int index) {{
    if (index >= 0 && index < RINGTONE_COUNT) {{
        return RINGTONE_REGISTRY[index].track_data;
    }}
    return nullptr;
}}

inline const uint8_t* getBeeperHeroTrackData(const char* name) {{
    for (int i = 0; i < RINGTONE_COUNT; i++) {{
        if (strcmp(RINGTONE_REGISTRY[i].name, name) == 0) {{
            return RINGTONE_REGISTRY[i].track_data;
        }}
    }}
    return nullptr;
}}

inline size_t getBeeperHeroTrackSize(int index) {{
    if (index >= 0 && index < RINGTONE_COUNT) {{
        return RINGTONE_REGISTRY[index].track_size;
    }}
    return 0;
}}

inline size_t getBeeperHeroTrackSize(const char* name) {{
    for (int i = 0; i < RINGTONE_COUNT; i++) {{
        if (strcmp(RINGTONE_REGISTRY[i].name, name) == 0) {{
            return RINGTONE_REGISTRY[i].track_size;
        }}
    }}
    return 0;
}}

#endif // RINGTONE_DATA_H
"""
    
    return header_content

def main():
    """Main function to generate ringtone data header with caching"""
    
    # Get project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    ringtone_dir = project_root / "data" / "ringtones"
    output_file = project_root / "src" / "ringtones" / "ringtone_data.h"
    cache_file = project_root / ".ringtone_cache"
    
    print(f"Scanning for RTTTL files in: {ringtone_dir}")
    
    # Check if ringtone directory exists
    if not ringtone_dir.exists():
        print(f"Error: Ringtones directory not found: {ringtone_dir}")
        sys.exit(1)
    
    # Load cache
    cache_data = load_cache(cache_file)
    
    # Check cache validity
    cache_valid, changed_files, current_hashes = check_cache_validity(ringtone_dir, cache_data)
    
    if cache_valid and output_file.exists():
        print("✅ Cache is valid - no changes detected")
        print(f"  Using cached ringtone data: {get_relative_path(output_file, project_root)}")
        print(f"  Last update: {cache_data.get('timestamp', 'unknown')}")
        print(f"  Total ringtones: {cache_data.get('ringtone_count', 'unknown')}")
        return
    
    # Cache is invalid or output doesn't exist - regenerate
    if changed_files:
        print(f"🔄 Changes detected in files: {', '.join(changed_files)}")
    else:
        print("🔄 Regenerating ringtone data (cache invalid or missing)")
    
    # Find all RTTTL files
    ringtone_files = []
    for file_path in ringtone_dir.glob("*.rtttl.txt"):
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read().strip()
                if content:
                    rtttl_name = extract_rtttl_name(content)
                    ringtone_files.append((file_path.name, content, rtttl_name))
                    print(f"  Found: {file_path.name} -> {rtttl_name}")
        except Exception as e:
            print(f"  Warning: Could not read {file_path}: {e}")
    
    if not ringtone_files:
        print("Error: No RTTTL files found!")
        sys.exit(1)
    
    print(f"\nFound {len(ringtone_files)} ringtone files")
    
    # Generate header file
    header_content = generate_header_file(ringtone_files)
    
    # Ensure output directory exists
    output_file.parent.mkdir(parents=True, exist_ok=True)
    
    # Write header file
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(header_content)
        print(f"\nGenerated: {get_relative_path(output_file, project_root)}")
        print(f"Total ringtones: {len(ringtone_files)}")
        print("\nRingtones included:")
        for _, _, rtttl_name in ringtone_files:
            print(f"  - {rtttl_name}")
        
        # Show memory savings
        total_text_size = sum(len(content) for _, content, _ in ringtone_files)
        total_binary_size = sum(len(parse_rtttl_to_binary(content) or []) for _, content, _ in ringtone_files)
        savings = ((total_text_size - total_binary_size) / total_text_size) * 100 if total_text_size > 0 else 0
        
        print(f"\n📊 Memory Analysis:")
        print(f"  Text format: {total_text_size} bytes")
        print(f"  Binary format: {total_binary_size} bytes")
        print(f"  Memory savings: {savings:.1f}%")
        print(f"  Using binary format only for maximum efficiency")
        
        # Update cache with relative paths
        cache_data['file_hashes'] = current_hashes
        cache_data['ringtone_count'] = len(ringtone_files)
        cache_data['output_file'] = get_relative_path(output_file, project_root)
        cache_data['ringtone_dir'] = get_relative_path(ringtone_dir, project_root)
        save_cache(cache_file, cache_data)
        print(f"\n💾 Cache updated: {get_relative_path(cache_file, project_root)}")
        
    except Exception as e:
        print(f"Error writing output file: {e}")
        sys.exit(1)
    
    print("\n✅ RTTTL data generation complete!")
    print("\nNext steps:")
    print("1. Include 'ringtone_data.h' in your RingtonePlayer class")
    print("2. Use getTextRTTTL()/getBeeperHeroTrackData() as needed")
    print("3. Binary format provides significant memory savings")
    print("4. Script will automatically cache changes - re-run when adding new files")

if __name__ == "__main__":
    main() 