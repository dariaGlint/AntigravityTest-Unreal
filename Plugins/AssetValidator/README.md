# Asset Validator Plugin

A comprehensive asset validation system for Unreal Engine 5 projects that ensures naming conventions, folder structure, and asset integrity.

## Features

### Phase 1: Basic Validation (Implemented)

- **Asset Naming Convention Validation**
  - Checks for proper prefixes (T_, M_, MI_, BP_, SM_, etc.)
  - Detects forbidden characters and spaces
  - Customizable naming rules
  - Supports all common asset types

- **Folder Structure Validation**
  - Validates assets are in recommended folders
  - Customizable folder rules
  - Project-specific structure validation

- **Asset Statistics**
  - Total asset count and categorization
  - Asset distribution by type
  - Detection of potentially unused assets

## Installation

1. Copy the `AssetValidator` folder to your project's `Plugins/` directory
2. Restart Unreal Editor
3. Enable the plugin in Edit → Plugins → Development
4. Restart the editor when prompted

## Usage

### Blueprint Usage

The Asset Validator Subsystem can be accessed from any Blueprint:

```cpp
// Get the subsystem
UAssetValidatorSubsystem* ValidatorSubsystem = GEditor->GetEditorSubsystem<UAssetValidatorSubsystem>();

// Validate all assets in the project
FValidationResult Result = ValidatorSubsystem->ValidateAssets("/Game/", false, true);

// Validate a single asset
FValidationResult SingleResult = ValidatorSubsystem->ValidateSingleAsset("/Game/Materials/M_MyMaterial");

// Get asset statistics
FAssetStatistics Stats = ValidatorSubsystem->GetAssetStatistics("/Game/");

// Print results to log
ValidatorSubsystem->PrintValidationResults(Result);

// Export results to file
ValidatorSubsystem->ExportValidationResults(Result, "D:/ValidationReport.txt");
```

### C++ Usage

```cpp
#include "AssetValidatorSubsystem.h"

UAssetValidatorSubsystem* Validator = GEditor->GetEditorSubsystem<UAssetValidatorSubsystem>();

// Validate all assets
FValidationResult Result = Validator->ValidateAssets(TEXT("/Game/"), false, true);

// Print to log
Validator->PrintValidationResults(Result);
```

### Command Line Usage

You can integrate the validator into your CI/CD pipeline using Unreal's command line tools:

```bash
# Example: Run validation via Python script
UnrealEditor.exe YourProject.uproject -run=pythonscript -script="validate_assets.py"
```

## Naming Conventions

The plugin validates the following naming conventions by default:

| Asset Type | Prefix | Example |
|------------|--------|---------|
| Texture2D | T_ | T_Character_Diffuse |
| Material | M_ | M_Character |
| Material Instance | MI_ | MI_Character_Blue |
| Static Mesh | SM_ | SM_Rock |
| Skeletal Mesh | SK_ | SK_Character |
| Animation Sequence | A_ | A_Run |
| Animation Montage | AM_ | AM_Attack |
| Blueprint | BP_ | BP_Character |
| Widget Blueprint | WBP_ | WBP_MainMenu |
| Input Action | IA_ | IA_Jump |
| Input Mapping Context | IMC_ | IMC_Default |

### Customizing Naming Rules

```cpp
UAssetNamingValidator* NamingValidator = Validator->GetNamingValidator();

// Add custom rule
FNamingRule CustomRule;
CustomRule.AssetClass = TEXT("CustomAsset");
CustomRule.RequiredPrefix = TEXT("CA_");
CustomRule.Description = TEXT("Custom assets should start with CA_");

NamingValidator->AddNamingRule(CustomRule);
```

## Folder Structure Rules

Recommended folder structure:

