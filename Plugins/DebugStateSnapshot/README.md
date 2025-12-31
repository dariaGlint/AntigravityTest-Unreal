# DebugStateSnapshot Plugin

Game state snapshot system for debugging and testing. This plugin allows you to save and restore game state at any point for bug reproduction and test scenarios.

## Features

- **Quick Save/Load**: Press F5 to quick save, F9 to quick load
- **Multiple Slots**: Save and manage multiple snapshots
- **Partial Restoration**: Choose which parts of the state to restore (player, enemies, world)
- **File Export/Import**: Share snapshots between developers
- **UI Manager**: Visual interface for managing snapshots
- **Development Only**: Automatically disabled in Shipping builds

## Installation

The plugin is already included in the project. To use it:

1. Enable the plugin in your `.uproject` file (if not already enabled)
2. Add the plugin to your module's dependencies in `YourModule.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "DebugStateSnapshot"
});
```

## Usage

### Quick Save/Load

In your player character or game mode:

1. Add the `USnapshotInputHandler` component
2. Configure the input actions and mapping context in the component properties
3. Optionally set the snapshot manager widget class

### Programmatic Usage

```cpp
#include "StateSnapshotSubsystem.h"

// Get the subsystem
UStateSnapshotSubsystem* SnapshotSystem = GetGameInstance()->GetSubsystem<UStateSnapshotSubsystem>();

// Save a snapshot
FSnapshotSaveOptions SaveOptions;
SaveOptions.bGenerateThumbnail = true;
SnapshotSystem->SaveSnapshot("MySnapshot", SaveOptions);

// Load a snapshot
FSnapshotLoadOptions LoadOptions;
SnapshotSystem->LoadSnapshot("MySnapshot", LoadOptions);

// Quick save/load
SnapshotSystem->QuickSave();
SnapshotSystem->QuickLoad();
```

### Making Objects Snapshotable

Implement the `ISnapshotable` interface on your actors:

```cpp
#include "ISnapshotable.h"

UCLASS()
class AMyCharacter : public ACharacter, public ISnapshotable
{
    GENERATED_BODY()

public:
    // Implement ISnapshotable interface
    virtual bool SaveState_Implementation(TArray<uint8>& OutData) override;
    virtual bool RestoreState_Implementation(const TArray<uint8>& InData) override;
    virtual FString GetSnapshotID_Implementation() const override;
};
```

Example implementation:

```cpp
bool AMyCharacter::SaveState_Implementation(TArray<uint8>& OutData)
{
    FMemoryWriter Writer(OutData);

    FVector Location = GetActorLocation();
    FRotator Rotation = GetActorRotation();
    float Health = CurrentHealth;

    Writer << Location;
    Writer << Rotation;
    Writer << Health;

    return true;
}

bool AMyCharacter::RestoreState_Implementation(const TArray<uint8>& InData)
{
    FMemoryReader Reader(const_cast<TArray<uint8>&>(InData));

    FVector Location;
    FRotator Rotation;
    float Health;

    Reader << Location;
    Reader << Rotation;
    Reader << Health;

    SetActorLocationAndRotation(Location, Rotation);
    CurrentHealth = Health;

    return true;
}

FString AMyCharacter::GetSnapshotID_Implementation() const
{
    return GetName(); // Or use a unique persistent ID
}
```

## Default Keybindings

- **F5**: Quick Save
- **F9**: Quick Load
- **F8**: Toggle Snapshot Manager UI (configure in your project)

## Snapshot Files

Snapshots are saved to:
```
YourProject/Saved/Snapshots/
```

Each snapshot is saved as a `.snapshot` file containing:
- Snapshot metadata (ID, name, timestamp, level)
- Serialized player state
- Serialized enemy/AI states
- Serialized world state
- Optional thumbnail

## Architecture

### Core Components

- **UStateSnapshotSubsystem**: Main subsystem managing snapshot operations
- **ISnapshotable**: Interface for objects that can be saved/restored
- **FStateSnapshot**: Data structure representing a saved state
- **USnapshotManagerWidget**: UI for managing snapshots
- **USnapshotInputHandler**: Component for handling input bindings

### Build Configuration

The plugin uses conditional compilation to ensure it's only active in Development and Editor builds:

```cpp
#if WITH_DEBUG_STATE_SNAPSHOT
    // Snapshot code here
#endif
```

## Integration with Other Plugins

### DebugConsoleCommands

If DebugConsoleCommands plugin is available, you can use console commands:

```
SaveState <SlotName>
LoadState <SlotName>
ListStates
DeleteState <SlotName>
```

### DebugEventLogViewer

Snapshot save/restore events are automatically logged to the event viewer if available.

## Limitations

- Level transitions are not fully implemented yet
- Thumbnail generation is not yet implemented
- Export/Import uses a simple binary format (JSON export is TODO)
- Only works in Development and Editor builds

## Future Enhancements

- [ ] Level transition support
- [ ] Screenshot thumbnails
- [ ] JSON export format
- [ ] Automatic periodic snapshots
- [ ] Snapshot comparison tool
- [ ] Replay functionality

## License

Copyright Epic Games, Inc. All Rights Reserved.
