#!/usr/bin/env python3

'''
clean up the source code by removing specific lines or blocks
'''

import os
import argparse

# Define the directory containing the files
directory = "."

# Keywords for line and block removal
REMOVE_LINE_KEYWORD = "!REMOVE_LINE"
REMOVE_START_KEYWORD = "!REMOVE_START"
REMOVE_END_KEYWORD = "!REMOVE_END"

def process_file(file_path, dry_run):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    new_lines = []
    in_remove_block = False
    lines_removed = 0

    for line_num, line in enumerate(lines):
        # Check if we need to start removing a block
        if REMOVE_START_KEYWORD in line:
            if in_remove_block:
                print(f"Error: Nested or unmatched {REMOVE_START_KEYWORD} at line {line_num + 1} in {file_path}")
                return False, 0
            in_remove_block = True
            lines_removed += 1
            continue

        # Check if we need to end removing a block
        if REMOVE_END_KEYWORD in line:
            if not in_remove_block:
                print(f"Error: {REMOVE_END_KEYWORD} found without matching {REMOVE_START_KEYWORD} at line {line_num + 1} in {file_path}")
                return False, 0
            in_remove_block = False
            lines_removed += 1
            continue

        # Skip lines in the remove block
        if in_remove_block:
            lines_removed += 1
            continue

        # Remove single line containing the REMOVE_LINE_KEYWORD
        if REMOVE_LINE_KEYWORD in line:
            lines_removed += 1
            continue

        # Otherwise, keep the line
        new_lines.append(line)

    # Check if we ended in a remove block without finding REMOVE_END_KEYWORD
    if in_remove_block:
        print(f"Error: {REMOVE_START_KEYWORD} without matching {REMOVE_END_KEYWORD} in {file_path} (started at line {line_num + 1})")
        return False, 0

    # Write the modified content back to the file if changes were made and not in dry-run mode
    if lines_removed > 0 and not dry_run:
        with open(file_path, 'w') as file:
            file.writelines(new_lines)
    
    return True, lines_removed

# Command line argument parsing
parser = argparse.ArgumentParser(description="Process C and asm files to remove specific lines or blocks.")
parser.add_argument("--dry-run", action="store_true", help="Run the script without making any changes to the files")
args = parser.parse_args()

# Summary information
files_processed = 0
files_changed = 0
lines_removed_summary = {}

# Process each file in the directory
for root, _, files in os.walk(directory):
    for file_name in files:
        if file_name.endswith(('.c', '.h', '.S', 'Makefile', '.ld')):
            file_path = os.path.join(root, file_name)
            success, lines_removed = process_file(file_path, args.dry_run)
            if not success:
                print(f"Processing failed for {file_path}")
                break
            files_processed += 1
            if lines_removed > 0:
                files_changed += 1
                lines_removed_summary[file_path] = lines_removed
                print(f"Processed {file_path}, lines removed: {lines_removed}")

# Print summary
print("\nSummary:")
print(f"Total files processed: {files_processed}")
print(f"Total files changed: {files_changed}")
if files_changed > 0:
    for file_path, lines_removed in lines_removed_summary.items():
        print(f"{file_path}: {lines_removed} lines removed")
