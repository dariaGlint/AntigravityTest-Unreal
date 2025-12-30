# Localization Helper Plugin

Agent skill for localization workflow automation in Unreal Engine 5.

## Overview

The Localization Helper plugin provides comprehensive tools for managing localization workflows, including text extraction, StringTable management, translation workflows, and quality validation.

## Features

### Phase 1: Text Extraction

- **Blueprint Text Extraction**: Extract text from UI Widgets, dialogs, and tooltips
- **C++ Source Extraction**: Detect `NSLOCTEXT`/`LOCTEXT` and identify hardcoded strings
- **Data Asset Extraction**: Extract text from Quest systems, item descriptions, and character names

### Phase 2: StringTable Management

- **Automatic StringTable Generation**: Create StringTables from extracted texts
- **Key Naming Conventions**: Apply consistent key generation patterns
- **Duplicate Detection**: Find and merge duplicate entries
- **Unused Entry Detection**: Identify and clean up dead code

### Phase 3: Translation Workflow

- **CSV/PO Export**: Export to translator-friendly formats with context
- **Translation Import**: Import and validate translations
- **Machine Translation**: Initial translation generation (integration ready)
- **Review Workflow**: Mark entries for review

### Phase 4: Validation and QA

- **Completeness Check**: Detect untranslated texts and track progress
- **Text Length Validation**: Warn about translations that may not fit UI
- **Placeholder Verification**: Ensure format parameters match ({0}, {1}, etc.)

## Installation

1. Copy the `LocalizationHelper` folder to your project's `Plugins` directory
2. Restart Unreal Editor
3. Enable the plugin in Edit → Plugins → Localization → Localization Helper
4. Restart the editor when prompted

## Usage

### C++ API

#### Text Extraction

```cpp
#include "LocalizationTextExtractor.h"

// Extract from C++ source
TArray<FExtractedTextEntry> Entries = ULocalizationTextExtractor::ExtractFromCppSource("Source/MyGame/MyFile.cpp");

// Export to CSV
ULocalizationTextExtractor::ExportToCSV(Entries, "Output/extracted_texts.csv");
```

#### StringTable Management

```cpp
#include "LocalizationStringTableManager.h"

// Generate a key
FString Key = ULocalizationStringTableManager::GenerateKey("UI", "Combat", "Health");
// Result: "UI.Combat.Health"

// Find duplicates
TArray<FString> StringTables = {"/Game/Localization/ST_UI", "/Game/Localization/ST_Combat"};
TArray<FString> Duplicates = ULocalizationStringTableManager::FindDuplicateEntries(StringTables);
```

#### Translation Validation

```cpp
#include "LocalizationTranslationManager.h"

// Validate placeholders
bool bValid = ULocalizationTranslationManager::ValidatePlaceholders(
    "Health: {0}/{1}",
    "体力: {0}/{1}"
);

// Check translation progress
float Progress = ULocalizationTranslationManager::GetTranslationProgress(
    "/Game/Localization/ST_UI",
    "ja"
);
```

### Python API

#### Text Extraction

```python
import localization_automation

# Extract all texts from project
localization_automation.extract_texts(output_csv='texts.csv')
```

#### StringTable Creation

```python
# Create StringTable from CSV
localization_automation.create_stringtable(
    csv_path='texts.csv',
    name='ST_UI_Combat'
)
```

#### Translation Workflow

```python
# Generate machine translations
localization_automation.translate(
    source_csv='texts.csv',
    target_lang='ja'
)

# Validate translations
localization_automation.check_localization(
    string_table_path='/Game/Localization/ST_UI',
    language='ja'
)
```

#### Find Unused Entries

```python
# Find unused StringTable entries
localization_automation.find_unused_strings(
    string_table_path='/Game/Localization/ST_UI'
)
```

## CSV Format

### Extraction CSV

```csv
Key,SourceString,Context,Location
UI.Combat.Health,"Health: {0}",Player HUD,BP_CombatCharacter:142
Quest.Q001.Title,"Welcome to the World",Quest title,QuestData/Q001
Dialog.NPC01.Greeting,"Hello, traveler!",NPC dialogue,BP_SideScrollingNPC:56
```

### Translation CSV

```csv
Key,SourceText,TranslatedText,Context,SourceLanguage,TargetLanguage
UI.Combat.Health,"Health: {0}","体力: {0}",Player HUD,en,ja
Quest.Q001.Title,"Welcome to the World","世界へようこそ",Quest title,en,ja
```

## StringTable Structure

Organize StringTables by category:

