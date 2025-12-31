# UE Asset Analyzer

Analyze Unreal Engine assets for usage, dependencies, size optimization, and project health.

## Overview

This command provides comprehensive asset analysis to help optimize project size, identify unused assets, analyze dependencies, and improve asset management in Unreal Engine 5 projects.

## Features

### 1. Unused Asset Detection
- Identify assets with no references
- Find orphaned assets (assets referencing nothing and referenced by nothing)
- Detect test/debug assets that can be removed
- Find duplicate assets with different names

### 2. Asset Size Analysis
- List largest assets by type
- Calculate folder size statistics
- Analyze texture, mesh, audio, and animation sizes
- Generate size optimization recommendations
- Identify over-sized textures and uncompressed audio

### 3. Dependency Analysis
- Map asset reference chains
- Detect circular dependencies
- Find redirector assets that need cleanup
- Analyze packaging impact (what gets included)
- Identify assets that pull in large dependency trees

### 4. Content Browser Structure
- Verify naming convention compliance
- Check folder organization consistency
- Generate asset type distribution statistics
- Identify misplaced assets

### 5. Optimization Recommendations
- Texture compression settings
- LOD configuration suggestions
- Material instance opportunities
- Asset consolidation proposals
- Packaging exclusion recommendations

## Usage

### Basic Analysis

```bash
# Full asset analysis
./scripts/ue-asset-analyzer.sh

# Analyze specific folder
./scripts/ue-asset-analyzer.sh --path Content/Variant_Combat

# Focus on specific analysis type
./scripts/ue-asset-analyzer.sh --type unused
./scripts/ue-asset-analyzer.sh --type size
./scripts/ue-asset-analyzer.sh --type dependencies
```

### Advanced Options

```bash
# Find assets larger than threshold (in MB)
./scripts/ue-asset-analyzer.sh --size-threshold 10

# Show detailed dependency tree
./scripts/ue-asset-analyzer.sh --show-dependencies --max-depth 3

# Generate optimization report
./scripts/ue-asset-analyzer.sh --optimize --output report.md

# Find unused assets with size impact
./scripts/ue-asset-analyzer.sh --type unused --show-sizes

# CI/CD integration - fail on large unused assets
./scripts/ue-asset-analyzer.sh --fail-on-waste --waste-threshold 100

# Export JSON for external tools
./scripts/ue-asset-analyzer.sh --format json --output assets.json
```

## Analysis Types

### Unused Asset Analysis (`--type unused`)
Identifies assets that are not referenced by other assets or code:
- Scans for references in .uasset files
- Checks for code references in C++ files
- Reports potential savings in MB
- Suggests safe removal candidates

### Size Analysis (`--type size`)
Analyzes asset sizes and storage impact:
- Top N largest assets
- Size breakdown by type (Textures, Meshes, Audio, etc.)
- Folder size statistics
- Compression opportunities
- Texture resolution analysis

### Dependency Analysis (`--type dependencies`)
Maps asset relationships:
- Reference chains (A → B → C)
- Circular reference detection
- Redirector cleanup opportunities
- Packaging impact analysis
- Dependency tree depth metrics

### Structure Analysis (`--type structure`)
Validates project organization:
- Naming convention compliance
- Folder structure validation
- Asset type distribution
- Misplaced asset detection

## Output Examples

### 1. Unused Assets Report

```
════════════════════════════════════════════════════════
  Unused Asset Analysis
════════════════════════════════════════════════════════

Found 23 unused assets (potential savings: 187 MB)

High Priority (Large unused assets):
  ✗ Content/TestAssets/T_Debug_Large_4K.uasset (45 MB)
  ✗ Content/OldAssets/SK_Character_V1.uasset (32 MB)
  ✗ Content/Temp/M_PrototypeMaterial.uasset (15 MB)

Medium Priority:
  ⚠ Content/Unused/BP_OldGameMode.uasset (2.1 MB)
  ⚠ Content/Debug/WBP_TestUI.uasset (1.8 MB)

Low Priority (Small assets):
  • Content/Temp/MI_TempInstance.uasset (0.3 MB)
  ... (15 more)

Safe to Remove:
- All assets in Content/TestAssets/
- Assets in Content/OldAssets/
- Debug materials and blueprints

Requires Verification:
- Content/Unused/BP_OldGameMode.uasset (may have Blueprint references)
```

### 2. Size Analysis Report

