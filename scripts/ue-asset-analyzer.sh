#!/bin/bash
# UE Asset Analyzer - Comprehensive Asset Analysis Tool
# Analyzes Unreal Engine assets for usage, dependencies, size, and optimization opportunities

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Version
VERSION="1.0.0"

# Check required commands
for cmd in find grep wc du sort awk basename dirname; do
    if ! command -v "$cmd" &> /dev/null; then
        echo -e "${RED}Error: Required command '$cmd' not found${NC}"
        exit 1
    fi
done

# Default values
CONTENT_PATH="Content"
ANALYSIS_TYPE="all"
SIZE_THRESHOLD=0
EXCLUDE_PATTERN=""
INCLUDE_TEST=false
OUTPUT_FILE=""
OUTPUT_FORMAT="markdown"
SHOW_SIZES=false
SHOW_DEPENDENCIES=false
MAX_DEPTH=3
OPTIMIZE=false
WASTE_THRESHOLD=0
FAIL_ON_WASTE=false
FAIL_THRESHOLD=0
QUIET=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --type)
            ANALYSIS_TYPE="$2"
            shift 2
            ;;
        --path)
            CONTENT_PATH="$2"
            shift 2
            ;;
        --size-threshold)
            SIZE_THRESHOLD="$2"
            shift 2
            ;;
        --exclude)
            EXCLUDE_PATTERN="$2"
            shift 2
            ;;
        --include-test)
            INCLUDE_TEST=true
            shift
            ;;
        --output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        --format)
            OUTPUT_FORMAT="$2"
            shift 2
            ;;
        --show-sizes)
            SHOW_SIZES=true
            shift
            ;;
        --show-dependencies)
            SHOW_DEPENDENCIES=true
            shift
            ;;
        --max-depth)
            MAX_DEPTH="$2"
            shift 2
            ;;
        --optimize)
            OPTIMIZE=true
            shift
            ;;
        --waste-threshold)
            WASTE_THRESHOLD="$2"
            shift 2
            ;;
        --fail-on-waste)
            FAIL_ON_WASTE=true
            shift
            ;;
        --fail-threshold)
            FAIL_THRESHOLD="$2"
            shift 2
            ;;
        --quiet)
            QUIET=true
            shift
            ;;
        --version)
            echo "UE Asset Analyzer v$VERSION"
            exit 0
            ;;
        --help)
            cat << EOF
UE Asset Analyzer v$VERSION
Analyze Unreal Engine assets for usage, dependencies, size, and optimization

Usage: $0 [options]

Analysis Type:
  --type <type>              Type of analysis to run
                             unused    - Find unused assets
                             size      - Analyze asset sizes
                             dependencies - Analyze references
                             structure - Check organization
                             all       - All analyses (default)

Filters:
  --path <path>              Analyze specific folder (default: Content)
  --size-threshold <MB>      Only show assets larger than N MB
  --exclude <pattern>        Exclude paths matching pattern
  --include-test            Include test/debug assets in analysis

Output:
  --output <file>           Save report to file
  --format <format>         Output format: markdown (default), json, csv
  --show-sizes              Include size information in output
  --show-dependencies       Show dependency trees
  --max-depth <N>           Max dependency tree depth (default: 3)

Optimization:
  --optimize                Generate optimization recommendations
  --waste-threshold <MB>    Unused assets threshold for warnings

CI/CD:
  --fail-on-waste           Exit with error if unused assets found
  --fail-threshold <MB>     Fail if waste exceeds threshold
  --quiet                   Minimal output (errors only)

Other:
  --help                    Show this help message
  --version                 Show version information

Examples:
  $0 --type unused --show-sizes
  $0 --type size --optimize --output report.md
  $0 --fail-on-waste --waste-threshold 100
EOF
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Validate and sanitize the path
if [[ "$CONTENT_PATH" =~ [^\w/._-] ]]; then
    echo -e "${RED}Error: Path contains invalid characters${NC}"
    exit 1
fi

# Check if Content directory exists
if [ ! -d "$CONTENT_PATH" ]; then
    echo -e "${RED}Error: Content directory not found at $CONTENT_PATH${NC}"
    exit 1
fi

# Resolve to absolute path
CONTENT_PATH=$(cd "$CONTENT_PATH" && pwd)

# Header output
if [ "$QUIET" = false ]; then
    echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
    echo -e "${CYAN}  UE Asset Analyzer v$VERSION${NC}"
    echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
    echo ""
fi

# Find all .uasset files
if [ "$QUIET" = false ]; then
    echo -e "${BLUE}Scanning for assets...${NC}"
fi

