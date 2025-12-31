# UE Plugin Scaffolder - AgentSkill Documentation

## Overview
The UE Plugin Scaffolder is an AgentSkill that automates the creation of Unreal Engine plugin structures with best practices and proper conventions.

## Purpose
- **Efficiency**: Quickly scaffold new plugins without manual file creation
- **Consistency**: Ensure all plugins follow the same structure and conventions
- **Best Practices**: Generate code that adheres to UE5 and project-specific standards
- **Reduced Errors**: Avoid common mistakes in plugin setup

## Supported Plugin Types

### 1. Runtime Plugin
A plugin that runs during gameplay.

**Use Cases**:
- Gameplay systems (inventory, dialogue, quest)
- Custom components
- Game mechanics

**Example**: ScreenTransitionSystem

**Generated Structure**:
```
Plugins/{PluginName}/
├── {PluginName}.uplugin
├── Resources/Icon128.png
├── Source/{PluginName}/
│   ├── {PluginName}.Build.cs
│   ├── Public/{PluginName}.h
│   └── Private/{PluginName}.cpp
├── Content/
└── README.md
```

### 2. Editor Plugin
A plugin that only runs in the Unreal Editor.

**Use Cases**:
- Asset validators
- Custom editor tools
- Content browser extensions
- Custom asset types

**Dependencies**: Adds UnrealEd, EditorSubsystem modules

### 3. Runtime+Editor Plugin
A plugin with both runtime and editor functionality.

**Use Cases**:
- Systems needing both gameplay and editor tools
- Custom asset types with gameplay functionality
- Debug tools visible in both editor and game

**Generated Structure**:
```
Source/
├── {PluginName}/          # Runtime module
└── {PluginName}Editor/    # Editor module
```

### 4. Content-Only Plugin
A plugin without C++ code.

**Use Cases**:
- Blueprint libraries
- Python script collections
- Asset packs
- Material libraries

**Example**: BlueprintAnalyzer (Python-based)

**Generated Structure**:
```
Plugins/{PluginName}/
├── {PluginName}.uplugin (with empty Modules array)
├── Resources/Icon128.png
├── Content/
├── Documentation/
└── README.md
```

### 5. BlueprintLibrary Plugin
A runtime plugin with Blueprint function library.

**Use Cases**:
- Math utilities
- String helpers
- Conversion functions
- Blueprint-accessible system APIs

**Sample Generated Class**:
```cpp
UCLASS()
class U{PluginName}Library : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "{PluginName}")
    static void SampleFunction();
};
```

### 6. ActorComponent Plugin
A runtime plugin with custom ActorComponent.

**Use Cases**:
- Reusable actor behavior
- Modular gameplay features
- Component-based systems

**Sample Generated Class**:
```cpp
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class U{PluginName}Component : public UActorComponent
{
    GENERATED_BODY()

public:
    U{PluginName}Component();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                              FActorComponentTickFunction* ThisTickFunction) override;
};
```

## Usage Examples

### Example 1: Create an Inventory System
```
User: "Create a new plugin for inventory management"

Skill prompts:
→ Plugin name: InventorySystem
→ Type: Runtime+Editor
→ Friendly name: "Inventory System"
→ Description: "Flexible inventory management with UI support"
→ Category: Gameplay

Result:
✓ Created Plugins/InventorySystem/
✓ Generated .uplugin, Build.cs, module files
✓ Created both Runtime and Editor modules
✓ Added README.md with usage instructions
```

### Example 2: Create a Blueprint Function Library
```
User: "I need a plugin with math utility functions for Blueprints"

Skill prompts:
→ Plugin name: MathUtilities
→ Type: BlueprintLibrary
→ Friendly name: "Math Utilities"
→ Description: "Extended math functions for Blueprints"
→ Category: Utilities

Result:
✓ Created Plugins/MathUtilities/
✓ Generated BlueprintFunctionLibrary class
✓ Included sample UFUNCTION(BlueprintCallable) methods
✓ Added documentation for extending the library
```

