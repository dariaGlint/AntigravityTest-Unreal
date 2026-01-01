# UI Navigation Flow - Test Specification

## Overview

This document defines the comprehensive test strategy for the UI Navigation Flow system, covering unit tests, integration tests, performance tests, and manual QA procedures.

## Test Coverage Goals

- **Unit Test Coverage:** ≥ 80%
- **Integration Test Coverage:** All critical user flows
- **Performance Test Coverage:** All transition types and screen combinations
- **Manual QA Coverage:** 100% of acceptance criteria

## Unit Tests

### ScreenTransitionManager Tests

#### Test Suite: ScreenTransitionManagerTests

**Test Case: Initialize_SetsDefaultState**
- **Setup:** Create ScreenTransitionManager
- **Action:** Call Initialize()
- **Assert:**
  - CurrentScreen is null
  - ScreenStack is empty
  - bIsTransitioning is false
  - DefaultTransitionEffectClass is null

**Test Case: TransitionToScreen_WithValidClass_CreatesAndActivatesScreen**
- **Setup:** Create manager with mock screen class
- **Action:** TransitionToScreen(MockScreenClass)
- **Assert:**
  - CurrentScreen is not null
  - CurrentScreen is instance of MockScreenClass
  - OnScreenChanged event was broadcast
  - OnTransitionCompleted event was broadcast

**Test Case: TransitionToScreen_WhileTransitioning_DoesNothing**
- **Setup:** Manager with ongoing transition
- **Action:** TransitionToScreen(AnotherScreenClass)
- **Assert:**
  - Transition is not started
  - CurrentScreen remains unchanged

**Test Case: TransitionToScreen_ClearsStack**
- **Setup:** Manager with 3 screens in stack
- **Action:** TransitionToScreen(NewScreenClass)
- **Assert:**
  - ScreenStack is empty
  - All previous screens were deactivated

**Test Case: PushScreen_AddsToStack**
- **Setup:** Manager with CurrentScreen
- **Action:** PushScreen(NewScreenClass)
- **Assert:**
  - ScreenStack.Num() == 1
  - ScreenStack[0].Screen is previous CurrentScreen
  - CurrentScreen is new screen

**Test Case: PushScreen_ExceedsMaxDepth_ReturnsError**
- **Setup:** Manager with MaxStackDepth=5, stack has 5 screens
- **Action:** PushScreen(NewScreenClass)
- **Assert:**
  - Returns false or logs error
  - ScreenStack.Num() remains 5
  - CurrentScreen unchanged

**Test Case: PopScreen_WithEmptyStack_ReturnsFalse**
- **Setup:** Manager with empty stack
- **Action:** PopScreen()
- **Assert:**
  - Returns false
  - CurrentScreen unchanged

**Test Case: PopScreen_RestoresPreviousScreen**
- **Setup:** Manager with 2 screens in stack
- **Action:** PopScreen()
- **Assert:**
  - ScreenStack.Num() == 1
  - CurrentScreen is the top stack screen
  - Transition occurred

**Test Case: TransitionWithContext_PassesContextToScreen**
- **Setup:** Mock screen that captures context
- **Action:** TransitionToScreenWithContext(MockScreenClass, Context)
- **Assert:**
  - Screen's OnEnterWithContext was called
  - Context data matches input

**Test Case: SetMaxStackDepth_ValidatesNewValue**
- **Action:** SetMaxStackDepth(-1)
- **Assert:** Returns error or clamps to valid range (1-100)

**Test Case: GetTransitionMetrics_ReturnsValidData**
- **Setup:** Perform several transitions
- **Action:** GetTransitionMetrics()
- **Assert:**
  - Metrics contain transition count
  - Average duration is reasonable (< 1s)
  - No memory leaks detected

### ScreenBase Tests

#### Test Suite: ScreenBaseTests

