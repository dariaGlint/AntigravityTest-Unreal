# UE Plugin Scaffolder

Generate a new Unreal Engine plugin with proper structure and best practices.

## Instructions

1. **Gather Plugin Information**
   - Ask the user for the plugin name (e.g., "MyCustomPlugin")
   - Ask for the plugin type:
     - **Runtime** - Runtime-only plugin with C++ module
     - **Editor** - Editor-only plugin
     - **Runtime+Editor** - Plugin with both Runtime and Editor modules
     - **Content-Only** - Plugin without C++ code (Python/Blueprint/Assets only)
     - **BlueprintLibrary** - Runtime plugin with Blueprint function library
     - **ActorComponent** - Runtime plugin with custom ActorComponent
   - Ask for a friendly name (default: same as plugin name)
   - Ask for a brief description
   - Ask for the category (e.g., "Gameplay", "UI", "Development", "Tools")

2. **Validate Plugin Name**
   - Check if plugin name follows PascalCase convention
   - Verify the plugin doesn't already exist in `Plugins/` directory
   - Ensure the name doesn't conflict with engine modules

3. **Create Directory Structure**
   Based on plugin type, create the following structure:

   **For Runtime/Editor/Runtime+Editor plugins:**
   ```
   Plugins/
   └── {PluginName}/
       ├── {PluginName}.uplugin
       ├── Resources/
       │   └── Icon128.png (placeholder or copy from existing)
       ├── Source/
       │   └── {PluginName}/
       │       ├── {PluginName}.Build.cs
       │       ├── Public/
       │       │   └── {PluginName}.h
       │       └── Private/
       │           └── {PluginName}.cpp
       ├── Content/
       └── README.md
   ```

   **For Content-Only plugins:**
   ```
   Plugins/
   └── {PluginName}/
       ├── {PluginName}.uplugin
       ├── Resources/
       │   └── Icon128.png
       ├── Content/
       │   └── Python/  (if Python plugin)
       ├── Documentation/
       └── README.md
   ```

4. **Generate .uplugin File**
   Create the plugin descriptor with proper metadata:
   ```json
   {
       "FileVersion": 3,
       "Version": 1,
       "VersionName": "1.0.0",
       "FriendlyName": "{FriendlyName}",
       "Description": "{Description}",
       "Category": "{Category}",
       "CreatedBy": "{Author}",
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
               "Name": "{PluginName}",
               "Type": "Runtime",
               "LoadingPhase": "Default"
           }
       ]
   }
   ```