- `/Game/Blueprints/` - Blueprint classes
- `/Game/Materials/` - Materials and material instances
- `/Game/Textures/` - Textures
- `/Game/Meshes/` - Static and skeletal meshes
- `/Game/Animations/` or `/Game/Anims/` - Animation assets
- `/Game/UI/` or `/Game/Widgets/` - UI widgets
- `/Game/Audio/` or `/Game/Sounds/` - Audio files
- `/Game/VFX/` - Visual effects
- `/Game/Input/` - Input actions and mapping contexts
- `/Game/Data/` - Data tables and assets

### Customizing Folder Rules

```cpp
UAssetFolderValidator* FolderValidator = Validator->GetFolderValidator();

// Add custom rule
FFolderRule CustomFolderRule;
CustomFolderRule.AssetClass = TEXT("CustomAsset");
CustomFolderRule.RecommendedFolders = {TEXT("CustomAssets"), TEXT("Special")};
CustomFolderRule.Description = TEXT("Custom assets should be in CustomAssets or Special folder");

FolderValidator->AddFolderRule(CustomFolderRule);
```

## Validation Results

The validation system returns detailed results including:

- **Issue Severity**: Error, Warning, or Info
- **Issue Type**: Naming Convention, Folder Structure, etc.
- **Asset Path**: Full path to the problematic asset
- **Message**: Description of the issue
- **Suggestion**: Recommended fix

Example output:

```
=== Asset Validation Results ===
Total Assets Checked: 150
Errors: 5
Warnings: 23
Info: 12

=== Issues Found ===
[ERROR] /Game/Characters/character texture: Asset name contains forbidden characters: (space)
  Suggestion: Remove special characters and spaces from asset name

[WARNING] /Game/Materials/CharacterMaterial: Asset does not follow naming convention. Expected prefix: M_
  Suggestion: Rename to: M_CharacterMaterial

[INFO] /Game/Blueprints/BP_Door: Asset is not in a recommended folder for its type (Blueprint)
  Suggestion: Consider moving to: Blueprints
```

## API Reference

### UAssetValidatorSubsystem

Main subsystem for asset validation.

**Methods:**
- `FValidationResult ValidateAssets(FString Path, bool bNamingOnly, bool bIncludeSubfolders)` - Validate multiple assets
- `FValidationResult ValidateSingleAsset(FString AssetPath)` - Validate single asset
- `FAssetStatistics GetAssetStatistics(FString Path, bool bIncludeSubfolders)` - Get asset statistics
- `void PrintValidationResults(FValidationResult Result)` - Print results to log
- `bool ExportValidationResults(FValidationResult Result, FString FilePath)` - Export results to file

### UAssetNamingValidator

Validates asset naming conventions.

**Methods:**
- `TArray<FValidationIssue> ValidateAssetNaming(FString AssetPath, FString AssetClass)` - Validate asset name
- `bool ContainsForbiddenCharacters(FString AssetName)` - Check for forbidden characters
- `FString GetExpectedPrefix(FString AssetClass)` - Get expected prefix for asset type
- `void AddNamingRule(FNamingRule Rule)` - Add custom naming rule

### UAssetFolderValidator

Validates folder structure.

**Methods:**
- `TArray<FValidationIssue> ValidateFolderStructure(FString AssetPath, FString AssetClass)` - Validate folder placement
- `bool IsInRecommendedFolder(FString AssetPath, FString AssetClass)` - Check if in recommended folder
- `TArray<FString> GetRecommendedFolders(FString AssetClass)` - Get recommended folders for asset type
- `void AddFolderRule(FFolderRule Rule)` - Add custom folder rule

## Future Enhancements (Planned)

### Phase 2: Detailed Validation
- Asset reference integrity checks
- Circular reference detection
- Texture validation (resolution, compression)
- Material validation (parameter naming, shader complexity)

### Phase 3: Advanced Features
- Performance impact analysis
- Automatic fix suggestions
- Custom rule scripting
- Integration with version control

## License

Copyright Epic Games, Inc. All Rights Reserved.

## Support

For issues and feature requests, please contact the development team.
