#!/bin/bash
# Blueprint Analyzer - Phase 1 Implementation
# Analyzes Unreal Engine Blueprint assets for naming conventions and structure

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Check required commands
for cmd in find grep wc basename dirname; do
    if ! command -v "$cmd" &> /dev/null; then
        echo -e "${RED}Error: Required command '$cmd' not found${NC}"
        exit 1
    fi
done

# Default values
CONTENT_PATH="Content"
ANALYSIS_TYPE="full"
GENERATE_DIAGRAM=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --path)
            CONTENT_PATH="$2"
            shift 2
            ;;
        --type)
            ANALYSIS_TYPE="$2"
            shift 2
            ;;
        --diagram)
            GENERATE_DIAGRAM=true
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --path <path>      Path to analyze (default: Content)"
            echo "  --type <type>      Asset type to focus on (blueprints|ui|materials|all)"
            echo "  --diagram          Generate Mermaid diagram"
            echo "  --help             Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Validate and sanitize the path
# Remove any special characters that could be dangerous
if [[ "$CONTENT_PATH" =~ [^\w/._-] ]]; then
    echo -e "${RED}Error: Path contains invalid characters. Only alphanumeric, /, ., _, and - are allowed.${NC}"
    exit 1
fi

# Check if Content directory exists
if [ ! -d "$CONTENT_PATH" ]; then
    echo -e "${RED}Error: Content directory not found at $CONTENT_PATH${NC}"
    exit 1
fi

# Resolve to absolute path to prevent path traversal
CONTENT_PATH=$(cd "$CONTENT_PATH" && pwd)

echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}  Blueprint Analysis Report${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
echo ""

# Find all .uasset files
echo -e "${BLUE}Scanning for Blueprint assets...${NC}"
UASSET_FILES=$(find "$CONTENT_PATH" -name "*.uasset" -not -path "*/__ExternalObjects__/*" 2>/dev/null || true)
TOTAL_COUNT=$(echo "$UASSET_FILES" | grep -v '^$' | wc -l)

echo -e "${GREEN}Found $TOTAL_COUNT assets${NC}"
echo ""

# Naming convention patterns
declare -A PREFIXES=(
    ["BP_"]="Blueprint Classes"
    ["WBP_"]="Widget Blueprints"
    ["M_"]="Materials"
    ["MI_"]="Material Instances"
    ["T_"]="Textures"
    ["IA_"]="Input Actions"
    ["IMC_"]="Input Mapping Contexts"
    ["NS_"]="Niagara Systems"
    ["ST_"]="State Trees"
    ["BT_"]="Behavior Trees"
    ["BB_"]="Blackboards"
    ["EQS_"]="Environment Queries"
    ["BPI_"]="Blueprint Interfaces"
)

# Count assets by prefix (single-pass optimization)
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo -e "${CYAN}1. Asset Statistics by Type${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo ""

declare -A PREFIX_COUNTS
# Initialize all counts to 0
for prefix in "${!PREFIXES[@]}"; do
    PREFIX_COUNTS[$prefix]=0
done

# Single pass through all files
while IFS= read -r file; do
    [ -z "$file" ] && continue
    filename=$(basename "$file")

    # Check each prefix and count once
    for prefix in "${!PREFIXES[@]}"; do
        if [[ $filename == $prefix* ]]; then
            ((PREFIX_COUNTS[$prefix]++))
            break  # Only count once per file
        fi
    done
done <<< "$UASSET_FILES"

# Display results
for prefix in "${!PREFIXES[@]}"; do
    count=${PREFIX_COUNTS[$prefix]}
    if [ $count -gt 0 ]; then
        printf "${GREEN}%-25s${NC} %3d\n" "${PREFIXES[$prefix]} (${prefix}*):" "$count"
    fi
done

# Count files without recognized prefix (calculate from total minus all prefixed)
TOTAL_PREFIXED=0
for prefix in "${!PREFIXES[@]}"; do
    TOTAL_PREFIXED=$((TOTAL_PREFIXED + PREFIX_COUNTS[$prefix]))
done
OTHER_COUNT=$((TOTAL_COUNT - TOTAL_PREFIXED))

printf "${YELLOW}%-25s${NC} %3d\n" "Other/Unknown:" "$OTHER_COUNT"
echo ""

# Count by variant
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo -e "${CYAN}2. Assets by Variant${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo ""

COMBAT_COUNT=$(echo "$UASSET_FILES" | grep "Variant_Combat" | wc -l)
SIDESCROLLING_COUNT=$(echo "$UASSET_FILES" | grep "Variant_SideScrolling" | wc -l)
PLATFORMING_COUNT=$(echo "$UASSET_FILES" | grep "Variant_Platforming" | wc -l)
CORE_COUNT=$(( TOTAL_COUNT - COMBAT_COUNT - SIDESCROLLING_COUNT - PLATFORMING_COUNT ))

printf "${GREEN}%-25s${NC} %3d\n" "Variant_Combat:" "$COMBAT_COUNT"
printf "${GREEN}%-25s${NC} %3d\n" "Variant_SideScrolling:" "$SIDESCROLLING_COUNT"
printf "${GREEN}%-25s${NC} %3d\n" "Variant_Platforming:" "$PLATFORMING_COUNT"
printf "${GREEN}%-25s${NC} %3d\n" "Core/Shared:" "$CORE_COUNT"
echo ""

# Check for naming convention violations
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo -e "${CYAN}3. Naming Convention Analysis${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo ""

VIOLATIONS=()
while IFS= read -r file; do
    [ -z "$file" ] && continue
    filename=$(basename "$file" .uasset)
    filepath=$(dirname "$file")

    # Skip external objects
    [[ $filepath == *"__ExternalObjects__"* ]] && continue

    has_prefix=false
    for prefix in "${!PREFIXES[@]}"; do
        if [[ $filename == $prefix* ]]; then
            has_prefix=true
            break
        fi
    done

    # Check if file looks like it should have a prefix
    if [ "$has_prefix" = false ]; then
        # Determine what prefix it should have based on location
        suggested_prefix=""
        if [[ $filepath == *"/Blueprints/"* ]] && [[ $filepath != *"/UI/"* ]]; then
            suggested_prefix="BP_"
        elif [[ $filepath == *"/UI/"* ]]; then
            suggested_prefix="WBP_"
        elif [[ $filepath == *"/Materials/"* ]]; then
            if [[ $filename == MI_* ]]; then
                suggested_prefix="" # Already correct
            else
                suggested_prefix="M_"
            fi
        elif [[ $filepath == *"/Input/"* ]]; then
            if [[ $filepath == *"/Actions/"* ]]; then
                suggested_prefix="IA_"
            elif [[ $filename == IMC_* ]]; then
                suggested_prefix="" # Already correct
            fi
        fi

        if [ -n "$suggested_prefix" ]; then
            VIOLATIONS+=("$file|$suggested_prefix")
        fi
    fi
done <<< "$UASSET_FILES"

if [ ${#VIOLATIONS[@]} -gt 0 ]; then
    echo -e "${YELLOW}Found ${#VIOLATIONS[@]} potential naming convention violations:${NC}"
    echo ""
    for violation in "${VIOLATIONS[@]}"; do
        file=$(echo "$violation" | cut -d'|' -f1)
        prefix=$(echo "$violation" | cut -d'|' -f2)
        filename=$(basename "$file")
        echo -e "  ${RED}✗${NC} $file"
        echo -e "    ${YELLOW}→ Suggest: ${prefix}${filename}${NC}"
    done
else
    echo -e "${GREEN}✓ No naming convention violations found!${NC}"
fi
echo ""

# Folder structure analysis
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo -e "${CYAN}4. Folder Structure Analysis${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo ""

# Check for assets in root Content folder
ROOT_ASSETS=$(echo "$UASSET_FILES" | grep -E "^Content/[^/]+\.uasset$" || true)
ROOT_COUNT=$(echo "$ROOT_ASSETS" | grep -v '^$' | wc -l)

if [ $ROOT_COUNT -gt 0 ]; then
    echo -e "${YELLOW}Found $ROOT_COUNT assets in root Content folder:${NC}"
    while IFS= read -r file; do
        [ -z "$file" ] && continue
        filename=$(basename "$file")
        echo -e "  ${YELLOW}⚠${NC} $filename"
        # Suggest proper location based on prefix
        if [[ $filename == BP_* ]]; then
            echo -e "    ${CYAN}→ Suggest: Content/Blueprints/$filename${NC}"
        elif [[ $filename == WBP_* ]]; then
            echo -e "    ${CYAN}→ Suggest: Content/UI/$filename${NC}"
        fi
    done <<< "$ROOT_ASSETS"
    echo ""
else
    echo -e "${GREEN}✓ No assets in root Content folder${NC}"
    echo ""
fi

# Blueprint categorization
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo -e "${CYAN}5. Blueprint Categories${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo ""

# Find character blueprints
CHAR_BP=$(echo "$UASSET_FILES" | grep "BP_.*Character" || true)
CHAR_COUNT=$(echo "$CHAR_BP" | grep -v '^$' | wc -l)

# Find game mode blueprints
GM_BP=$(echo "$UASSET_FILES" | grep "BP_.*GameMode" || true)
GM_COUNT=$(echo "$GM_BP" | grep -v '^$' | wc -l)

# Find AI blueprints
AI_BP=$(echo "$UASSET_FILES" | grep -E "(AIController|ST_|BT_|BB_)" || true)
AI_COUNT=$(echo "$AI_BP" | grep -v '^$' | wc -l)

# Find interactable blueprints
INTERACT_BP=$(echo "$UASSET_FILES" | grep -E "BP_.*(Dummy|Floor|Box|Spawner)" || true)
INTERACT_COUNT=$(echo "$INTERACT_BP" | grep -v '^$' | wc -l)

# Find UI widgets
UI_BP=$(echo "$UASSET_FILES" | grep -E "(WBP_|UI_)" || true)
UI_COUNT=$(echo "$UI_BP" | grep -v '^$' | wc -l)

echo "├── Character Blueprints: $CHAR_COUNT"
echo "├── Game Mode Blueprints: $GM_COUNT"
echo "├── AI Components: $AI_COUNT"
echo "├── Interactables: $INTERACT_COUNT"
echo "└── UI Widgets: $UI_COUNT"
echo ""

# Recommendations
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo -e "${CYAN}6. Recommendations${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
echo ""

RECOMMENDATION_COUNT=0

if [ ${#VIOLATIONS[@]} -gt 0 ]; then
    RECOMMENDATION_COUNT=$((RECOMMENDATION_COUNT + 1))
    echo -e "${YELLOW}$RECOMMENDATION_COUNT.${NC} Fix ${#VIOLATIONS[@]} naming convention violations"
fi

if [ $ROOT_COUNT -gt 0 ]; then
    RECOMMENDATION_COUNT=$((RECOMMENDATION_COUNT + 1))
    echo -e "${YELLOW}$RECOMMENDATION_COUNT.${NC} Move $ROOT_COUNT assets from root Content folder to appropriate subdirectories"
fi

if [ $RECOMMENDATION_COUNT -eq 0 ]; then
    echo -e "${GREEN}✓ Project structure looks good!${NC}"
else
    echo ""
    echo -e "${CYAN}Consider organizing assets into these suggested folders:${NC}"
    echo "  - Content/Core/Blueprints/ (for shared Blueprints)"
    echo "  - Content/Variant_*/Blueprints/Characters/"
    echo "  - Content/Variant_*/Blueprints/GameModes/"
    echo "  - Content/Variant_*/Blueprints/AI/"
fi
echo ""

# Generate Mermaid diagram if requested
if [ "$GENERATE_DIAGRAM" = true ]; then
    echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
    echo -e "${CYAN}7. Project Structure Diagram${NC}"
    echo -e "${CYAN}─────────────────────────────────────────────────────${NC}"
    echo ""
    echo '```mermaid'
    echo 'graph TD'
    echo '    Root[Content Directory]'

    if [ $COMBAT_COUNT -gt 0 ]; then
        echo '    Root --> Combat[Variant_Combat]'
        echo '    Combat --> CombatBP[Blueprints: '$CHAR_COUNT' chars]'
        echo '    Combat --> CombatUI[UI: '$(echo "$UASSET_FILES" | grep "Variant_Combat/UI" | wc -l)' widgets]'
        echo '    Combat --> CombatAI[AI: '$(echo "$UASSET_FILES" | grep "Variant_Combat.*AI" | wc -l)' components]'
    fi

    if [ $SIDESCROLLING_COUNT -gt 0 ]; then
        echo '    Root --> SideScrolling[Variant_SideScrolling]'
    fi

    if [ $PLATFORMING_COUNT -gt 0 ]; then
        echo '    Root --> Platforming[Variant_Platforming]'
    fi

    if [ $CORE_COUNT -gt 0 ]; then
        echo '    Root --> Core[Core Assets: '$CORE_COUNT']'
    fi

    echo '```'
    echo ""
fi

echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}Analysis complete!${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