**Test Case: OnEnter_BroadcastsDelegate**
- **Setup:** Create screen instance
- **Action:** Call OnEnter()
- **Assert:** OnScreenEnterDelegate was broadcast

**Test Case: OnExit_BroadcastsDelegate**
- **Setup:** Create and enter screen
- **Action:** Call OnExit()
- **Assert:** OnScreenExitDelegate was broadcast

**Test Case: OnEnterWithContext_ReceivesContext**
- **Setup:** Create screen with context handler
- **Action:** Call OnEnterWithContext(Context)
- **Assert:**
  - Context data is accessible in screen
  - Context matches input

**Test Case: OnValidate_ReturnsTrueByDefault**
- **Setup:** Create base screen
- **Action:** Call OnValidate()
- **Assert:** Returns true

**Test Case: ApplyInputSettings_ConfiguresMouse**
- **Setup:** Screen with bShowMouseCursor=true
- **Action:** Call OnEnter()
- **Assert:**
  - Mouse cursor is visible
  - Click events are enabled

### TransitionEffect Tests

#### Test Suite: FadeTransitionEffectTests

**Test Case: StartTransition_InitializesState**
- **Setup:** Create FadeTransitionEffect
- **Action:** StartTransition(FromScreen, ToScreen)
- **Assert:**
  - bIsPlaying is true
  - CurrentTime is 0
  - Timer is active

**Test Case: TickTransition_UpdatesAlpha**
- **Setup:** Effect with Duration=1.0s
- **Action:** Tick at 0.5s
- **Assert:**
  - Alpha is approximately 0.5
  - Neither screen is fully visible/hidden yet

**Test Case: CompleteTransition_BroadcastsDelegate**
- **Setup:** Effect in progress
- **Action:** Let transition complete
- **Assert:**
  - OnTransitionComplete was broadcast
  - bIsPlaying is false

**Test Case: FadeTransition_Duration_MeetsRequirement**
- **Setup:** FadeTransitionEffect with default duration
- **Action:** Measure actual transition time
- **Assert:** Duration ≤ 500ms

#### Test Suite: SlideTransitionEffectTests

**Test Case: SlideLeft_MovesScreenLeftToRight**
- **Setup:** SlideTransitionEffect with Direction=Left
- **Action:** StartTransition(FromScreen, ToScreen)
- **Assert:**
  - ToScreen starts off-screen left
  - ToScreen ends at center
  - FromScreen moves right

**Test Case: SlideTransition_Duration_MeetsRequirement**
- **Setup:** SlideTransitionEffect with default duration
- **Action:** Measure actual transition time
- **Assert:** Duration ≤ 500ms

**Test Case: SlideTransition_EasingCurve_Applied**
- **Setup:** SlideTransitionEffect with EaseInOut curve
- **Action:** Sample alpha at 0%, 25%, 50%, 75%, 100%
- **Assert:**
  - Alpha progression follows ease curve (slow start, fast middle, slow end)

### Error Handling Tests

#### Test Suite: ErrorHandlingTests

**Test Case: TransitionToScreen_NullClass_LogsError**
- **Action:** TransitionToScreen(nullptr)
- **Assert:**
  - Error is logged
  - GetLastError() returns appropriate error code
  - No crash

**Test Case: PushScreen_StackOverflow_LogsError**
- **Setup:** Stack at max depth
- **Action:** PushScreen(NewClass)
- **Assert:**
  - Error is logged
  - Stack remains at max depth
  - New screen is not created

**Test Case: TransitionTimeout_RecoveryMechanism**
- **Setup:** Mock transition effect that never completes
- **Action:** Wait for timeout (5s)
- **Assert:**
  - Transition is force-completed
  - Error is logged
  - System returns to stable state

## Integration Tests

### User Flow Tests

#### Test Case: GameStartupFlow_Complete**
- **Flow:** Splash → Title → MainMenu → CombatHUD
- **Steps:**
  1. Simulate game startup
  2. Wait for splash screen (2s auto-transition)
  3. Press any key on title screen
  4. Select "Start Game - Combat" in main menu
  5. Verify CombatHUD is active
