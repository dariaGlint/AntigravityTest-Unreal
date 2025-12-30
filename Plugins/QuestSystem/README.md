# Quest System Plugin

A comprehensive quest system for Unreal Engine 5 with automated quest generation from YAML/JSON definitions.

## Features

- **Data-Driven Quest Design**: Define quests in YAML or JSON format
- **Automatic Asset Generation**: Generate Quest and Objective DataAssets automatically
- **Flexible Quest Types**: Support for various objective types (defeat enemies, collect items, reach locations, etc.)
- **Reward System**: Configure multiple reward types (XP, currency, items)
- **Quest Dependencies**: Define prerequisites and quest chains
- **Repeatable Quests**: Mark quests as repeatable or one-time
- **Claude Code Integration**: Use the `generate-quest` skill for easy quest creation

## Installation

1. Copy the `QuestSystem` plugin to your project's `Plugins` directory
2. Enable the plugin in the Unreal Editor (.uproject file)
3. Ensure `PythonScriptPlugin` and `EditorScriptingUtilities` are enabled
4. Restart the editor

## Quick Start

### 1. Define a Quest

Create a YAML file (e.g., `my_quest.yaml`):

```yaml
quest:
  id: Q001_MyFirstQuest
  title: "My First Quest"
  description: "This is my first quest"

  objectives:
    - type: DefeatEnemies
      target: BP_Enemy
      count: 3
      description: "Defeat 3 enemies"

  rewards:
    - type: Experience
      amount: 100

  prerequisites: []
  repeatable: false
```

### 2. Generate Quest Assets

#### Using Claude Code Skill:

```bash
claude generate-quest --from my_quest.yaml
```

#### Using Python in UE Editor:

```python
import sys
sys.path.append('/Game/Plugins/QuestSystem/Content/Python')
from quest_generator import generate_quest_from_file

quest = generate_quest_from_file('path/to/my_quest.yaml', '/Game/Quests/')
```

### 3. Use Quest in Game

The generated DataAssets can be referenced in your Blueprints or C++ code:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
TObjectPtr<UQuestDataAsset> MyQuest;
```

## Quest Definition Format

### Required Fields

- `id`: Unique quest identifier (string)
- `title`: Quest title (string)
- `description`: Quest description (string)
- `objectives`: Array of quest objectives (at least one required)

### Optional Fields

- `rewards`: Array of quest rewards (default: empty)
- `prerequisites`: Array of prerequisite quest IDs (default: empty)
- `repeatable`: Whether the quest can be repeated (default: false)

### Objective Structure

Each objective requires:

- `type`: Objective type (see Objective Types below)
- `description`: Objective description

Optional fields:

- `target`: Target actor/item ID
- `count`: Required count (default: 1)
- `location`: Location ID (for ReachLocation type)
- `optional`: Whether objective is optional (default: false)

### Objective Types

| Type | Description | Required Fields |
|------|-------------|----------------|
| `DefeatEnemies` | Defeat a certain number of enemies | `target`, `count` |
| `ReachLocation` | Reach a specific location | `location` |
| `CollectItems` | Collect a certain number of items | `target`, `count` |
| `InteractWithObject` | Interact with an object | `target` |
| `Custom` | Custom objective type | - |

### Reward Structure

Each reward requires:

- `type`: Reward type (see Reward Types below)

Optional fields:

- `amount`: Reward amount (for Experience, Currency)
- `item`: Item ID (for Item type)
- `count`: Item count (default: 1)
- `description`: Reward description

### Reward Types

| Type | Description | Required Fields |
|------|-------------|----------------|
| `Experience` | Grant experience points | `amount` |
| `Currency` | Grant in-game currency | `amount` |
| `Item` | Grant an item | `item`, `count` |
| `Custom` | Custom reward type | - |

## Examples

See `Content/Quests/Examples/` for complete quest examples:

- `intro_quest.yaml` - Basic introduction quest
- `collect_quest.yaml` - Collection quest with optional objectives
- `boss_quest.json` - Boss fight quest with prerequisites

## C++ API

### Quest Data Asset

```cpp
UCLASS(BlueprintType)
class QUESTSYSTEM_API UQuestDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<TObjectPtr<UQuestObjectiveDataAsset>> Objectives;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FQuestReward> Rewards;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FString> Prerequisites;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bRepeatable;
};
```

### Quest Objective Data Asset

```cpp
UCLASS(BlueprintType)
class QUESTSYSTEM_API UQuestObjectiveDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString ObjectiveID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    EQuestObjectiveType ObjectiveType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString TargetID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RequiredCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bOptional;
};
```

## Python API

### QuestGenerator Class

```python
from quest_generator import QuestGenerator

generator = QuestGenerator()

# Generate from YAML
quest = generator.generate_from_yaml('path/to/quest.yaml', '/Game/Quests/')

# Generate from JSON
quest = generator.generate_from_json('path/to/quest.json', '/Game/Quests/')

# Generate from dictionary
quest_data = {
    'quest': {
        'id': 'Q001',
        'title': 'Test Quest',
        'description': 'A test quest',
        'objectives': [...]
    }
}
quest = generator.generate_from_dict(quest_data, '/Game/Quests/')
```

## Validation

The quest generator validates:

- ✓ All required fields are present
- ✓ At least one objective is defined
- ✓ Objective types are valid
- ✓ Reward types are valid
- ✓ Quest IDs are unique (warning if duplicate)

## Future Enhancements (Phase 2 & 3)

Planned features for future releases:

### Phase 2
- Blueprint auto-generation for custom quest logic
- Localization support with StringTable integration
- Quest chain visualization
- Branching quest support

### Phase 3
- Interactive quest editor UI
- Quest templates system
- Automated test case generation
- Integration with TutorialSystem plugin

## Troubleshooting

### Quest assets not appearing

1. Check that the output path exists in your Content Browser
2. Verify the plugin is enabled in your .uproject
3. Check the Output Log for errors

### Python import errors

1. Ensure PythonScriptPlugin is enabled
2. Check Python path includes the plugin's Python directory
3. Verify quest_generator.py is in the correct location

### Validation errors

Check the Output Log for detailed validation error messages. Common issues:
- Missing required fields (id, title, description)
- No objectives defined
- Invalid objective/reward types

## License

Copyright Epic Games, Inc. All Rights Reserved.

## Support

For issues and questions, please refer to the project documentation or create an issue in the project repository.