```
════════════════════════════════════════════════════════
  Asset Size Analysis
════════════════════════════════════════════════════════

Total Content Size: 2.4 GB

By Asset Type:
  Textures:              1.2 GB (50%)  - 234 assets
  Static Meshes:         680 MB (28%)  - 156 assets
  Skeletal Meshes:       320 MB (13%)  - 45 assets
  Audio:                 150 MB (6%)   - 89 assets
  Animations:            50 MB (2%)    - 67 assets
  Other:                 30 MB (1%)    - 123 assets

Top 10 Largest Assets:
  1. T_Environment_Atlas_4K.uasset          120 MB
  2. SK_MainCharacter_WithAnimations.uasset  85 MB
  3. T_Terrain_Heightmap.uasset              78 MB
  4. SM_DetailedEnvironment.uasset           65 MB
  5. A_CombatMusic_Uncompressed.uasset       45 MB
  ...

Optimization Opportunities:
  ✓ 15 textures can be reduced to 2K (save ~200 MB)
  ✓ 8 audio files can be compressed (save ~60 MB)
  ✓ 12 meshes missing LODs (improve performance)
  ✓ 5 materials can be instanced (reduce shader count)

Estimated Savings: 260 MB (10.8%)
```

### 3. Dependency Analysis Report

```
════════════════════════════════════════════════════════
  Dependency Analysis
════════════════════════════════════════════════════════

Asset Relationships:
  Total References Analyzed: 1,234
  Average Reference Depth: 3.2
  Max Reference Depth: 8

Circular Dependencies Detected: 2
  ⚠ BP_Character ↔ BP_Weapon ↔ BP_Character
    Location: Content/Variant_Combat/Blueprints/
    Impact: May cause packaging issues

  ⚠ M_Master ↔ MI_Instance ↔ M_Master
    Location: Content/Variant_Combat/Materials/
    Impact: Material compilation issues

Redirectors Found: 12
  → T_OldTexture_001 → T_NewTexture_001
  → BP_OldCharacter → BP_Character
  ... (10 more)
  Recommendation: Run "Fix Up Redirectors" in editor

Heavy Dependency Trees (assets pulling many dependencies):
  BP_CombatCharacter (pulls 47 assets, 125 MB)
  BP_MainMenu (pulls 35 assets, 89 MB)
  M_MasterMaterial (pulls 28 assets, 45 MB)

Packaging Impact:
  Content/TestAssets/ - NOT included in builds (excluded)
  Content/Variant_Combat/ - 45 assets, 680 MB
  Content/Core/ - 120 assets, 890 MB
```

### 4. Optimization Recommendations

```
════════════════════════════════════════════════════════
  Optimization Recommendations
════════════════════════════════════════════════════════

Priority Actions:

1. Remove Unused Assets (High Impact)
   - Delete 23 unused assets to save 187 MB
   - Focus on: TestAssets/, OldAssets/, Debug/

2. Texture Optimization (High Impact)
   - Reduce 15 textures from 4K to 2K
   - Estimated Savings: 200 MB
   - Files:
     • T_Environment_Detail_4K → 2K (12 MB saved)
     • T_Character_Normal_4K → 2K (8 MB saved)
     ...

3. Audio Compression (Medium Impact)
   - Compress 8 audio files
   - Estimated Savings: 60 MB
   - Current: Uncompressed WAV
   - Recommend: OGG Vorbis Quality 0.4

4. LOD Addition (Performance)
   - Add LODs to 12 static meshes
   - Improve rendering performance
   - Files:
     • SM_DetailedEnvironment (no LODs)
     • SM_ComplexProp (no LODs)
     ...

5. Material Instancing (Shader Optimization)
   - Convert 5 materials to instances
   - Reduce shader permutations
   - Parent: M_MasterMaterial
   - Candidates:
     • M_Character_Variant1 → MI_
     • M_Character_Variant2 → MI_
     ...

6. Fix Circular Dependencies (Critical)
   - Resolve 2 circular reference issues
   - May prevent packaging or cause runtime issues

7. Clean Up Redirectors (Maintenance)
   - Fix 12 redirector references
   - Run: Content Browser → Fix Up Redirectors in Folder

Total Potential Savings: 447 MB (18.6%)
Performance Improvements: 12 meshes + 5 materials
```

## Command Line Options

```
Usage: ./scripts/ue-asset-analyzer.sh [options]

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
```

## Implementation Details

### Asset Detection Methods

1. **File System Analysis**
   - Scan `.uasset` files in Content directory
   - Read file sizes from filesystem
   - Parse file paths for naming conventions

2. **Reference Scanning** (Phase 1 - Basic)
   - Search for asset references in `.uasset` files (binary grep)
   - Search for references in C++ source files
   - Build reference map