- **Assert:**
  - All transitions completed successfully
  - No errors logged
  - Final screen is WBP_CombatHUD
  - Context data passed correctly (VariantType="Combat")

#### Test Case: PauseResumeFlow_Complete**
- **Flow:** CombatHUD → PauseMenu → Resume
- **Steps:**
  1. Start from active CombatHUD
  2. Press pause key
  3. Verify PauseMenu is pushed (stack depth = 1)
  4. Select "Resume"
  5. Verify PauseMenu is popped, back to CombatHUD
- **Assert:**
  - Stack operations correct
  - Game state paused/resumed appropriately
  - No memory leaks

#### Test Case: NestedMenuFlow_Complete**
- **Flow:** CombatHUD → PauseMenu → Settings → Back → Back
- **Steps:**
  1. Start from CombatHUD
  2. Push PauseMenu
  3. Push Settings (from PauseMenu)
  4. Pop Settings
  5. Pop PauseMenu
- **Assert:**
  - Stack depth tracks correctly (0 → 1 → 2 → 1 → 0)
  - Each screen receives correct context (ReturnScreen)
  - Final state is CombatHUD

#### Test Case: GameOverFlow_Complete**
- **Flow:** CombatHUD → GameResult → Retry → CombatHUD
- **Steps:**
  1. Trigger game over condition
  2. Verify GameResult screen appears with stats
  3. Select "Retry"
  4. Verify level reloads and CombatHUD appears
- **Assert:**
  - Stack is cleared on game over
  - Result screen receives correct context (score, time, etc.)
  - Level restarts properly

#### Test Case: QuitToMainMenuFlow_Complete**
- **Flow:** CombatHUD → PauseMenu → Confirm Quit → MainMenu
- **Steps:**
  1. From CombatHUD, open PauseMenu
  2. Select "Main Menu"
  3. Confirm in dialog
  4. Verify transition to MainMenu
- **Assert:**
  - Stack is cleared
  - Game state is reset
  - MainMenu is active

### Cross-Variant Tests

#### Test Case: VariantSwitching_PreservesSystemState**
- **Steps:**
  1. Start Combat variant
  2. Return to MainMenu
  3. Start SideScrolling variant
  4. Return to MainMenu
  5. Start Platforming variant
- **Assert:**
  - No memory leaks between variant switches
  - Each HUD receives correct context
  - System state resets properly

### Modal Stacking Tests

#### Test Case: MultipleModals_StackCorrectly**
- **Flow:** CombatHUD → PauseMenu → Inventory → QuestLog
- **Steps:**
  1. Push PauseMenu (stack depth = 1)
  2. Push Inventory (stack depth = 2)
  3. Push QuestLog (stack depth = 3)
  4. Pop QuestLog (stack depth = 2)
  5. Pop Inventory (stack depth = 1)
  6. Pop PauseMenu (stack depth = 0)
- **Assert:**
  - Stack depth never exceeds max
  - Each screen properly overlays previous
  - Input routing works correctly (only top screen receives input)

#### Test Case: ModalInputBlocking_WorksCorrectly**
- **Setup:** CombatHUD with PauseMenu modal on top
- **Action:** Send game input (move, attack)
- **Assert:**
  - Game does not respond to input
  - PauseMenu receives input instead

## Performance Tests

### Transition Duration Tests

#### Test Case: FadeTransition_MeetsDurationRequirement**
- **Action:** Measure 100 fade transitions
- **Assert:**
  - Average duration ≤ 500ms
  - 95th percentile ≤ 550ms
  - No transition > 1000ms

#### Test Case: SlideTransition_MeetsDurationRequirement**
- **Action:** Measure 100 slide transitions (all directions)
- **Assert:**
  - Average duration ≤ 500ms
  - 95th percentile ≤ 550ms

