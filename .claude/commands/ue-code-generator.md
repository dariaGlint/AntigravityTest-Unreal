# UE Code Generator

Generate Unreal Engine 5 C++ boilerplate code following UE coding standards and project conventions.

## Overview

This skill automates the creation of UE5 C++ classes with proper UPROPERTY/UFUNCTION macros, naming conventions, and boilerplate code. It analyzes existing project patterns to generate code that matches your project's style.

## Capabilities

### 1. Class Template Generation

Generate complete .h/.cpp files for:

- **Actor Classes**: `AActor`, `APawn`, `ACharacter`, `APlayerController`, `AHUD`
- **Components**: `UActorComponent`, `USceneComponent`, `UPrimitiveComponent`
- **Objects**: `UObject`, `UDataAsset`, `UBlueprintFunctionLibrary`
- **Interfaces**: `UInterface` with implementation helpers
- **Game Framework**: `AGameMode`, `AGameState`, `APlayerState`
- **Widgets**: `UUserWidget` (UMG)
- **AI**: `AAIController`, `UBehaviorTree`, `UEnvQuery`

### 2. UPROPERTY Macro Generation

Automatically generate properties with appropriate specifiers:

```cpp
// Edit in editor, Blueprint read-write
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
float MaxSpeed = 600.0f;

// Visible in editor, Blueprint read-only (common for components)
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
TObjectPtr<USpringArmComponent> CameraBoom;

// Edit in editor with constraints
UPROPERTY(EditAnywhere, Category="Health", meta=(ClampMin=0, ClampMax=100))
float MaxHealth = 100.0f;

// Replicated property (for multiplayer)
UPROPERTY(Replicated, BlueprintReadOnly, Category="State")
bool bIsAlive = true;
```

**Common Specifier Patterns**:
- `EditAnywhere` + `BlueprintReadWrite` - Tweakable values
- `VisibleAnywhere` + `BlueprintReadOnly` - Component references
- `EditDefaultsOnly` - Only editable on Blueprint defaults
- `Replicated` - Network-replicated properties

**Meta Tags**:
- `ClampMin`, `ClampMax` - Numeric range constraints
- `Units` - Display units (cm, s, deg)
- `AllowPrivateAccess` - Blueprint access to private members
- `ExposeOnSpawn` - Blueprint construction script parameter

### 3. UFUNCTION Macro Generation

Generate functions with appropriate specifiers:

```cpp
// Blueprint callable function
UFUNCTION(BlueprintCallable, Category="Gameplay")
void PerformAction();

// Blueprint pure function (no exec pins)
UFUNCTION(BlueprintPure, Category="Gameplay")
float GetHealthPercent() const;

// Blueprint implementable event (no C++ implementation)
UFUNCTION(BlueprintImplementableEvent, Category="Events")
void OnDamageReceived(float Damage);

// Blueprint native event (C++ default, Blueprint override)
UFUNCTION(BlueprintNativeEvent, Category="Events")
void OnInteract(AActor* Interactor);
virtual void OnInteract_Implementation(AActor* Interactor);

// Server RPC (multiplayer)
UFUNCTION(Server, Reliable, WithValidation)
void ServerPerformAction();
virtual void ServerPerformAction_Implementation();
virtual bool ServerPerformAction_Validate();
```

**Common Patterns**:
- `BlueprintCallable` - Call from Blueprint
- `BlueprintPure` - Pure function (no side effects)
- `BlueprintImplementableEvent` - Blueprint-only override
- `BlueprintNativeEvent` - C++ default with Blueprint override (_Implementation suffix required)
- `Server`, `Client`, `NetMulticast` - Network RPCs
- `Reliable`, `Unreliable` - Network reliability

### 4. Interface Implementation

Generate interface classes and implementation helpers:

```cpp
// Interface definition (MyInterface.h)
UINTERFACE(MinimalAPI, Blueprintable)
class UMyInterface : public UInterface
{
    GENERATED_BODY()
};

class IMyInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interface")
    void DoSomething();
    virtual void DoSomething_Implementation();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interface")
    bool CanDoSomething() const;
    virtual bool CanDoSomething_Implementation() const;
};

// Implementation in another class
class PROJECTNAME_API AMyActor : public AActor, public IMyInterface
{
    GENERATED_BODY()

public:
    // IMyInterface implementation
    virtual void DoSomething_Implementation() override;
    virtual bool CanDoSomething_Implementation() const override;
};
```

### 5. Template-Based Generation

The skill learns from your existing code:

