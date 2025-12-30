# Claude Code Skills for AntigravityTest

This directory contains Claude Code skills for the AntigravityTest Unreal Engine 5 project.

## Available Skills

### generate-quest

Generates Quest DataAssets from YAML/JSON quest definitions.

See: [skills/generate-quest.md](skills/generate-quest.md)

**Quick Usage:**
```bash
# Generate from YAML
claude generate-quest --from Content/Quests/Examples/intro_quest.yaml

# Generate from JSON
claude generate-quest --from Content/Quests/Examples/boss_quest.json

# Custom output path
claude generate-quest --from my_quest.yaml --output /Game/MyQuests/
```

## Adding New Skills

To add a new skill:

1. Create a markdown file in `skills/` directory (e.g., `skills/my-skill.md`)
2. Define the skill's purpose, usage, and implementation
3. Add any supporting Python scripts to the appropriate plugin's `Content/Python/` directory
4. Update this README with the new skill information

## Skill Development Guidelines

- Skills should be focused on specific tasks
- Provide clear documentation and examples
- Include validation and error handling
- Follow the project's coding standards (see CLAUDE.md)
- Test skills before committing

## Project Integration

Skills in this directory integrate with the following plugins:

- **QuestSystem**: Quest generation and management (`generate-quest`)
- **MermaidMaterial**: Material graph generation from Mermaid diagrams
- **ScreenTransitionSystem**: Screen transition management

## Requirements

Most skills require:
- Unreal Engine 5.7
- PythonScriptPlugin enabled
- EditorScriptingUtilities enabled
- Appropriate plugins enabled

See individual skill documentation for specific requirements.
