# Day Night Cycle System

A comprehensive day/night cycle system for Unreal Engine 5 with time progression management, dynamic lighting updates, and time acceleration features.

## Features

- **Time Progression Management**: Configurable time flow with customizable day duration
- **Dynamic Lighting Updates**: Automatic sun and sky light updates based on time of day
- **Time Acceleration**: Speed up or slow down time with multiplier support
- **Time Period Detection**: Automatic detection of dawn, morning, noon, afternoon, dusk, night, and midnight
- **Event System**: Blueprint-accessible events for time changes and transitions
- **Pause/Resume**: Full control over time flow

## Components

### TimeManager
A component that manages time progression and triggers time-based events.

**Key Properties:**
- `CurrentTime`: Time of day in hours (0-24)
- `CurrentDay`: Current day number
- `TimeSpeed`: Time progression multiplier
- `DayDurationInSeconds`: Real-time duration for a full game day

**Key Functions:**
- `SetTime()`: Set current time
- `SetTimeSpeed()`: Change time flow speed
- `PauseTime()`/`ResumeTime()`: Control time flow
- `GetCurrentTimeOfDay()`: Get current time period (Dawn, Morning, etc.)

### DayNightLightingController
An actor that controls directional light (sun) and sky light based on time.

**Key Properties:**
- `TimeManager`: Reference to time manager component
- `SunLight`: Directional light to control
- `SkyLightActor`: Sky light to control
- `SunIntensityCurve`: Optional curve for sun intensity over time
- `SunColorCurve`: Optional curve for sun color over time

**Key Functions:**
- `UpdateLighting()`: Manually update all lighting
- `SetSunRotationFromTime()`: Update sun rotation
- `SetSunIntensityFromTime()`: Update sun intensity

## Quick Start

1. **Enable the Plugin**
   - Go to Edit > Plugins
   - Search for "Day Night Cycle System"
   - Enable the plugin and restart the editor

2. **Add to Your Level**
   - Place a `DayNightLightingController` actor in your level
   - The controller will automatically find and control existing Directional Light and Sky Light
   - Alternatively, manually assign lights in the Details panel

3. **Configure Time**
   - Adjust `TimeManager` properties on the controller:
     - `CurrentTime`: Starting time of day
     - `TimeSpeed`: How fast time progresses
     - `DayDurationInSeconds`: How long a day lasts in real seconds

4. **Customize Lighting** (Optional)
   - Create Float Curves for sun intensity
   - Create Linear Color Curves for sun color
   - Assign curves to the lighting controller

## Blueprint Usage

### Accessing Time
```cpp
// Get time manager reference
UTimeManager* TimeManager = LightingController->TimeManager;

// Get current time
float CurrentTime = TimeManager->CurrentTime;

// Get time of day
ETimeOfDay TimeOfDay = TimeManager->GetCurrentTimeOfDay();
```

### Controlling Time
```cpp
// Set time to sunset
TimeManager->SetTime(18.0f);

// Speed up time (2x speed)
TimeManager->SetTimeSpeed(2.0f);

// Pause time
TimeManager->PauseTime();
```

### Listening to Events
Bind to events in Blueprint:
- `OnTimeChanged`: Fires every time update
- `OnDayChanged`: Fires when day number changes
- `OnTimeOfDayChanged`: Fires when entering new time period

## Example Use Cases

1. **Fast Day/Night Cycle**: Set `DayDurationInSeconds` to 300 (5 minutes real time = 1 game day)
2. **Realistic Time**: Set `TimeSpeed` to 1.0 and `DayDurationInSeconds` to 86400 (real-time)
3. **Time-Based Gameplay**: Listen to `OnTimeOfDayChanged` to trigger enemy spawns, shop closures, etc.
4. **Cinematic Control**: Pause time and manually set specific times for cutscenes

## Advanced Features

### Custom Time Periods
Adjust time period thresholds in `TimeManager`:
- `DawnStartTime` (default: 5.0)
- `MorningStartTime` (default: 7.0)
- `NoonStartTime` (default: 11.0)
- `AfternoonStartTime` (default: 14.0)
- `DuskStartTime` (default: 17.0)
- `NightStartTime` (default: 19.0)
- `MidnightStartTime` (default: 23.0)

### Lighting Curves
Create custom curves for smooth transitions:
1. Create a Float Curve asset for intensity
2. Add keys at different time points (0.0 to 1.0 normalized time)
3. Assign to `SunIntensityCurve` or `SkyIntensityCurve`

### Multiple Lighting Controllers
You can have multiple controllers for different lighting setups (indoor vs outdoor, different regions, etc.)

## Technical Details

- **Plugin Category**: World/Environment
- **Engine Compatibility**: Unreal Engine 5
- **Module Type**: Runtime
- **Blueprint Support**: Full
- **Network Support**: Can be extended for multiplayer (time should be controlled by server)

## License

Copyright Epic Games, Inc. All Rights Reserved.