1. **Analyze Project Patterns**: Scans existing headers for style
2. **Match Naming Conventions**: Follows your project's conventions
3. **Replicate Code Style**: Matches indentation, spacing, comments
4. **Reuse Common Patterns**: Identifies and reuses your project's patterns

## Usage Instructions

### Interactive Mode

When invoked, the skill will:

1. **Ask for Class Type**:
   - Actor, Pawn, Character
   - Component (Actor, Scene, Primitive)
   - Object, DataAsset
   - Interface
   - Game Framework (GameMode, PlayerController, etc.)
   - Widget (UMG)

2. **Ask for Class Name**:
   - Enter the name (prefix will be added automatically)
   - Example: "HealthComponent" → "UHealthComponent"

3. **Ask for Features**:
   - Replication support?
   - Tick function?
   - Interface implementations?
   - Common functions (BeginPlay, etc.)?

4. **Generate Code**:
   - Create .h and .cpp files in appropriate Source directories
   - Add necessary includes
   - Generate boilerplate with proper macros
   - Add to project module if needed

### Direct Command Mode

You can also provide details directly:

```
Generate a UActorComponent called HealthComponent with:
- EditAnywhere float MaxHealth (default 100, clamped 0-100)
- BlueprintReadOnly float CurrentHealth
- BlueprintCallable void TakeDamage(float Amount)
- BlueprintNativeEvent void OnDeath()
```

## Generation Examples

### Example 1: Simple Component

**Request**: "Create a UActorComponent for tracking health"

**Generated Files**:

`Source/AntigravityTest/Component/HealthComponent.h`:
```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/**
 * Component for managing actor health
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANTIGRAVITYTEST_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    /** Maximum health value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health", meta=(ClampMin=0, ClampMax=1000))
    float MaxHealth;

    /** Current health value */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
    float CurrentHealth;

    /** Apply damage to this component */
    UFUNCTION(BlueprintCallable, Category="Health")
    void TakeDamage(float DamageAmount);

    /** Get health as a percentage (0.0 to 1.0) */
    UFUNCTION(BlueprintPure, Category="Health")
    float GetHealthPercent() const;

    /** Called when health reaches zero */
    UFUNCTION(BlueprintNativeEvent, Category="Health")
    void OnDeath();
    virtual void OnDeath_Implementation();

protected:
    virtual void BeginPlay() override;
};
```

`Source/AntigravityTest/Component/HealthComponent.cpp`:
```cpp
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
    : MaxHealth(100.0f)
    , CurrentHealth(0.0f)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize current health to max
    CurrentHealth = MaxHealth;
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f)
    {
        return;
    }

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

    if (CurrentHealth <= 0.0f)
    {
        OnDeath();
    }
}

float UHealthComponent::GetHealthPercent() const
{
    if (MaxHealth <= 0.0f)
    {
        return 0.0f;
    }

    return CurrentHealth / MaxHealth;
}

void UHealthComponent::OnDeath_Implementation()
{
    // Blueprint can override this
}
```

### Example 2: Interface Implementation

**Request**: "Create an IInteractable interface with Interact and CanInteract methods"

**Generated Files**:

`Source/AntigravityTest/Interface/Interactable.h`:
```cpp
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for objects that can be interacted with
 */
class IInteractable
{
    GENERATED_BODY()

public:
    /** Perform interaction */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    void Interact(AActor* Interactor);

    /** Check if interaction is currently allowed */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool CanInteract(AActor* Interactor) const;

    /** Get interaction prompt text */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    FText GetInteractionPrompt() const;
};
```

### Example 3: Character Class

**Request**: "Create an ACharacter subclass with enhanced input support"

**Generated Files**:

`Source/AntigravityTest/Character/MyCharacter.h`:
```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCharacter.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * Custom character class with enhanced input
 */
UCLASS(abstract)
class ANTIGRAVITYTEST_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

    /** Called to bind functionality to input */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

    // Enhanced Input Actions
    UPROPERTY(EditAnywhere, Category="Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category="Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category="Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category="Input")
    TObjectPtr<UInputAction> JumpAction;

    // Input handlers
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
};
```

### Example 4: Replicated Property

**Request**: "Add a replicated bool property bIsActive"

**Generated Code**:
```cpp
// In .h file
protected:
    UPROPERTY(Replicated, BlueprintReadOnly, Category="State")
    bool bIsActive;

// In .cpp file
#include "Net/UnrealNetwork.h"

void AMyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyActor, bIsActive);
}
```

## Implementation Steps

When generating code, the skill follows these steps:

### 1. Analyze Request
- Parse class type and name
- Identify required features
- Determine parent class

