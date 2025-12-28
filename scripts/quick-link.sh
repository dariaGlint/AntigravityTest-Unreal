#!/bin/bash
# Quick Link Helper - Source this file or call directly with arguments
# Usage: ./quick-link.sh [plugin-name] [ue5-project-path]

PLUGIN_NAME="${1:-ScreenTransitionSystem}"
UE5_PROJECT="$2"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKTREE_DIR="$(dirname "$SCRIPT_DIR")"

if [ -z "$UE5_PROJECT" ]; then
    echo "Usage: $0 <plugin-name> <ue5-project-path>"
    echo "Example: $0 ScreenTransitionSystem C:/MyUE5Project"
    exit 1
fi

SOURCE_PATH="$WORKTREE_DIR/Plugins/$PLUGIN_NAME"
TARGET_PATH="$UE5_PROJECT/Plugins/$PLUGIN_NAME"

echo "Source: $SOURCE_PATH"
echo "Target: $TARGET_PATH"

if [ ! -d "$SOURCE_PATH" ]; then
    echo "Error: Plugin not found at $SOURCE_PATH"
    exit 1
fi

mkdir -p "$UE5_PROJECT/Plugins"

if [ -e "$TARGET_PATH" ] || [ -L "$TARGET_PATH" ]; then
    echo "Removing existing: $TARGET_PATH"
    rm -rf "$TARGET_PATH"
fi

if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    # Windows (Git Bash)
    cmd //c mklink //D "$(cygpath -w "$TARGET_PATH")" "$(cygpath -w "$SOURCE_PATH")"
else
    # Linux/Mac
    ln -s "$SOURCE_PATH" "$TARGET_PATH"
fi

if [ $? -eq 0 ]; then
    echo "✓ Successfully linked!"
else
    echo "✗ Failed to create link"
    exit 1
fi
