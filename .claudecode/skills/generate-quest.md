# Generate Quest Skill

Generate Unreal Engine quest assets from YAML/JSON definitions.

## Description

This skill automates the creation of Quest DataAssets in Unreal Engine 5 from structured YAML or JSON files. It creates:
- Quest DataAssets
- Objective DataAssets
- Configured rewards and prerequisites

## Usage

### From YAML file:
```bash
claude generate-quest --from quest_definition.yaml
```

### From JSON file:
```bash
claude generate-quest --from quest_definition.json
```

### Interactive mode:
```bash
claude generate-quest --interactive
```

### With custom output path:
```bash
claude generate-quest --from quest.yaml --output /Game/MyQuests/
```

## Quest Definition Format

### YAML Example:

```yaml
quest:
  id: Q001_IntroQuest
  title: "Welcome to the World"
  description: "Complete your first combat challenge"

  objectives:
    - type: DefeatEnemies
      target: BP_CombatEnemy
      count: 5
      description: "Defeat 5 enemies"

    - type: ReachLocation
      location: Checkpoint_01
      description: "Reach the checkpoint"

  rewards:
    - type: Experience
      amount: 100
    - type: Item
      item: Sword_Basic
      count: 1

  prerequisites:
    - CompletedTutorial

  repeatable: false
```

### JSON Example:

```json
{
  "quest": {
    "id": "Q001_IntroQuest",
    "title": "Welcome to the World",
    "description": "Complete your first combat challenge",
    "objectives": [
      {
        "type": "DefeatEnemies",
        "target": "BP_CombatEnemy",
        "count": 5,
        "description": "Defeat 5 enemies"
      },
      {
        "type": "ReachLocation",
        "location": "Checkpoint_01",
        "description": "Reach the checkpoint"
      }
    ],
    "rewards": [
      {
        "type": "Experience",
        "amount": 100
      },
      {
        "type": "Item",
        "item": "Sword_Basic",
        "count": 1
      }
    ],
    "prerequisites": ["CompletedTutorial"],
    "repeatable": false
  }
}
```

## Objective Types

- `DefeatEnemies` - Kill a specific number of enemies
- `ReachLocation` - Reach a specific location
- `CollectItems` - Collect a specific number of items
- `InteractWithObject` - Interact with an object
- `Custom` - Custom objective type

## Reward Types

- `Experience` - Grant experience points
- `Currency` - Grant in-game currency
- `Item` - Grant an item
- `Custom` - Custom reward type

## Implementation

The skill uses the QuestSystem plugin's Python API:

```python
from quest_generator import generate_quest_from_file

# Generate quest
quest_asset = generate_quest_from_file(
    'path/to/quest.yaml',
    '/Game/Quests/'
)
```

## Requirements

- QuestSystem plugin must be enabled
- Python Script Plugin must be enabled
- Editor must be running (assets are created in the editor)

## Output

Creates the following assets:
- Main Quest DataAsset: `/Game/Quests/DA_{QuestID}`
- Objective DataAssets: `/Game/Quests/Objectives/DA_{QuestID}_Obj{NN}`

## Validation

The generator validates:
- Required fields (id, title, description)
- At least one objective
- Valid objective types
- Valid reward types