### 2. Scan Existing Code
```bash
# Find similar classes
Glob: Source/**/*Component.h
Glob: Source/**/*Character.h

# Analyze patterns
Read: <matching files>
- Extract common includes
- Identify naming patterns
- Note comment styles
- Detect project conventions
```

### 3. Select Template
- Match to predefined templates:
  - `actor_template.h/.cpp`
  - `component_template.h/.cpp`
  - `interface_template.h`
  - `character_template.h/.cpp`
  - etc.

### 4. Generate Header (.h)
```cpp
// 1. Pragma once
#pragma once

// 2. Core includes
#include "CoreMinimal.h"
#include "<ParentClass>.h"

// 3. Forward declarations (if needed)
class UInputAction;

// 4. Generated include (always last)
#include "<ClassName>.generated.h"

// 5. Class declaration
UCLASS(<specifiers>)
class <API_MACRO> <ClassName> : public <ParentClass>
{
    GENERATED_BODY()

public:
    // Constructor
    <ClassName>();

    // Overrides
    virtual void BeginPlay() override;

protected:
    // Properties
    UPROPERTY(...)
    float MyProperty;

    // Functions
    UFUNCTION(...)
    void MyFunction();
};
```

### 5. Generate Implementation (.cpp)
```cpp
// 1. Include own header (first)
#include "<ClassName>.h"

// 2. Additional includes
#include "..."

// 3. Constructor
<ClassName>::<ClassName>()
    : MyProperty(DefaultValue)
{
    // Initialization
}

// 4. Function implementations
void <ClassName>::BeginPlay()
{
    Super::BeginPlay();

    // Implementation
}
```

### 6. Update Build.cs (if needed)
- Check if new modules need to be added
- Add to PublicDependencyModuleNames or PrivateDependencyModuleNames

### 7. Verify Structure
- Check that files are in correct directories:
  - Public headers: `Source/<Module>/Public/` or `Source/<Module>/<Category>/`
  - Private headers: `Source/<Module>/Private/`
  - Source files: `Source/<Module>/Private/`
- Verify naming conventions
- Ensure API macros are correct

## Project-Specific Conventions

This skill automatically adapts to project conventions defined in CLAUDE.md:

### Naming Conventions
- **Classes**: Type prefix (A, U, F, I, E, T) + PascalCase
- **Functions**: PascalCase
- **Blueprint Callable**: `Do` prefix for input handlers
- **Blueprint Events**: `BP_` prefix
- **Variables**: PascalCase, `b` prefix for booleans
- **Constants**: UPPER_SNAKE_CASE or PascalCase

### Directory Structure
- AI classes: `Source/AntigravityTest/AI/`
- Components: `Source/AntigravityTest/Component/`
- Items: `Source/AntigravityTest/Items/`
- Tests: `Source/AntigravityTest/Tests/`
- UI: `Source/AntigravityTest/UI/`
- Variant-specific: `Source/AntigravityTest/Variant_<Name>/`

### Module Information
- **Project Module**: AntigravityTest
- **Module Type**: Runtime
- **API Macro**: `ANTIGRAVITYTEST_API`

### Common Includes
```cpp
// Core
#include "CoreMinimal.h"

// Enhanced Input
#include "InputActionValue.h"

// Common Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Common Modules (add to Build.cs if needed)
- Engine
- AIModule
- UMG
- EnhancedInput
- GameplayAbilities (if using GAS)
```

## Advanced Features

### Smart Property Detection

When adding properties, the skill considers:

1. **Type-Based Defaults**:
   - `TObjectPtr<UComponent>` → VisibleAnywhere, BlueprintReadOnly
   - `float` (health/damage) → EditAnywhere with meta=(ClampMin/Max)
   - `bool` → EditAnywhere, BlueprintReadWrite
   - `FVector` (movement) → meta=(Units="cm/s") or similar

2. **Context-Based Specifiers**:
   - Component references → AllowPrivateAccess="true"
   - Gameplay values → Category matching the feature
   - State variables → Consider Replicated for multiplayer

3. **Meta Tag Suggestions**:
   - Numbers → ClampMin, ClampMax
   - Physics → Units="kg", "cm", "s", "deg"
   - Tooltips → Auto-generate from property name

### Interface Implementation Helper

When implementing interfaces:

1. **List all interface methods**
2. **Generate _Implementation functions** for BlueprintNativeEvent
3. **Add virtual overrides** in .h file
4. **Provide default implementations** in .cpp file
5. **Include interface header** properly

### Replication Support

When adding multiplayer support:

1. **Add Replicated specifier** to UPROPERTY
2. **Generate GetLifetimeReplicatedProps()** override
3. **Add DOREPLIFETIME()** entries
4. **Include "Net/UnrealNetwork.h"**
5. **Consider ReplicatedUsing** for callbacks

