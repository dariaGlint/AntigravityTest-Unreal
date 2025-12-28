#!/bin/bash
# UE5 Plugin Syntax Checker
# Checks for common C++ syntax errors in the plugin

PLUGIN_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/Plugins/ScreenTransitionSystem"
ERRORS=0

echo "========================================"
echo "UE5 Plugin Syntax Checker"
echo "========================================"
echo "Checking: $PLUGIN_DIR"
echo

# Check 1: All header files have include guards or #pragma once
echo "[1/5] Checking header guards..."
for header in $(find "$PLUGIN_DIR/Source" -name "*.h"); do
    if ! grep -q "#pragma once" "$header"; then
        echo "  ⚠️  Missing #pragma once: $header"
        ERRORS=$((ERRORS + 1))
    fi
done

# Check 2: All UCLASS/USTRUCT have GENERATED_BODY
echo "[2/5] Checking GENERATED_BODY..."
for file in $(find "$PLUGIN_DIR/Source" -name "*.h"); do
    if grep -q "UCLASS\|USTRUCT\|UENUM" "$file"; then
        if ! grep -q "GENERATED_BODY\|GENERATED_USTRUCT_BODY" "$file"; then
            echo "  ⚠️  Missing GENERATED_BODY: $file"
            ERRORS=$((ERRORS + 1))
        fi
    fi
done

# Check 3: Check for common syntax errors
echo "[3/5] Checking for common syntax errors..."
for file in $(find "$PLUGIN_DIR/Source" -name "*.cpp" -o -name "*.h"); do
    # Check for mismatched braces (basic check)
    OPEN=$(grep -o "{" "$file" | wc -l)
    CLOSE=$(grep -o "}" "$file" | wc -l)
    if [ "$OPEN" -ne "$CLOSE" ]; then
        echo "  ⚠️  Mismatched braces: $file (open: $OPEN, close: $CLOSE)"
        ERRORS=$((ERRORS + 1))
    fi
done

# Check 4: All .cpp files include their corresponding .h
echo "[4/5] Checking include statements..."
for cpp in $(find "$PLUGIN_DIR/Source" -name "*.cpp"); do
    BASENAME=$(basename "$cpp" .cpp)
    HEADER="${BASENAME}.h"

    if ! grep -q "#include \"$HEADER\"" "$cpp"; then
        echo "  ⚠️  Missing header include in: $cpp"
        ERRORS=$((ERRORS + 1))
    fi
done

# Check 5: Check for GENERATED.h includes in headers with UCLASS/USTRUCT
echo "[5/5] Checking .generated.h includes..."
for header in $(find "$PLUGIN_DIR/Source" -name "*.h"); do
    if grep -q "UCLASS\|USTRUCT\|UENUM" "$header"; then
        BASENAME=$(basename "$header" .h)
        if ! grep -q "#include \"${BASENAME}.generated.h\"" "$header"; then
            echo "  ⚠️  Missing .generated.h include: $header"
            ERRORS=$((ERRORS + 1))
        fi
    fi
done

echo
echo "========================================"
if [ $ERRORS -eq 0 ]; then
    echo "✓ No syntax errors found!"
    echo "========================================"
    exit 0
else
    echo "✗ Found $ERRORS potential issue(s)"
    echo "========================================"
    echo
    echo "Note: These are basic checks. Full compilation may reveal other issues."
    exit 1
fi
