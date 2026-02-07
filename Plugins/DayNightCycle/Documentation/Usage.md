# Day Night Cycle System - Usage Guide

## Table of Contents
1. [Basic Setup](#basic-setup)
2. [Time Management](#time-management)
3. [Lighting Control](#lighting-control)
4. [Blueprint Integration](#blueprint-integration)
5. [Common Scenarios](#common-scenarios)

## Basic Setup

### Step 1: Enable Plugin
1. Open your Unreal Engine project
2. Go to **Edit > Plugins**
3. Search for "Day Night Cycle"
4. Check the box to enable it
5. Restart the editor when prompted

### Step 2: Add Lighting Controller
1. Open your level
2. In the **Place Actors** panel, search for "DayNightLightingController"
3. Drag it into your level
4. The controller will automatically find existing lights

### Step 3: Verify Light References
1. Select the DayNightLightingController in the level
2. In the Details panel, check the **Lighting** section
3. Verify that **Sun Light** and **Sky Light Actor** are assigned
4. If not assigned, manually select your DirectionalLight and SkyLight

## Time Management

### Understanding Time System

The time system represents a 24-hour day:
- `0.0` = Midnight (00:00)
- `6.0` = 6 AM
- `12.0` = Noon (12:00)
- `18.0` = 6 PM
- `24.0` = Midnight (loops to 0.0)

### Time Periods

The system automatically detects these time periods:
- **Midnight**: 23:00 - 05:00
- **Dawn**: 05:00 - 07:00
- **Morning**: 07:00 - 11:00
- **Noon**: 11:00 - 14:00
- **Afternoon**: 14:00 - 17:00
- **Dusk**: 17:00 - 19:00
- **Night**: 19:00 - 23:00

### Configuring Time Speed

Time speed determines how fast game time progresses:

```
TimeSpeed = 1.0   → Real-time (not recommended for games)
TimeSpeed = 60.0  → 1 real minute = 1 game hour
TimeSpeed = 120.0 → 1 real minute = 2 game hours
```

**Example Configuration for 20-minute day:**
- DayDurationInSeconds: 1200 (20 minutes)
- TimeSpeed: 1.0

**Example Configuration for fast gameplay:**
- DayDurationInSeconds: 300 (5 minutes)
- TimeSpeed: 1.0

## Lighting Control

### Automatic Lighting Updates

By default, lighting updates automatically:
- **bAutoUpdateLighting**: true
- **UpdateInterval**: 0.0 (every frame)

For performance optimization, set UpdateInterval to 0.1 (update every 0.1 seconds).

### Manual Lighting Control

To control lighting manually:
1. Set `bAutoUpdateLighting` to false
2. Call `UpdateLighting()` when needed

### Sun Rotation

The sun rotates based on time:
- **SunriseAngle**: -90.0 (sun rises from below horizon)
- **SunsetAngle**: 90.0 (sun sets below horizon)

Adjust these values to change sun path.

### Lighting Curves

For advanced control, create lighting curves:

#### Creating Sun Intensity Curve
1. Right-click in Content Browser
2. Select **Miscellaneous > Curve**
3. Choose **Curve Float**
4. Name it "SunIntensityCurve"
5. Open and add keys:
   - Time 0.0 (midnight): Value 0.1
   - Time 0.25 (6 AM): Value 0.5
   - Time 0.5 (noon): Value 10.0
   - Time 0.75 (6 PM): Value 0.5
   - Time 1.0 (midnight): Value 0.1
6. Assign to LightingController's **SunIntensityCurve**

#### Creating Sun Color Curve
1. Create **Curve Linear Color**
2. Name it "SunColorCurve"
3. Add keys with colors:
   - Time 0.0: Blue-ish (night)
   - Time 0.25: Orange (sunrise)
   - Time 0.5: White-yellow (day)
   - Time 0.75: Orange-red (sunset)
   - Time 1.0: Blue-ish (night)
4. Assign to LightingController's **SunColorCurve**

## Blueprint Integration

### Getting Time Information

**Get Current Time:**
```
Get Time Manager → Get Current Time
```

**Get Time of Day:**
```
Get Time Manager → Get Current Time Of Day
Returns: Enum (Dawn, Morning, Noon, etc.)
```

**Get Normalized Time (0-1):**
```
Get Time Manager → Get Normalized Time
```

### Controlling Time

**Set Specific Time:**
```
Get Time Manager → Set Time (NewTime: 18.0)
```

**Change Time Speed:**
```
Get Time Manager → Set Time Speed (NewSpeed: 2.0)
```

**Pause/Resume:**
```
Get Time Manager → Pause Time
Get Time Manager → Resume Time
Get Time Manager → Toggle Pause
```

### Event Handling

**Listen for Time Changes:**
```
Event BeginPlay
→ Get Time Manager
→ Bind Event to OnTimeChanged
→ Create Custom Event: Handle Time Changed
```

**Listen for Day Changes:**
```
Event BeginPlay
→ Get Time Manager
→ Bind Event to OnDayChanged
→ Create Custom Event: Handle Day Changed
```

**Listen for Time Period Changes:**
```
Event BeginPlay
→ Get Time Manager
→ Bind Event to OnTimeOfDayChanged
→ Create Custom Event: Handle TimeOfDay Changed
```

## Common Scenarios

### Scenario 1: Time-Based Enemy Spawning

Spawn zombies at night:
```
Event: OnTimeOfDayChanged
→ Branch (TimeOfDay == Night)
  → True: Spawn Zombies
  → False: Despawn Zombies
```

### Scenario 2: Shop Hours

Open shop during business hours:
```
Event: OnTimeChanged
→ Branch (Time >= 9.0 AND Time <= 17.0)
  → True: Enable Shop Interaction
  → False: Disable Shop Interaction
```

### Scenario 3: Dynamic Weather by Time

Different weather in different periods:
```
Event: OnTimeOfDayChanged
→ Switch on Enum (TimeOfDay)
  → Dawn: Set Foggy Weather
  → Morning: Set Clear Weather
  → Noon: Set Sunny Weather
  → Dusk: Set Cloudy Weather
  → Night: Set Clear Night
```

### Scenario 4: Time Trial Challenge

Create a timed challenge:
```
Event: Start Challenge
→ Get Time Manager
→ Set Time Speed (10.0) // Fast time
→ Set Time (6.0) // Start at 6 AM

Event: OnTimeChanged
→ Branch (Time >= 18.0) // 6 PM
  → True: End Challenge
```

### Scenario 5: Cinematic Time Control

Control time for cutscenes:
```
Event: Start Cutscene
→ Get Time Manager → Pause Time
→ Set Time (17.5) // Golden hour
→ Play Cutscene Sequence

Event: End Cutscene
→ Get Time Manager → Resume Time
```

### Scenario 6: Sleep System

Fast-forward time when player sleeps:
```
Event: Player Sleep
→ Get Time Manager
→ Set Time Speed (360.0) // Very fast
→ Delay (2 seconds)
→ Set Time Speed (1.0) // Back to normal
→ Set Time (6.0) // Wake up at 6 AM
```

## Performance Tips

1. **Update Interval**: Set to 0.1 or higher for better performance
2. **Tick Optimization**: Disable tick on TimeManager if using events only
3. **Light Mobility**: Set lights to Movable for dynamic updates
4. **Curve Complexity**: Keep curves simple with fewer keys

## Troubleshooting

**Lighting not updating:**
- Verify lights are set to Movable
- Check bAutoUpdateLighting is true
- Verify light references are assigned

**Time not progressing:**
- Check bPaused is false
- Verify TimeSpeed > 0
- Ensure TimeManager tick is enabled

**Events not firing:**
- Verify event bindings in BeginPlay
- Check TimeManager component is valid
- Ensure actor is in level (not just blueprint)

## Advanced Customization

### Creating Custom Time-Based Logic

Extend the system by creating your own components that listen to time events:

```cpp
UCLASS()
class UMyGameTimeComponent : public UActorComponent
{
    GENERATED_BODY()

    UFUNCTION()
    void OnTimeChanged(float NewTime)
    {
        // Your custom logic
    }
};
```

Bind in BeginPlay:
```cpp
TimeManager->OnTimeChanged.AddDynamic(this, &UMyGameTimeComponent::OnTimeChanged);
```

### Network Replication

For multiplayer games:
1. Make TimeManager properties replicated
2. Control time only on server
3. Replicate CurrentTime and CurrentDay
4. Clients update lighting based on replicated values