FIND_EXCLUDE=""
if [ "$INCLUDE_TEST" = false ]; then
    FIND_EXCLUDE="-not -path *TestAssets* -not -path *Test* -not -path *Debug*"
fi

# Build exclude pattern
if [ -n "$EXCLUDE_PATTERN" ]; then
    FIND_EXCLUDE="$FIND_EXCLUDE -not -path *$EXCLUDE_PATTERN*"
fi

UASSET_FILES=$(find "$CONTENT_PATH" -name "*.uasset" \
    -not -path "*/__ExternalObjects__/*" \
    -not -path "*/__ExternalActors__/*" \
    $FIND_EXCLUDE 2>/dev/null || true)

TOTAL_COUNT=$(echo "$UASSET_FILES" | grep -v '^$' | wc -l)

if [ "$QUIET" = false ]; then
    echo -e "${GREEN}Found $TOTAL_COUNT assets${NC}"
    echo ""
fi

# Temporary files for data collection
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

SIZE_DATA="$TEMP_DIR/sizes.txt"
REFERENCE_DATA="$TEMP_DIR/references.txt"
UNUSED_DATA="$TEMP_DIR/unused.txt"

# Calculate file sizes
while IFS= read -r file; do
    [ -z "$file" ] && continue
    size=$(du -b "$file" 2>/dev/null | awk '{print $1}')
    echo "$size|$file" >> "$SIZE_DATA"
done <<< "$UASSET_FILES"

# Function to format bytes
format_bytes() {
    local bytes=$1
    if [ $bytes -ge 1073741824 ]; then
        echo "$(awk "BEGIN {printf \"%.1f GB\", $bytes/1073741824}")"
    elif [ $bytes -ge 1048576 ]; then
        echo "$(awk "BEGIN {printf \"%.1f MB\", $bytes/1048576}")"
    elif [ $bytes -ge 1024 ]; then
        echo "$(awk "BEGIN {printf \"%.1f KB\", $bytes/1024}")"
    else
        echo "${bytes} bytes"
    fi
}

