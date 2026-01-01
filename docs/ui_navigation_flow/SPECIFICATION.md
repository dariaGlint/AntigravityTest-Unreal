# UI Navigation Flow Specification

## Overview

This document specifies the UI navigation flow system for AntigravityTest, built on top of the ScreenTransitionSystem plugin. The system provides a unified, robust framework for managing screen transitions, modal dialogs, and HUD elements across all game variants.

## Architecture

### Core Components

#### 1. ScreenTransitionManager (Extended)

**New Features:**
- Context data passing between screens
- Stack depth validation (configurable max depth)
- Comprehensive error handling and recovery
- Debug logging system
- Performance metrics tracking

**Context Data Support:**
```cpp
USTRUCT(BlueprintType)
struct FScreenTransitionContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
    TMap<FString, FString> StringData;

    UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
    TMap<FString, int32> IntData;

    UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
    TMap<FString, float> FloatData;

    UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
    TMap<FString, bool> BoolData;
};
```

**New Methods:**
- `TransitionToScreenWithContext(ScreenClass, Context, bUseTransition, TransitionEffectClass)`
- `PushScreenWithContext(ScreenClass, Context, bAsModal, bUseTransition, TransitionEffectClass)`
- `GetMaxStackDepth() / SetMaxStackDepth(int32)`
- `GetLastError() / ClearLastError()`
- `GetTransitionMetrics()` - Returns performance data

**Error Handling:**
- Validates screen classes before creation
- Prevents stack overflow with configurable depth limit
- Logs all errors with severity levels (Warning, Error, Critical)
- Provides error callbacks for Blueprint handling

#### 2. ScreenBase (Extended)

**New Virtual Methods:**
- `OnEnterWithContext(FScreenTransitionContext)` - Receives context data on entry
- `OnValidate()` - Returns bool, called before screen activation
- `GetScreenName()` - Returns FString for debugging

**Input Handling:**
- Enhanced input mode control
- Focus management for nested UI elements
- Automatic input routing based on screen stack

#### 3. TransitionEffect Implementations

##### FadeTransitionEffect
- Smooth opacity fade from 0 to 1
- Configurable duration (default: 0.3s)
- Optional intermediate solid color (for "fade to black" effect)
- Easing curve support (Linear, EaseIn, EaseOut, EaseInOut)

##### SlideTransitionEffect
- Directional slide animations (Left, Right, Up, Down)
- Configurable duration (default: 0.4s)
- Optional parallax effect for depth
- Easing curve support

#### 4. Debug System

**Console Commands:**
- `ScreenTransition.ShowDebugUI` - Toggle debug UI
- `ScreenTransition.DumpStack` - Print current screen stack
- `ScreenTransition.SetMaxDepth <value>` - Configure max stack depth
- `ScreenTransition.LogLevel <level>` - Set logging verbosity (0-3)

**Debug UI Widget:**
- Real-time screen stack visualization
- Transition metrics (duration, frame drops)
- Memory usage tracking
- Error log display

## Screen Hierarchy

### System Screens (Full-Screen, Replace Stack)

#### WBP_Splash
- **Purpose:** Initial loading screen with branding
- **Transition:** Auto-transitions to WBP_Title after 2s or asset load completion
- **Input:** None (input disabled)
- **Context Data:** None

#### WBP_Title
- **Purpose:** Game title screen with "Press Any Key" prompt
- **Transition:** To WBP_MainMenu on any input
- **Input:** Any key accepted
- **Context Data:** None

#### WBP_MainMenu
- **Purpose:** Main menu hub
- **Options:**
  - Start Game (variant selection)
  - Settings
  - Credits
  - Quit
- **Transition:**
  - To Game HUD (WBP_CombatHUD / WBP_SideScrollingHUD / WBP_PlatformingHUD) with context `{VariantType}`
  - To WBP_Settings with context `{ReturnScreen: "MainMenu"}`
  - To WBP_Credits with context `{ReturnScreen: "MainMenu"}`
