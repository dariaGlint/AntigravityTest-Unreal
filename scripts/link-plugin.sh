#!/bin/bash
# UE5 Plugin Symlink Creator (Unix/Linux/Mac)
# This script creates a symbolic link from this worktree's plugin to your UE5 project

echo "========================================"
echo "UE5 Plugin Symlink Creator"
echo "========================================"
echo

# Get the current worktree path
WORKTREE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Ask for plugin name
read -p "Enter plugin name (e.g., ScreenTransitionSystem): " PLUGIN_NAME

# Ask for UE5 project path
read -p "Enter your UE5 project path (e.g., /home/user/MyProject): " UE5_PROJECT

# Set paths
SOURCE_PATH="$WORKTREE_DIR/Plugins/$PLUGIN_NAME"
TARGET_PATH="$UE5_PROJECT/Plugins/$PLUGIN_NAME"

echo
echo "Source: $SOURCE_PATH"
echo "Target: $TARGET_PATH"
echo

# Check if source exists
if [ ! -d "$SOURCE_PATH" ]; then
    echo "ERROR: Plugin '$PLUGIN_NAME' not found in worktree"
    echo "Path: $SOURCE_PATH"
    exit 1
fi

# Create Plugins directory in UE5 project if it doesn't exist
if [ ! -d "$UE5_PROJECT/Plugins" ]; then
    echo "Creating Plugins directory in UE5 project..."
    mkdir -p "$UE5_PROJECT/Plugins"
fi

# Check if target already exists
if [ -e "$TARGET_PATH" ] || [ -L "$TARGET_PATH" ]; then
    echo "WARNING: Target already exists!"
    read -p "Do you want to remove it and create a new symlink? (y/n): " CONFIRM
    if [ "$CONFIRM" != "y" ] && [ "$CONFIRM" != "Y" ]; then
        echo "Cancelled."
        exit 0
    fi

    # Remove existing directory/symlink
    rm -rf "$TARGET_PATH"
fi

echo
echo "Creating symbolic link..."

# Create symbolic link
ln -s "$SOURCE_PATH" "$TARGET_PATH"

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: Failed to create symbolic link"
    exit 1
fi

echo
echo "========================================"
echo "SUCCESS!"
echo "========================================"
echo "Symbolic link created successfully!"
echo
echo "You can now open your UE5 project and the plugin will be available."
echo "Any changes made in this worktree will be immediately reflected in your UE5 project."
echo