### Example 3: Create a Content-Only Plugin
```
User: "Create a plugin for Python automation scripts"

Skill prompts:
→ Plugin name: AutomationScripts
→ Type: Content-Only
→ Friendly name: "Automation Scripts"
→ Description: "Python scripts for editor automation"
→ Category: Development

Result:
✓ Created Plugins/AutomationScripts/
✓ Generated .uplugin with PythonScriptPlugin dependency
✓ Created Content/Python/ directory
✓ No C++ module files (content-only)
```

## Generated File Templates

### .uplugin Template
```json
{
    "FileVersion": 3,
    "Version": 1,
    "VersionName": "1.0.0",
    "FriendlyName": "Your Plugin Name",
    "Description": "Plugin description",
    "Category": "Gameplay",
    "CreatedBy": "Your Name",
    "CreatedByURL": "",
    "DocsURL": "",
    "MarketplaceURL": "",
    "SupportURL": "",
    "CanContainContent": true,
    "IsBetaVersion": false,
    "IsExperimentalVersion": false,
    "Installed": false,
    "Modules": [
        {
            "Name": "YourPluginName",
            "Type": "Runtime",
            "LoadingPhase": "Default"
        }
    ]
}
```

### Build.cs Template
```csharp
using UnrealBuildTool;

public class YourPluginName : ModuleRules
{
    public YourPluginName(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}
```

### Module Header Template
```cpp
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FYourPluginNameModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
```

### Module Implementation Template
```cpp
#include "YourPluginName.h"

#define LOCTEXT_NAMESPACE "FYourPluginNameModule"

void FYourPluginNameModule::StartupModule()
{
    // Startup logic here
}

void FYourPluginNameModule::ShutdownModule()
{
    // Shutdown logic here
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FYourPluginNameModule, YourPluginName)
```

## Integration with Project

### Adding to .uproject
The skill can optionally add the plugin reference to `AntigravityTest.uproject`:

```json
{
    "Plugins": [
        {
            "Name": "YourPluginName",
            "Enabled": true
        }
    ]
}
```

### Building the Plugin
After generation, the user should:

1. **Regenerate Project Files**:
   - Windows: Right-click `AntigravityTest.uproject` → "Generate Visual Studio project files"
   - Linux: Run `./GenerateProjectFiles.sh`

2. **Build the Project**:
   - Use `Build.bat` (Windows) or build script
   - Or build from Visual Studio/Rider

3. **Enable in Editor**:
   - Open Unreal Editor
   - Go to Edit → Plugins
   - Find your plugin and enable it
   - Restart editor if prompted

## Best Practices Enforced

### Naming Conventions
- ✓ Plugin names use PascalCase
- ✓ Module classes use `F` prefix (e.g., `FInventorySystemModule`)
- ✓ UObject classes use `U` prefix (e.g., `UInventoryComponent`)
- ✓ Actor classes use `A` prefix (e.g., `AInventoryActor`)

### Module Structure
- ✓ Public headers in `Public/` folder
- ✓ Implementation in `Private/` folder
- ✓ PCH usage enabled for faster compilation
- ✓ Proper dependency management

### Documentation
- ✓ Comprehensive README.md generated
- ✓ Usage examples included
- ✓ API documentation placeholders
- ✓ Next steps for developers

### UE5 Compatibility
- ✓ FileVersion 3 in .uplugin
- ✓ Compatible with UE 5.7
- ✓ Follows Epic's coding standards
- ✓ Uses modern module loading

## Advanced Features

### Custom Dependencies
For plugins needing specific modules, the skill can add them to Build.cs:

**Common Dependencies**:
- `UMG` - UI widgets
- `Slate`, `SlateCore` - Low-level UI
- `AIModule` - AI features
- `InputCore` - Input handling
- `UnrealEd` - Editor functionality
- `BlueprintGraph` - Blueprint nodes
- `AssetRegistry` - Asset queries