- **Input:** Mouse/Keyboard/Gamepad navigation

#### WBP_Settings
- **Purpose:** Game settings configuration
- **Categories:**
  - Graphics
  - Audio
  - Controls
  - Gameplay
- **Transition:** Back to caller (using context `ReturnScreen`)
- **Context Data:** `{ReturnScreen: string}`
- **Input:** Full UI navigation

#### WBP_Credits
- **Purpose:** Credits roll
- **Transition:** Back to caller after completion or skip
- **Context Data:** `{ReturnScreen: string}`
- **Input:** Skip button / back

### In-Game Modals (Overlay, Push to Stack)

#### WBP_PauseMenu
- **Purpose:** Pause game overlay
- **Options:**
  - Resume
  - Settings
  - Return to Main Menu
  - Quit
- **Transition:**
  - Pop (Resume)
  - Push WBP_Settings with context `{ReturnScreen: "PauseMenu"}`
  - To WBP_MainMenu (with confirmation)
- **Input:** Pause key to toggle, UI navigation
- **Context Data:** `{CanResume: bool, GameTime: float}`

#### WBP_Inventory
- **Purpose:** Player inventory management
- **Features:**
  - Item grid display
  - Item details panel
  - Equipment slots
  - Use/Drop/Equip actions
- **Transition:** Pop on close
- **Input:** Inventory key to toggle, UI navigation
- **Context Data:** `{InventoryData: serialized}`

#### WBP_QuestLog
- **Purpose:** Quest tracking and management
- **Features:**
  - Active quests list
  - Quest details
  - Objectives tracking
  - Rewards preview
- **Transition:** Pop on close
- **Input:** Quest log key to toggle
- **Context Data:** `{ActiveQuestId: int32}`

#### WBP_ConfirmDialog
- **Purpose:** Generic confirmation dialog
- **Options:**
  - Confirm
  - Cancel
- **Transition:** Pop on either choice, executes callback
- **Input:** Accept/Cancel keys, mouse
- **Context Data:**
  - `{Title: string, Message: string, ConfirmText: string, CancelText: string, CallbackId: string}`

#### WBP_GameResult
- **Purpose:** End-of-level results screen
- **Features:**
  - Score display
  - Statistics
  - Unlocks/Achievements
  - Continue/Retry/Exit options
- **Transition:**
  - To next level HUD (Continue)
  - Reload current level (Retry)
  - To WBP_MainMenu (Exit)
- **Context Data:**
  - `{Score: int32, Time: float, Deaths: int32, ItemsCollected: int32, NextLevel: string}`

### Variant-Specific HUDs (In-Game, Full-Screen)

#### WBP_CombatHUD
- **Purpose:** Combat variant HUD
- **Elements:**
  - Health bar
  - Combo counter
  - Enemy count
  - Special ability cooldowns
- **Transition:** To WBP_PauseMenu (push), To WBP_GameResult (replace)
- **Input:** Game controls + pause key
- **Context Data:** `{VariantType: "Combat", LevelName: string}`

#### WBP_SideScrollingHUD
- **Purpose:** Side-scrolling variant HUD
- **Elements:**
  - Health bar
  - Lives counter
  - Score
  - Timer
- **Transition:** To WBP_PauseMenu (push), To WBP_GameResult (replace)
- **Input:** Game controls + pause key
- **Context Data:** `{VariantType: "SideScrolling", LevelName: string}`

#### WBP_PlatformingHUD
- **Purpose:** Platforming variant HUD
- **Elements:**
  - Health bar
  - Collectibles counter
  - Timer
  - Checkpoint indicator
- **Transition:** To WBP_PauseMenu (push), To WBP_GameResult (replace)
- **Input:** Game controls + pause key
- **Context Data:** `{VariantType: "Platforming", LevelName: string}`

## Transition Flow Examples

