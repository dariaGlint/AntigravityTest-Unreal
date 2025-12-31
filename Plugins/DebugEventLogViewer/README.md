# Debug Event Log Viewer Plugin

## Overview

The Debug Event Log Viewer plugin provides a real-time event logging and viewing system for Unreal Engine 5 games. It helps developers track and debug game events such as damage, state transitions, AI behavior, and system events.

## Features

- **Real-time Event Logging**: Automatically log events as they occur during gameplay
- **Event Categorization**: Organize events by category (Combat, State, AI, System, Custom)
- **Severity Levels**: Classify events by severity (Verbose, Info, Warning, Error, Critical)
- **Ring Buffer**: Configurable maximum event count with automatic oldest-event removal
- **Filtering**: Filter events by category, severity, and search text
- **Export**: Export event logs to CSV or JSON formats
- **Blueprint Support**: Full Blueprint integration for logging and viewing events
- **Console Commands**: Debug console commands for quick access
- **Auto Event Collection**: Components that automatically log common game events
- **Non-Shipping Only**: Automatically disabled in Shipping builds

## Installation

The plugin is already enabled in the project. No additional installation required.

## Quick Start

### 1. Logging Events from C++

```cpp
#include "EventLogSubsystem.h"

void AMyCharacter::TakeDamage(float Damage)
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEventLogSubsystem* EventLog = GI->GetSubsystem<UEventLogSubsystem>())
        {
            EventLog->LogEvent(
                EDebugEventCategory::Combat,
                EDebugEventSeverity::Info,
                FString::Printf(TEXT("Took %.1f damage"), Damage),
                this
            );
        }
    }
}
```

### 2. Logging Events from Blueprint

1. Get the Event Log Subsystem (Get Game Instance -> Get Subsystem -> Event Log Subsystem)
2. Call "Log Event" node
3. Fill in Category, Severity, Message, and optional Actor reference

### 3. Using Combat Event Listener Component

Add the `CombatEventListener` component to your combat characters:

```cpp
// In your character's constructor or BeginPlay
UCombatEventListener* EventListener = CreateDefaultSubobject<UCombatEventListener>(TEXT("CombatEventListener"));

// Then call the logging functions when events occur:
EventListener->LogDamageEvent(DamageAmount, DamageCauser, DamageLocation);
EventListener->LogDeathEvent(Killer);
EventListener->LogAttackEvent(TEXT("Melee"), Target);
EventListener->LogHealEvent(HealAmount, Healer);
```

### 4. Viewing Events

Create a UMG widget that inherits from `UEventLogWidget` and implement the `CreateEventEntry` Blueprint event to customize how events are displayed.

Press **F10** to toggle the event log widget visibility (if you've set up the widget).

## Console Commands

The following console commands are available:

- `Debug.ShowEventLog` - Show or hide the event log widget
- `Debug.ExportEventLog [CSV|JSON]` - Export event log to file
- `Debug.ClearEventLog` - Clear all events
- `Debug.FilterEventLog [Combat|State|AI|System]` - Filter events by category
- `Debug.SetMaxEvents <number>` - Set maximum number of events to store

## API Reference

### UEventLogSubsystem

Main subsystem for event logging.

#### Key Functions

- `LogEvent(Category, Severity, Message, Actor, ContextData)` - Log a new event
- `GetEvents()` - Get all logged events
- `GetFilteredEvents(Categories, Severities, SearchText)` - Get filtered events
- `ClearEvents()` - Clear all events
- `SetMaxEvents(MaxEvents)` - Set maximum event count
- `ExportToCSV(FilePath)` - Export to CSV file
- `ExportToJSON(FilePath)` - Export to JSON file
- `CopyToClipboard(Format)` - Copy events to clipboard
- `SetLoggingEnabled(bEnabled)` - Enable/disable logging

### FDebugEvent

Event data structure.

#### Fields

- `Timestamp` (FDateTime) - When the event occurred
- `FrameNumber` (int64) - Frame number when event occurred
- `Category` (EDebugEventCategory) - Event category
- `Severity` (EDebugEventSeverity) - Event severity level
- `Message` (FString) - Event message
- `Actor` (TWeakObjectPtr<AActor>) - Actor that triggered the event
- `ContextData` (FString) - Additional context in JSON format

### UCombatEventListener

Component for automatically logging combat events.

#### Functions

- `LogDamageEvent(DamageAmount, DamageCauser, DamageLocation)` - Log damage
- `LogDeathEvent(Killer)` - Log death
- `LogAttackEvent(AttackType, Target)` - Log attack
- `LogHealEvent(HealAmount, Healer)` - Log heal

### UEventLogWidget

UMG widget base class for displaying events.

#### Key Functions

- `ToggleVisibility()` - Toggle widget visibility
- `RefreshEventList()` - Refresh the event list
- `ClearEvents()` - Clear all events
- `TogglePause()` - Pause/resume logging
- `ExportEvents(Format)` - Export events

#### Blueprint Implementable Events

- `CreateEventEntry(Event)` - Create a widget for displaying an event entry

## Configuration

The subsystem supports config-based settings. You can override these in `DefaultEngine.ini`:

```ini
[/Script/DebugEventLogViewer.EventLogSubsystem]
MaxStoredEvents=1000
bIsLoggingEnabled=true
```

## Building for Shipping

The plugin is automatically disabled in Shipping builds through preprocessor directives. All logging calls are compiled out when `WITH_DEBUG_EVENT_LOG=0`.

## Example Use Cases

### 1. Combat Debugging

Track all damage events, deaths, and attacks to understand combat balance issues.

### 2. AI Debugging

Log AI state transitions and decision-making to debug AI behavior.

### 3. Performance Analysis

Log system events with timestamps to identify performance bottlenecks.

### 4. Regression Testing

Export event logs before and after changes to compare behavior.

## Integration with Variant_Combat

To automatically log events in the Variant_Combat system:

1. Add `UCombatEventListener` component to `ACombatCharacter` Blueprint
2. In damage handling code, call `LogDamageEvent`
3. In death handling code, call `LogDeathEvent`
4. In attack code, call `LogAttackEvent`

## Troubleshooting

### Events not appearing

- Check that logging is enabled: `Debug.SetLoggingEnabled true`
- Verify the subsystem is initialized (should happen automatically)
- Check that you're not in a Shipping build

### Widget not showing

- Ensure you've created a Widget Blueprint that inherits from `UEventLogWidget`
- Implement the `CreateEventEntry` event in your widget
- Add the widget to your HUD or viewport

### Export fails

- Check that the Saved/Logs directory exists
- Verify write permissions
- Check UE_LOG output for error messages

## License

This plugin is part of the AntigravityTest project.

## Version

Version 1.0 - Initial release