### Memory Tests

#### Test Case: TransitionMemoryUsage_WithinBounds**
- **Action:** Perform 1000 transitions between screens
- **Assert:**
  - Total memory growth < 5MB
  - No memory leaks detected (all screens properly destroyed)

#### Test Case: StackMemoryUsage_WithinBounds**
- **Action:** Push max stack depth, pop all, repeat 100 times
- **Assert:**
  - Memory usage stable (no growth pattern)
  - No leaked screen instances

### Frame Rate Tests

#### Test Case: TransitionFrameRate_MinimalImpact**
- **Setup:** Measure baseline frame time
- **Action:** Trigger transition, measure frame times during transition
- **Assert:**
  - Frame time increase ≤ 2ms
  - No frame drops > 16ms (60 FPS)

### Stress Tests

#### Test Case: RapidTransitions_NoErrors**
- **Action:** Trigger 100 transitions in rapid succession (every 100ms)
- **Assert:**
  - All transitions complete or properly cancel
  - No crashes or hangs
  - System returns to stable state

#### Test Case: MaxStackDepth_Sustained**
- **Action:** Push screens to max depth, hold for 60 seconds, pop all
- **Assert:**
  - System remains responsive
  - No performance degradation
  - Memory usage stable

## Manual QA Tests

### Checklist: System Screens

- [ ] **WBP_Splash**
  - [ ] Displays correctly on startup
  - [ ] Auto-transitions after 2s or asset load
  - [ ] Branding/logo visible

- [ ] **WBP_Title**
  - [ ] "Press Any Key" prompt visible
  - [ ] Responds to any input
  - [ ] Transitions to MainMenu smoothly

- [ ] **WBP_MainMenu**
  - [ ] All buttons visible and functional
  - [ ] Mouse navigation works
  - [ ] Keyboard navigation works
  - [ ] Gamepad navigation works
  - [ ] Transitions to correct screens

- [ ] **WBP_Settings**
  - [ ] All settings categories accessible
  - [ ] Settings persist after save
  - [ ] "Back" returns to calling screen
  - [ ] Graphics settings apply correctly
  - [ ] Audio settings apply correctly

- [ ] **WBP_Credits**
  - [ ] Credits scroll smoothly
  - [ ] Skip button works
  - [ ] Returns to calling screen

### Checklist: In-Game Modals

- [ ] **WBP_PauseMenu**
  - [ ] Pause key toggles menu
  - [ ] Game freezes when paused
  - [ ] Resume works correctly
  - [ ] Settings accessible from pause
  - [ ] "Main Menu" shows confirmation dialog

- [ ] **WBP_Inventory**
  - [ ] Inventory key toggles menu
  - [ ] Item grid displays correctly
  - [ ] Item details update on selection
  - [ ] Use/Drop/Equip actions work

- [ ] **WBP_QuestLog**
  - [ ] Quest log key toggles menu
  - [ ] Active quests display correctly
  - [ ] Quest details update on selection
  - [ ] Objectives track progress

- [ ] **WBP_ConfirmDialog**
  - [ ] Dialog displays with correct text
  - [ ] Confirm button works
  - [ ] Cancel button works
  - [ ] ESC key cancels
  - [ ] Callback executes on confirm

- [ ] **WBP_GameResult**
  - [ ] Statistics display correctly
  - [ ] Score calculated correctly
  - [ ] Continue loads next level
  - [ ] Retry reloads current level
  - [ ] Exit returns to main menu

### Checklist: Variant HUDs

- [ ] **WBP_CombatHUD**
  - [ ] Health bar updates correctly
  - [ ] Combo counter increments
  - [ ] Enemy count accurate
  - [ ] Ability cooldowns display
  - [ ] Pause accessible

- [ ] **WBP_SideScrollingHUD**
  - [ ] Health bar updates correctly
  - [ ] Lives counter accurate
  - [ ] Score increments
  - [ ] Timer counts correctly
  - [ ] Pause accessible