### Game Startup Flow
```
WBP_Splash (Fade In, 2s)
  → WBP_Title (Fade, 0.3s)
  → WBP_MainMenu (Fade, 0.3s)
  → WBP_CombatHUD (Fade, 0.5s) {VariantType: "Combat", LevelName: "Level1"}
```

### Pause Flow
```
WBP_CombatHUD (Active)
  → Push WBP_PauseMenu (Slide Down, 0.3s)
  → User selects "Settings"
  → Push WBP_Settings (Slide Left, 0.3s) {ReturnScreen: "PauseMenu"}
  → User saves settings
  → Pop WBP_Settings (Slide Right, 0.3s)
  → Back to WBP_PauseMenu
  → User selects "Resume"
  → Pop WBP_PauseMenu (Slide Up, 0.3s)
  → Back to WBP_CombatHUD
```

### Game Over Flow
```
WBP_CombatHUD (Player dies)
  → WBP_GameResult (Fade, 0.5s) {Score: 1000, Time: 120.5, Deaths: 3, ...}
  → User selects "Retry"
  → WBP_CombatHUD (Fade, 0.5s) {VariantType: "Combat", LevelName: "Level1"}
```

## Performance Requirements

- **Transition Duration:** ≤ 500ms per transition
- **Memory Overhead:** ≤ 5MB for UI system
- **Frame Rate Impact:** ≤ 2ms per frame during transitions
- **Stack Depth:** Maximum 10 screens (configurable)

## Error Handling

### Error Types
1. **Invalid Screen Class:** Screen class is null or invalid
2. **Stack Overflow:** Attempting to push beyond max stack depth
3. **Stack Underflow:** Attempting to pop from empty stack
4. **Transition Timeout:** Transition effect exceeds timeout threshold
5. **Context Validation Failed:** Screen rejects provided context data

### Error Recovery
- Log error with full context (stack trace, screen names, context data)
- Attempt graceful fallback (e.g., transition without effect if effect fails)
- Notify application layer via error callbacks
- Optionally display user-facing error dialog (WBP_ConfirmDialog)

## Testing Strategy

### Unit Tests
- Screen creation/destruction
- Stack push/pop operations
- Context data serialization/deserialization
- Error condition handling
- Transition effect timing

### Integration Tests
- Full flow tests (startup → main menu → game → pause → resume)
- Cross-variant transitions
- Modal stacking (multiple overlays)
- Memory leak detection
- Input handling across screen changes

### Performance Tests
- Transition duration benchmarks
- Memory profiling (stack growth/shrink)
- Frame rate stability during transitions
- Rapid transition stress test

## Configuration

### DefaultEngine.ini
```ini
[/Script/ScreenTransitionSystem.ScreenTransitionManager]
DefaultTransitionEffectClass=/Script/ScreenTransitionSystem.FadeTransitionEffect
MaxStackDepth=10
EnableDebugLogging=true
LogLevel=2
TransitionTimeout=5.0
```

## Blueprint Integration

All screens should be implemented as Blueprint subclasses of `UScreenBase` for maximum flexibility. C++ provides the framework, Blueprints provide the content and visual design.

### Example: Creating a Custom Screen in Blueprint

1. Create Blueprint Widget based on `UScreenBase`
2. Override `OnEnterWithContext` to receive initialization data
3. Override `OnExit` for cleanup
4. Override `OnValidate` to check preconditions
5. Bind UI events to Blueprint functions
6. Use ScreenTransitionManager functions to navigate

## Future Enhancements

- **Animation Curves:** Support for custom animation curves
- **Async Loading:** Asynchronous screen asset loading with progress bars
- **Screen Pooling:** Reuse screen instances to reduce allocation overhead
- **Transition Chaining:** Queue multiple transitions
- **Accessibility:** Screen reader support, high contrast modes
- **Analytics Integration:** Track user navigation patterns

---

**Version:** 1.0
**Last Updated:** 2025-12-31
**Author:** Claude Code