```
ST_UI_Combat
├── UI.Combat.Health
├── UI.Combat.Stamina
└── UI.Combat.Score

ST_Quests
├── Quest.Q001.Title
├── Quest.Q001.Description
└── Quest.Q001.Objective1

ST_Dialogs
├── Dialog.NPC01.Greeting
├── Dialog.NPC01.Quest
└── Dialog.NPC02.Shop
```

## Key Naming Conventions

Use the pattern: `Category.Subcategory.Identifier`

**Examples:**
- `UI.Combat.Health` - Combat UI health display
- `Quest.Q001.Title` - Quest #001 title
- `Dialog.NPC01.Greeting` - NPC #01 greeting dialog
- `Item.Weapon.Sword.Name` - Sword weapon name
- `Tutorial.Step01.Description` - Tutorial step 1 description

## Supported Languages

- Japanese (ja)
- English (en)
- Chinese Simplified (zh-Hans)
- Chinese Traditional (zh-Hant)
- Korean (ko)
- French (fr)
- Spanish (es)
- German (de)

## Best Practices

1. **Avoid Hardcoded Text**: Always use StringTables or `LOCTEXT`
2. **Consistent Key Naming**: Follow the `Category.Subcategory.Identifier` pattern
3. **Add Context**: Provide context information for translators
4. **Validate Early**: Run validation checks before sending to translators
5. **Review Machine Translations**: Always review auto-generated translations
6. **Test UI Fit**: Verify translated text fits in UI elements

## Integration with Existing Systems

### QuestSystem Plugin

The Localization Helper can automatically extract quest texts:

```cpp
// Quest texts will be extracted with keys like:
// Quest.Q001.Title
// Quest.Q001.Description
// Quest.Q001.Objective1
```

### UI Blueprints

Widget text can be extracted and managed:

```cpp
// UI texts will be extracted with keys like:
// UI.Combat.HealthBar
// UI.Menu.StartButton
// UI.Settings.AudioVolume
```

## Workflow Example

### 1. Extract Texts

```python
# Extract all texts from project
extract_texts('extracted_texts.csv')
```

### 2. Create StringTable

```python
# Create StringTable from extracted texts
create_stringtable('extracted_texts.csv', 'ST_UI_Combat')
```

### 3. Export for Translation

```cpp
// Export to CSV for translators
ULocalizationTranslationManager::ExportForTranslation(
    "/Game/Localization/ST_UI_Combat",
    "en",
    "Output/for_translation.csv"
);
```

### 4. Import Translations

```cpp
// After receiving translations, import them
ULocalizationTranslationManager::ImportTranslations(
    "Input/translated_ja.csv",
    "/Game/Localization/ST_UI_Combat",
    "ja"
);
```

### 5. Validate

```python
# Check translation quality and completeness
check_localization('/Game/Localization/ST_UI_Combat', 'ja')
```

## Troubleshooting

### Plugin Not Loading

- Ensure the plugin is enabled in Edit → Plugins
- Check that all module dependencies are available
- Rebuild the project if necessary

### Python Scripts Not Working

- Verify Python plugin is enabled in Unreal
- Check Python script paths in Content/Python/
- Review Output Log for Python errors

### StringTable Not Created

- Ensure output path is valid
- Check CSV format is correct
- Verify write permissions

## API Reference

### ULocalizationTextExtractor

- `ExtractFromBlueprint()` - Extract text from Blueprint assets
- `ExtractFromCppSource()` - Extract text from C++ files
- `ExtractFromDataAsset()` - Extract text from data assets
- `ExtractAllTexts()` - Extract all project texts
- `ExportToCSV()` - Export entries to CSV

### ULocalizationStringTableManager

- `CreateStringTableFromCSV()` - Create StringTable from CSV
- `AddEntriesToStringTable()` - Add entries to existing StringTable
- `FindDuplicateEntries()` - Find duplicate keys
- `FindUnusedEntries()` - Find unused keys
- `GenerateKey()` - Generate consistent key
- `ValidateKeyNamingConventions()` - Validate key format

### ULocalizationTranslationManager

- `ExportForTranslation()` - Export for translators
- `ImportTranslations()` - Import translated CSV
- `FindMissingTranslations()` - Find untranslated entries
- `GetTranslationProgress()` - Calculate progress percentage
- `ValidatePlaceholders()` - Validate format parameters
- `ValidateTextLength()` - Check text length

## Contributing

This plugin is part of the AntigravityTest project. Contributions are welcome!

## License

See project license for details.

## Version History

### 1.0.0 (Initial Release)
- Text extraction framework
- StringTable management utilities
- Translation workflow tools
- Validation and QA features
- Python automation support