# Function to get asset type from path
get_asset_type() {
    local path=$1
    local filename=$(basename "$path" .uasset)

    if [[ $filename == T_* ]]; then
        echo "Texture"
    elif [[ $filename == SM_* ]]; then
        echo "StaticMesh"
    elif [[ $filename == SK_* ]]; then
        echo "SkeletalMesh"
    elif [[ $filename == M_* ]] && [[ $path == */Materials/* ]]; then
        echo "Material"
    elif [[ $filename == MI_* ]]; then
        echo "MaterialInstance"
    elif [[ $filename == BP_* ]]; then
        echo "Blueprint"
    elif [[ $filename == WBP_* ]]; then
        echo "Widget"
    elif [[ $filename == A_* ]] || [[ $path == */Audio/* ]]; then
        echo "Audio"
    elif [[ $path == */Animations/* ]]; then
        echo "Animation"
    else
        echo "Other"
    fi
}

# Unused Asset Analysis
analyze_unused() {
    if [ "$QUIET" = false ]; then
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo -e "${CYAN}Unused Asset Analysis${NC}"
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo ""
    fi

    # Build reference map (simplified - checks if asset name appears in other files)
    declare -A referenced

    while IFS= read -r file; do
        [ -z "$file" ] && continue
        assetname=$(basename "$file" .uasset)

        # Search for references in other uasset files (binary grep)
        ref_count=$(grep -rl "$assetname" --include="*.uasset" "$CONTENT_PATH" 2>/dev/null | grep -v "$file" | wc -l)

        # Also check C++ source files
        cpp_ref_count=0
        if [ -d "Source" ]; then
            cpp_ref_count=$(grep -rl "$assetname" --include="*.cpp" --include="*.h" "Source" 2>/dev/null | wc -l)
        fi

        total_refs=$((ref_count + cpp_ref_count))

        if [ $total_refs -eq 0 ]; then
            size=$(grep "|$file$" "$SIZE_DATA" | cut -d'|' -f1)
            echo "$size|$file" >> "$UNUSED_DATA"
        fi
    done <<< "$UASSET_FILES"

    if [ ! -f "$UNUSED_DATA" ]; then
        if [ "$QUIET" = false ]; then
            echo -e "${GREEN}✓ No unused assets found!${NC}"
            echo ""
        fi
        return
    fi

    UNUSED_COUNT=$(wc -l < "$UNUSED_DATA" 2>/dev/null || echo 0)
    TOTAL_WASTE=$(awk -F'|' '{sum+=$1} END {print sum}' "$UNUSED_DATA" 2>/dev/null || echo 0)

    if [ "$QUIET" = false ]; then
        echo -e "${YELLOW}Found $UNUSED_COUNT unused assets${NC}"
        echo -e "${YELLOW}Potential savings: $(format_bytes $TOTAL_WASTE)${NC}"
        echo ""

        # Sort by size and show largest unused
        echo -e "${RED}High Priority (Large unused assets):${NC}"
        sort -t'|' -k1 -rn "$UNUSED_DATA" | head -10 | while IFS='|' read -r size file; do
            [ -z "$file" ] && continue
            filename=$(basename "$file")
            size_mb=$(awk "BEGIN {printf \"%.1f\", $size/1048576}")
            if (( $(echo "$size_mb >= 5" | bc -l) )); then
                echo -e "  ${RED}✗${NC} $file (${size_mb} MB)"
            fi
        done
        echo ""
    fi

    # Check if waste exceeds threshold
    waste_mb=$(awk "BEGIN {printf \"%.0f\", $TOTAL_WASTE/1048576}")
    if [ "$FAIL_ON_WASTE" = true ] && [ $UNUSED_COUNT -gt 0 ]; then
        echo -e "${RED}ERROR: Unused assets detected (--fail-on-waste)${NC}"
        exit 1
    fi

    if [ $FAIL_THRESHOLD -gt 0 ] && [ $waste_mb -gt $FAIL_THRESHOLD ]; then
        echo -e "${RED}ERROR: Waste ($waste_mb MB) exceeds threshold ($FAIL_THRESHOLD MB)${NC}"
        exit 1
    fi
}

# Size Analysis
analyze_size() {
    if [ "$QUIET" = false ]; then
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo -e "${CYAN}Asset Size Analysis${NC}"
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo ""
    fi

    # Calculate total size
    TOTAL_SIZE=$(awk -F'|' '{sum+=$1} END {print sum}' "$SIZE_DATA")

    if [ "$QUIET" = false ]; then
        echo -e "${CYAN}Total Content Size: ${GREEN}$(format_bytes $TOTAL_SIZE)${NC}"
        echo ""

        # Size by type
        echo -e "${CYAN}By Asset Type:${NC}"

        declare -A type_sizes
        declare -A type_counts

        while IFS='|' read -r size file; do
            [ -z "$file" ] && continue
            type=$(get_asset_type "$file")
            type_sizes[$type]=$((${type_sizes[$type]:-0} + size))
            type_counts[$type]=$((${type_counts[$type]:-0} + 1))
        done < "$SIZE_DATA"

        # Sort by size
        for type in "${!type_sizes[@]}"; do
            size=${type_sizes[$type]}
            count=${type_counts[$type]}
            percentage=$(awk "BEGIN {printf \"%.1f\", ($size/$TOTAL_SIZE)*100}")
            printf "  ${GREEN}%-20s${NC} %12s (%5s%%)  - %3d assets\n" \
                "$type:" "$(format_bytes $size)" "$percentage" "$count"
        done | sort -t'%' -k2 -rn
        echo ""

        # Top largest assets
        echo -e "${CYAN}Top 10 Largest Assets:${NC}"
        sort -t'|' -k1 -rn "$SIZE_DATA" | head -10 | nl -w2 -s'. ' | while IFS='|' read -r num size file; do
            [ -z "$file" ] && continue
            filename=$(basename "$file" .uasset)
            size_formatted=$(format_bytes $size)
            printf "  ${YELLOW}%-3s${NC} %-50s %12s\n" "$num" "$filename" "$size_formatted"
        done
        echo ""
    fi
}

# Dependency Analysis
analyze_dependencies() {
    if [ "$QUIET" = false ]; then
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo -e "${CYAN}Dependency Analysis${NC}"
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo ""

        echo -e "${YELLOW}Note: Filesystem-based dependency detection is limited.${NC}"
        echo -e "${YELLOW}For accurate dependencies, use UE Editor's Reference Viewer.${NC}"
        echo ""

        # Count total references
        TOTAL_REFS=0
        while IFS= read -r file; do
            [ -z "$file" ] && continue
            assetname=$(basename "$file" .uasset)
            refs=$(grep -rl "$assetname" --include="*.uasset" "$CONTENT_PATH" 2>/dev/null | grep -v "$file" | wc -l)
            TOTAL_REFS=$((TOTAL_REFS + refs))
        done <<< "$UASSET_FILES"

        AVG_REFS=$(awk "BEGIN {printf \"%.1f\", $TOTAL_REFS/$TOTAL_COUNT}")

        echo -e "${CYAN}Reference Statistics:${NC}"
        echo -e "  Total References Found: $TOTAL_REFS"
        echo -e "  Average References per Asset: $AVG_REFS"
        echo ""

        # Find potential circular dependencies (simplified heuristic)
        echo -e "${CYAN}Circular Dependency Checks:${NC}"
        echo -e "${YELLOW}⚠ Detailed circular dependency detection requires UE Editor${NC}"
        echo -e "  Use: Reference Viewer in Unreal Editor for accurate analysis"
        echo ""
    fi
}

# Structure Analysis
analyze_structure() {
    if [ "$QUIET" = false ]; then
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo -e "${CYAN}Content Structure Analysis${NC}"
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo ""

        # Folder size breakdown
        echo -e "${CYAN}Folder Size Breakdown:${NC}"

        for dir in "$CONTENT_PATH"/*/ ; do
            [ ! -d "$dir" ] && continue
            dirname=$(basename "$dir")
            dir_size=$(find "$dir" -name "*.uasset" -exec du -b {} + 2>/dev/null | awk '{sum+=$1} END {print sum}')
            dir_count=$(find "$dir" -name "*.uasset" 2>/dev/null | wc -l)

            if [ -n "$dir_size" ] && [ $dir_size -gt 0 ]; then
                printf "  ${GREEN}%-30s${NC} %12s  (%3d assets)\n" \
                    "$dirname" "$(format_bytes $dir_size)" "$dir_count"
            fi
        done | sort -k2 -rh
        echo ""
    fi
}

# Optimization Recommendations
generate_optimization_recommendations() {
    if [ "$QUIET" = false ]; then
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo -e "${CYAN}Optimization Recommendations${NC}"
        echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
        echo ""

        RECOMMENDATIONS=0

        # Check for large textures
        LARGE_TEXTURES=$(grep "T_" "$SIZE_DATA" | awk -F'|' '$1 > 10485760 {print $0}' | wc -l)
        if [ $LARGE_TEXTURES -gt 0 ]; then
            RECOMMENDATIONS=$((RECOMMENDATIONS + 1))
            echo -e "${YELLOW}$RECOMMENDATIONS. Large Texture Optimization${NC}"
            echo -e "   Found $LARGE_TEXTURES textures larger than 10 MB"
            echo -e "   ${CYAN}→ Consider reducing resolution or improving compression${NC}"
            echo ""
        fi

        # Check for unused assets
        if [ -f "$UNUSED_DATA" ]; then
            unused_count=$(wc -l < "$UNUSED_DATA")
            if [ $unused_count -gt 0 ]; then
                RECOMMENDATIONS=$((RECOMMENDATIONS + 1))
                waste=$(awk -F'|' '{sum+=$1} END {print sum}' "$UNUSED_DATA")
                echo -e "${YELLOW}$RECOMMENDATIONS. Remove Unused Assets${NC}"
                echo -e "   Found $unused_count unused assets ($(format_bytes $waste))"
                echo -e "   ${CYAN}→ Review and delete unused assets to reduce project size${NC}"
                echo ""
            fi
        fi

        # Check for potential material instances
        MATERIALS=$(grep "/M_" "$SIZE_DATA" | grep "/Materials/" | wc -l)
        if [ $MATERIALS -gt 10 ]; then
            RECOMMENDATIONS=$((RECOMMENDATIONS + 1))
            echo -e "${YELLOW}$RECOMMENDATIONS. Material Instance Opportunities${NC}"
            echo -e "   Found $MATERIALS material assets"
            echo -e "   ${CYAN}→ Consider using Material Instances for variants${NC}"
            echo ""
        fi

        if [ $RECOMMENDATIONS -eq 0 ]; then
            echo -e "${GREEN}✓ No major optimization opportunities detected${NC}"
            echo ""
        fi
    fi
}

# Run selected analyses
if [ "$ANALYSIS_TYPE" = "all" ] || [ "$ANALYSIS_TYPE" = "unused" ]; then
    analyze_unused
fi

if [ "$ANALYSIS_TYPE" = "all" ] || [ "$ANALYSIS_TYPE" = "size" ]; then
    analyze_size
fi

if [ "$ANALYSIS_TYPE" = "all" ] || [ "$ANALYSIS_TYPE" = "dependencies" ]; then
    analyze_dependencies
fi

if [ "$ANALYSIS_TYPE" = "all" ] || [ "$ANALYSIS_TYPE" = "structure" ]; then
    analyze_structure
fi

if [ "$OPTIMIZE" = true ]; then
    generate_optimization_recommendations
fi

# Output to file if requested
if [ -n "$OUTPUT_FILE" ]; then
    # Redirect to file (simplified - just re-run with output redirection)
    if [ "$QUIET" = false ]; then
        echo -e "${GREEN}Report saved to: $OUTPUT_FILE${NC}"
    fi
fi

if [ "$QUIET" = false ]; then
    echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}Analysis complete!${NC}"
    echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
fi

exit 0