### Input Action Binding

For Enhanced Input support:

1. **Add Input Action properties** (TObjectPtr<UInputAction>)
2. **Add Input Mapping Context** reference
3. **Generate SetupPlayerInputComponent()** override
4. **Create input handler functions**
5. **Add EnhancedInputComponent** cast and binding

## Error Prevention

The skill helps prevent common mistakes:

### UPROPERTY/UFUNCTION Errors
- ✅ Always include GENERATED_BODY() macro
- ✅ Include .generated.h as last include
- ✅ Use correct specifier combinations
- ✅ Add Category for Blueprint visibility

### Memory Management
- ✅ Use TObjectPtr<> for UObject references
- ✅ Mark component properties with UPROPERTY
- ✅ Avoid raw pointers to UObjects without UPROPERTY

### Network Replication
- ✅ Include GetLifetimeReplicatedProps() when using Replicated
- ✅ Use Server/Client function naming (_Implementation suffix)
- ✅ Add validation functions for Server RPCs

### Module Dependencies
- ✅ Check Build.cs for required modules
- ✅ Add to PublicDependencyModuleNames when using public headers
- ✅ Add to PrivateDependencyModuleNames for private implementation

## Integration with Existing Skills

Works well with other skills:

- **analyze-blueprints**: Analyze Blueprint classes that use generated C++ classes
- **link-plugin**: Link plugins that depend on generated classes

## Troubleshooting

### Common Issues

1. **"Cannot open include file"**
   - Check Build.cs for required modules
   - Verify include paths

2. **"Unresolved external symbol"**
   - Ensure implementation (.cpp) file exists
   - Check that functions are not declared but not defined

3. **"UPROPERTY not recognized"**
   - Verify GENERATED_BODY() is present
   - Check that .generated.h is included last

4. **"Blueprint could not be loaded"**
   - Compile C++ code first
   - Restart Unreal Editor

## Best Practices

When using this skill:

1. **Start Simple**: Generate basic class first, add complexity later
2. **Review Generated Code**: Always review before committing
3. **Match Project Style**: Skill learns from your code, keep it consistent
4. **Use Categories**: Group related properties/functions
5. **Document**: Add doc comments for complex functionality
6. **Test in Blueprint**: Create Blueprint child class to test

## Example Workflow

### Creating a New Game Feature

1. **Generate Component**:
   ```
   Create UActorComponent for inventory management
   - TArray<UItemDataAsset*> Items
   - int32 MaxSlots
   - AddItem/RemoveItem functions
   ```

2. **Generate Interface**:
   ```
   Create IInventoryHolder interface
   - GetInventory() → UInventoryComponent*
   ```

3. **Implement Interface**:
   ```
   Add IInventoryHolder to AMyCharacter
   - Generate interface implementation
   ```

4. **Generate Data Asset**:
   ```
   Create UDataAsset for item definitions
   - FText ItemName
   - UTexture2D* Icon
   - float Weight
   ```

5. **Build and Test**:
   ```bash
   # Compile
   Build.bat

   # Open editor
   LaunchEditor.bat

   # Create Blueprint child classes
   # Test functionality
   ```

## Technical Implementation

The skill uses these tools:

- **Glob**: Search for existing class files
- **Grep**: Find code patterns and conventions
- **Read**: Analyze existing implementations
- **Write**: Create new .h and .cpp files
- **Edit**: Modify existing files (add properties/functions)
- **Bash**: Run builds, check compilation

## Output Format

Generated files follow this structure:

```
Generated: Source/AntigravityTest/Component/HealthComponent.h
Generated: Source/AntigravityTest/Component/HealthComponent.cpp

Next steps:
1. Review generated code
2. Build project: Build.bat
3. Create Blueprint child class in editor
4. Assign to actors in your levels

Note: This class is marked 'abstract' - create a Blueprint
child class to use it in your game.
```

## Future Enhancements

Potential additions:

- AI Behavior Tree node generation
- Enhanced Input mapping generation
- Gameplay Ability System (GAS) ability generation
- Slate UI widget generation
- Automated testing code generation

## Related Documentation

- UE5 Coding Standard: https://docs.unrealengine.com/5.7/epic-cplusplus-coding-standard-for-unreal-engine/
- UPROPERTY Reference: https://docs.unrealengine.com/5.7/unreal-engine-uproperties/
- UFUNCTION Reference: https://docs.unrealengine.com/5.7/ufunctions-in-unreal-engine/
- Gameplay Framework: https://docs.unrealengine.com/5.7/gameplay-framework-in-unreal-engine/