- [ ] **WBP_PlatformingHUD**
  - [ ] Health bar updates correctly
  - [ ] Collectibles counter accurate
  - [ ] Timer counts correctly
  - [ ] Checkpoint indicator updates
  - [ ] Pause accessible

### Checklist: Transitions

- [ ] **Fade Transition**
  - [ ] Smooth opacity change
  - [ ] Duration ≤ 500ms
  - [ ] No visual artifacts

- [ ] **Slide Transition**
  - [ ] Smooth directional movement
  - [ ] All 4 directions work (Left, Right, Up, Down)
  - [ ] Duration ≤ 500ms
  - [ ] No visual artifacts

### Checklist: Debug Features

- [ ] **Console Commands**
  - [ ] `ScreenTransition.ShowDebugUI` toggles debug UI
  - [ ] `ScreenTransition.DumpStack` prints stack
  - [ ] `ScreenTransition.SetMaxDepth` changes limit
  - [ ] `ScreenTransition.LogLevel` changes verbosity

- [ ] **Debug UI**
  - [ ] Screen stack visualization updates in real-time
  - [ ] Transition metrics display correctly
  - [ ] Memory usage tracks accurately
  - [ ] Error log displays recent errors

## Acceptance Criteria Verification

### Quality Gates

- [ ] **Unit Tests:** 100% pass, ≥ 80% coverage
- [ ] **Integration Tests:** 100% pass
- [ ] **Performance Tests:** All metrics within requirements
  - [ ] Transition duration ≤ 500ms
  - [ ] Memory overhead ≤ 5MB
  - [ ] Frame impact ≤ 2ms
- [ ] **Manual QA:** 100% checklist complete
- [ ] **Bug Counts:**
  - [ ] 0 Critical bugs
  - [ ] 0 Major bugs
  - [ ] ≤ 5 Minor bugs (documented)

### Code Quality

- [ ] **Code Review:** Passed peer review
- [ ] **Documentation:** All public APIs documented
- [ ] **Coding Standards:** Follows UE5 coding standards
- [ ] **Performance:** No regressions from baseline

## Test Data

### Mock Screens

Create lightweight mock screen classes for testing:
- `UMockScreenBase` - Minimal implementation for unit tests
- `UMockScreenWithContext` - Captures context data for validation
- `UMockScreenWithValidation` - Returns configurable validation result
- `UMockScreenSlowLoading` - Simulates slow loading for timeout tests

### Mock Transition Effects

- `UMockTransitionInstant` - Completes immediately
- `UMockTransitionNeverCompletes` - For timeout testing
- `UMockTransitionVariable` - Configurable duration

## Test Execution Schedule

### Phase 1: Unit Tests (Week 1)
- Implement all unit tests
- Achieve 80%+ coverage
- Fix all failing tests

### Phase 2: Integration Tests (Week 2)
- Implement flow tests
- Verify all user scenarios
- Fix integration issues

### Phase 3: Performance Tests (Week 3)
- Run performance benchmarks
- Identify bottlenecks
- Optimize as needed

### Phase 4: Manual QA (Week 4)
- Execute full QA checklist
- Log all bugs
- Verify fixes

### Phase 5: Final Validation (Week 5)
- Re-run all tests
- Verify acceptance criteria
- Sign-off

## Bug Severity Definitions

### Critical
- System crash or hang
- Data loss
- Complete feature failure
- Security vulnerability

### Major
- Feature partially broken
- Significant performance degradation
- Error without graceful recovery
- User experience severely impacted

### Minor
- Visual glitches
- Minor performance impact
- Edge case failures
- Polish issues

### Trivial
- Typos
- Minor visual inconsistencies
- Logging/debug issues

---

**Version:** 1.0
**Last Updated:** 2025-12-31
**Author:** Claude Code