5. **Generate Build.cs File** (for C++ plugins)
   Create a Build.cs file with common dependencies:
   ```csharp
   using UnrealBuildTool;

   public class {PluginName} : ModuleRules
   {
       public {PluginName}(ReadOnlyTargetRules Target) : base(Target)
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

6. **Generate Module Header File** (for C++ plugins)
   Create the module interface header:
   ```cpp
   #pragma once

   #include "CoreMinimal.h"
   #include "Modules/ModuleManager.h"

   class F{PluginName}Module : public IModuleInterface
   {
   public:
       virtual void StartupModule() override;
       virtual void ShutdownModule() override;
   };
   ```

7. **Generate Module Implementation File** (for C++ plugins)
   Create the module implementation:
   ```cpp
   #include "{PluginName}.h"

   #define LOCTEXT_NAMESPACE "F{PluginName}Module"

   void F{PluginName}Module::StartupModule()
   {
       // Startup logic here
   }

   void F{PluginName}Module::ShutdownModule()
   {
       // Shutdown logic here
   }

   #undef LOCTEXT_NAMESPACE

   IMPLEMENT_MODULE(F{PluginName}Module, {PluginName})
   ```

8. **Generate Type-Specific Sample Classes**

   **For BlueprintLibrary:**
   - Create `{PluginName}Library.h` and `.cpp`
   - Include `UFUNCTION(BlueprintCallable)` examples

   **For ActorComponent:**
   - Create `{PluginName}Component.h` and `.cpp`
   - Derive from `UActorComponent`
   - Include `TickComponent()` and basic lifecycle methods

9. **Generate README.md**
   Create a comprehensive README with:
   - Plugin name and description
   - Features section
   - Installation instructions
   - Usage examples
   - API documentation (if applicable)
   - License information
   - Contributing guidelines

10. **Add Plugin to .uproject (Optional)**
    - Ask the user if they want to add the plugin reference to `AntigravityTest.uproject`
    - If yes, add the plugin to the "Plugins" array in .uproject:
    ```json
    {
        "Name": "{PluginName}",
        "Enabled": true
    }
    ```

11. **Create Icon Placeholder**
    - Copy an existing icon from another plugin, or
    - Note that the user should replace `Resources/Icon128.png` with their own 128x128 icon

12. **Summary and Next Steps**
    Provide a summary showing:
    - Created directory structure
    - Generated files list
    - Next steps for the user:
      - Build the project to compile the plugin
      - Enable the plugin in the Unreal Editor
      - Add custom functionality to the generated classes
      - Replace the icon placeholder
      - Update README.md with specific documentation

## Plugin Type Details

### Runtime Plugin
- **Modules**: Single Runtime module
- **Use Case**: Game runtime functionality
- **Example**: ScreenTransitionSystem
- **Dependencies**: Core, CoreUObject, Engine

### Editor Plugin
- **Modules**: Single Editor module
- **LoadingPhase**: PostEngineInit or Default
- **Use Case**: Editor tools, asset validators, custom editors
- **Dependencies**: Core, CoreUObject, Engine, UnrealEd

### Runtime+Editor Plugin
- **Modules**: Both Runtime and Editor modules
- **Use Case**: Functionality needed both in-game and in-editor
- **Example**: Plugin with gameplay features and custom editor tools
- **Structure**:
  ```
  Source/
  ├── {PluginName}/          # Runtime module
  └── {PluginName}Editor/    # Editor module
  ```

### Content-Only Plugin
- **Modules**: None
- **Use Case**: Blueprint libraries, Python scripts, asset collections
- **Example**: BlueprintAnalyzer (Python-based)
- **Can Contain**: Blueprints, Python scripts, Materials, etc.

### BlueprintLibrary Plugin
- **Modules**: Runtime module with BlueprintFunctionLibrary
- **Use Case**: Expose C++ functions to Blueprints
- **Sample Class**: Static function library with `UFUNCTION(BlueprintCallable)`
- **Dependencies**: Core, CoreUObject, Engine

### ActorComponent Plugin
- **Modules**: Runtime module with custom UActorComponent
- **Use Case**: Reusable actor components
- **Sample Class**: Custom component with Tick and lifecycle methods
- **Dependencies**: Core, CoreUObject, Engine

## Important Notes

### Naming Conventions
- **Plugin Name**: PascalCase (e.g., `InventorySystem`, `DialogueManager`)
- **Module Name**: Same as plugin name
- **Class Names**: Follow UE naming conventions
  - `F` prefix for module classes (e.g., `FInventorySystemModule`)
  - `U` prefix for UObject-derived classes (e.g., `UInventoryComponent`)
  - `A` prefix for Actor-derived classes (e.g., `AInventoryActor`)

### Module Dependencies
Common module dependencies to consider:
- **Core**: Always required
- **CoreUObject**: Always required for UObject support
- **Engine**: Required for most gameplay features
- **UMG**: For UI widgets
- **Slate/SlateCore**: For low-level UI
- **AIModule**: For AI features
- **InputCore**: For input handling
- **UnrealEd**: For editor-only functionality

### Best Practices
1. Follow the existing plugin structure in this project
2. Use `PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs` in Build.cs
3. Include `#pragma once` in all header files
4. Use `LOCTEXT_NAMESPACE` for localization support
5. Keep Public headers minimal (only public API)
6. Put implementation details in Private folder
7. Add comprehensive README documentation

### Project Integration
After creating the plugin:
1. The user should regenerate project files (right-click .uproject > Generate Visual Studio project files)
2. Build the project in Visual Studio or using the Build.bat script
3. Enable the plugin in Unreal Editor (Edit > Plugins)
4. Restart the editor if prompted

## Example Usage

**User Request**: "Create a new plugin called InventorySystem for managing player inventory"

**Expected Workflow**:
1. Plugin name: InventorySystem
2. Type: Runtime+Editor (to include editor tools)
3. Friendly name: "Inventory System"
4. Description: "A flexible inventory management system with UI support"
5. Category: "Gameplay"

**Result**: Complete plugin structure with:
- InventorySystem.uplugin
- Build.cs for both Runtime and Editor modules
- Module interface files
- Sample InventoryComponent class
- Comprehensive README.md

## Related Tools
- **link-plugin**: Use this to link the new plugin to other UE5 projects
- **analyze-blueprints**: Can analyze Blueprint assets in the plugin's Content folder

## Technical Implementation

### File Generation Priority
1. Directory structure first
2. .uplugin file (critical - plugin won't load without it)
3. Build.cs (if C++ plugin)
4. Module files (.h and .cpp)
5. Sample classes (optional but recommended)
6. README.md (documentation)
7. Icon placeholder

### Error Handling
- If plugin already exists, ask user if they want to:
  - Overwrite (dangerous)
  - Create with a different name
  - Cancel operation
- If .uproject update fails, inform user to manually add plugin reference
- If icon copy fails, create placeholder note in README

### Validation Checks
- Plugin name doesn't contain spaces or special characters
- Plugin name doesn't conflict with engine modules (Core, Engine, etc.)
- Target directory is writable
- .uplugin JSON is valid

## Integration with CLAUDE.md
The generated plugins should follow the conventions documented in CLAUDE.md:
- Proper C++ naming conventions (F, U, A prefixes)
- UPROPERTY/UFUNCTION macros for Blueprint exposure
- Module dependency management
- Documentation standards