### Multi-Module Plugins
For Runtime+Editor plugins, the skill generates two modules:

```
Source/
├── PluginName/           # Runtime module
│   ├── PluginName.Build.cs
│   ├── Public/
│   └── Private/
└── PluginNameEditor/     # Editor module
    ├── PluginNameEditor.Build.cs
    ├── Public/
    └── Private/
```

### Sample Classes
Based on plugin type, generates:
- **BlueprintLibrary**: Static function library
- **ActorComponent**: Component with Tick
- **Actor**: Custom actor class
- **Object**: Custom UObject class

## Troubleshooting

### Plugin Not Recognized
**Problem**: Plugin doesn't appear in editor

**Solutions**:
1. Verify .uplugin file is valid JSON
2. Regenerate project files
3. Check plugin is in `Plugins/` directory
4. Ensure module name matches plugin name

### Compilation Errors
**Problem**: Plugin fails to compile

**Solutions**:
1. Check Build.cs has required dependencies
2. Verify all includes use `#pragma once`
3. Ensure module is properly registered with `IMPLEMENT_MODULE`
4. Check for typos in class names

### Blueprint Functions Not Visible
**Problem**: UFUNCTION not appearing in Blueprints

**Solutions**:
1. Verify `BlueprintCallable` is set
2. Check function is public or protected
3. Ensure plugin is enabled and compiled
4. Restart Unreal Editor

## Comparison with Manual Creation

| Aspect | Manual | UE Plugin Scaffolder |
|--------|--------|---------------------|
| Time | 15-30 minutes | 1-2 minutes |
| Errors | Common (typos, structure) | Rare (validated) |
| Consistency | Varies | Guaranteed |
| Best Practices | May be missed | Always applied |
| Documentation | Often skipped | Always generated |

## Related AgentSkills

### link-plugin
After creating a plugin, use `link-plugin` to:
- Create symbolic link to other UE5 projects
- Enable cross-project development
- Test plugin in multiple projects

**Example Workflow**:
```
1. Use ue-plugin-scaffolder to create plugin
2. Develop plugin features
3. Use link-plugin to test in other projects
```

### analyze-blueprints
Use `analyze-blueprints` to:
- Check Blueprint assets in plugin's Content folder
- Verify naming conventions
- Analyze performance

## Future Enhancements

Potential improvements for this skill:
- [ ] Support for Blueprint-only plugins with generated Blueprint classes
- [ ] Custom class templates (e.g., GameMode, PlayerController)
- [ ] Plugin icon generation with default graphics
- [ ] Automated testing setup (Google Test integration)
- [ ] CI/CD configuration files
- [ ] Sample Blueprint assets
- [ ] Documentation website scaffolding

## Technical Notes

### File Generation Order
1. Create directory structure
2. Generate .uplugin (critical file)
3. Generate Build.cs (if C++ plugin)
4. Generate module interface files
5. Generate sample classes
6. Generate README.md
7. Copy/create icon placeholder

### Validation Checks
- ✓ Plugin name is PascalCase
- ✓ No spaces or special characters
- ✓ Doesn't conflict with engine modules
- ✓ Directory is writable
- ✓ .uplugin JSON is valid

### Error Recovery
- If plugin exists: Prompt for rename/overwrite/cancel
- If .uproject update fails: Manual instruction provided
- If icon copy fails: Placeholder note in README

## Contributing

To improve this skill:
1. Test with different plugin types
2. Suggest additional templates
3. Report issues or edge cases
4. Propose new plugin type support

## License
This AgentSkill is part of the AntigravityTest project and follows the same license.

## Support
For issues or questions:
- Check CLAUDE.md for project conventions
- Review existing plugins for examples
- Refer to UE5 plugin documentation

## Version History
- **1.0.0** (Initial) - Basic plugin scaffolding with 6 plugin types