3. **Unreal Editor Integration** (Phase 2 - Advanced, requires UE Editor)
   - Use Asset Registry for accurate dependency data
   - Query reference viewer data
   - Access actual asset metadata
   - Get compression settings and recommendations

### Size Calculation

```bash
# File size detection
find Content -name "*.uasset" -exec du -b {} \; | sort -n

# Folder aggregation
du -sh Content/*/ | sort -h

# Asset type categorization by extension and path
```

### Dependency Mapping

```bash
# Basic reference detection (grep in binary files)
grep -r "AssetName" Content/*.uasset

# Build dependency graph
# For each asset: find all references → recursive scan → build tree
```

### Unused Asset Detection Logic

An asset is considered "unused" if:
1. No other `.uasset` files reference it (checked via binary grep)
2. No C++ source files reference it (checked via text grep)
3. Not in excluded paths (e.g., `Content/TestAssets` is explicitly test content)
4. Not a primary game asset (GameMode, Character blueprints in use)

**False Positives**: Some assets may be:
- Loaded dynamically at runtime (require manual verification)
- Referenced by string paths in code (hard to detect)
- Used in editor-only workflows

The tool marks these as "Requires Verification" rather than "Safe to Remove".

## Configuration

Create `Config/AssetAnalyzer.ini` for custom settings:

```ini
[AssetAnalyzer]
; Paths to exclude from unused asset detection
ExcludePaths=Content/__ExternalActors__,Content/__ExternalObjects__,Content/Collections

; File patterns to always include in package
AlwaysInclude=*_GameMode,*_Character,BP_*Controller

; Size thresholds (MB)
LargeAssetThreshold=10
WasteThreshold=100

; Optimization settings
TextureMaxResolution=2048
AudioCompressionQuality=0.4
RequireLODsForMeshesOver=1000  ; triangles

[TextureOptimization]
; Texture compression recommendations
DefaultCompression=DXT5
NormalMapCompression=BC5
UITextureCompression=BC7

[DependencyAnalysis]
MaxDepth=5
ShowCircularReferences=true
ShowRedirectors=true
```

## Integration with CI/CD

```yaml
# GitHub Actions example
- name: Analyze Assets
  run: |
    ./scripts/ue-asset-analyzer.sh \
      --type all \
      --fail-on-waste \
      --waste-threshold 200 \
      --output asset-report.md

- name: Upload Report
  uses: actions/upload-artifact@v3
  with:
    name: asset-analysis
    path: asset-report.md
```

## Limitations

### Phase 1 (Filesystem-based)
- Cannot detect runtime-loaded assets with certainty
- Binary reference detection may have false negatives
- No access to actual UE metadata (compression settings, etc.)
- Cannot analyze Blueprint node usage

### Phase 2 (UE Editor Integration - Future)
- Requires Unreal Editor to be running
- Uses Python API for Asset Registry access
- Accurate dependency tracking
- Access to compression and quality settings
- Can detect runtime references

## Related Tools

- **analyze-blueprints**: Blueprint-specific analysis (naming, structure, performance)
- **Unreal Editor**: Reference Viewer, Size Map, Asset Audit
- **UE Asset Manager**: Runtime asset loading management

## Example Workflows

### Clean Up Project Before Release

```bash
# 1. Find all unused assets
./scripts/ue-asset-analyzer.sh --type unused --show-sizes --output unused.md

# 2. Review the report and verify safe removal
# Manual step: Check unused.md

# 3. Analyze size optimization opportunities
./scripts/ue-asset-analyzer.sh --type size --optimize --output optimize.md

# 4. Check dependencies before making changes
./scripts/ue-asset-analyzer.sh --type dependencies --show-dependencies
```

### Regular Project Health Check

```bash
# Weekly automated check
./scripts/ue-asset-analyzer.sh \
  --type all \
  --optimize \
  --fail-on-waste \
  --waste-threshold 500 \
  --output weekly-report.md
```

## Output Files

- **Markdown Reports**: Human-readable analysis results
- **JSON Export**: Machine-readable data for external tools
- **CSV Export**: Spreadsheet import for tracking

## Future Enhancements

- [ ] Phase 2: UE Python API integration for accurate metadata
- [ ] Automatic redirector cleanup
- [ ] Asset consolidation suggestions with preview
- [ ] Texture resolution analyzer with visual preview
- [ ] Integration with Perforce/Git for change tracking
- [ ] Historical trend analysis (size over time)
- [ ] Comparison between branches
- [ ] Auto-fix for simple optimizations

## Version History

- v1.0 (2025-12-30): Initial implementation with filesystem-based analysis
  - Unused asset detection
  - Size analysis
  - Basic dependency mapping
  - Optimization recommendations
