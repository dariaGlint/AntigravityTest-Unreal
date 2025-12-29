# Day Night Cycle System - API Reference

## Table of Contents
1. [UTimeManager](#utimemanager)
2. [ADayNightLightingController](#adaynightlightingcontroller)
3. [ETimeOfDay](#etimeofday)
4. [Events](#events)

---

## UTimeManager

Time management component that handles time progression and day/night cycle logic.

### Properties

#### Time Properties

##### `CurrentTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
float CurrentTime;
```
Current time of day in hours (0.0 - 24.0).

##### `CurrentDay`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
int32 CurrentDay;
```
Current day number (starts at 1).

##### `TimeSpeed`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
float TimeSpeed;
```
Time progression multiplier. 1.0 = real-time, 60.0 = 1 real minute = 1 game hour.

##### `bPaused`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
bool bPaused;
```
Whether time progression is paused.

##### `DayDurationInSeconds`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
float DayDurationInSeconds;
```
Duration of a full game day in real-time seconds. Default: 1200.0 (20 minutes).

#### Time Period Thresholds

##### `DawnStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float DawnStartTime;
```
Hour when dawn begins. Default: 5.0.

##### `MorningStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float MorningStartTime;
```
Hour when morning begins. Default: 7.0.

##### `NoonStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float NoonStartTime;
```
Hour when noon begins. Default: 11.0.

##### `AfternoonStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float AfternoonStartTime;
```
Hour when afternoon begins. Default: 14.0.

##### `DuskStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float DuskStartTime;
```
Hour when dusk begins. Default: 17.0.

##### `NightStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float NightStartTime;
```
Hour when night begins. Default: 19.0.

##### `MidnightStartTime`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
float MidnightStartTime;
```
Hour when midnight period begins. Default: 23.0.

### Functions

#### `SetTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Time")
void SetTime(float NewTime);
```
Set the current time of day.

**Parameters:**
- `NewTime`: Time in hours (0.0 - 24.0)

**Example:**
```cpp
TimeManager->SetTime(18.5f); // Set to 6:30 PM
```

#### `SetDay`
```cpp
UFUNCTION(BlueprintCallable, Category = "Time")
void SetDay(int32 NewDay);
```
Set the current day number.

**Parameters:**
- `NewDay`: Day number

**Example:**
```cpp
TimeManager->SetDay(10); // Set to day 10
```

#### `SetTimeSpeed`
```cpp
UFUNCTION(BlueprintCallable, Category = "Time")
void SetTimeSpeed(float NewSpeed);
```
Change time progression speed.

**Parameters:**
- `NewSpeed`: Speed multiplier (must be >= 0)

**Example:**
```cpp
TimeManager->SetTimeSpeed(2.0f); // 2x speed
```

#### `PauseTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Time")
void PauseTime();
```
Pause time progression.

**Example:**
```cpp
TimeManager->PauseTime();
```

#### `ResumeTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Time")
void ResumeTime();
```
Resume time progression after pause.

**Example:**
```cpp
TimeManager->ResumeTime();
```

#### `TogglePause`
```cpp
UFUNCTION(BlueprintCallable, Category = "Time")
void TogglePause();
```
Toggle pause state.

**Example:**
```cpp
TimeManager->TogglePause();
```

#### `GetCurrentTimeOfDay`
```cpp
UFUNCTION(BlueprintPure, Category = "Time")
ETimeOfDay GetCurrentTimeOfDay() const;
```
Get the current time period.

**Returns:** ETimeOfDay enum value

**Example:**
```cpp
ETimeOfDay TimeOfDay = TimeManager->GetCurrentTimeOfDay();
if (TimeOfDay == ETimeOfDay::Night)
{
    // Do night-time logic
}
```

#### `GetTimeOfDayString`
```cpp
UFUNCTION(BlueprintPure, Category = "Time")
FString GetTimeOfDayString() const;
```
Get current time period as a string.

**Returns:** String representation ("Dawn", "Morning", etc.)

**Example:**
```cpp
FString TimeString = TimeManager->GetTimeOfDayString();
UE_LOG(LogTemp, Warning, TEXT("Current time: %s"), *TimeString);
```

#### `GetNormalizedTime`
```cpp
UFUNCTION(BlueprintPure, Category = "Time")
float GetNormalizedTime() const;
```
Get current time normalized to 0.0 - 1.0 range.

**Returns:** Normalized time value

**Example:**
```cpp
float NormalizedTime = TimeManager->GetNormalizedTime();
// Use for curve lookups
```

### Events

#### `OnTimeChanged`
```cpp
UPROPERTY(BlueprintAssignable, Category = "Time Events")
FOnTimeChanged OnTimeChanged;
```
Fires when time changes. Updates every frame when time is progressing.

**Delegate Signature:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, float, CurrentTime);
```

#### `OnDayChanged`
```cpp
UPROPERTY(BlueprintAssignable, Category = "Time Events")
FOnDayChanged OnDayChanged;
```
Fires when day number changes (midnight transition).

**Delegate Signature:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, int32, CurrentDay);
```

#### `OnTimeOfDayChanged`
```cpp
UPROPERTY(BlueprintAssignable, Category = "Time Events")
FOnTimeOfDayChanged OnTimeOfDayChanged;
```
Fires when entering a new time period (Dawn, Morning, etc.).

**Delegate Signature:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, FString, TimeOfDay);
```

---

## ADayNightLightingController

Actor that controls lighting based on time of day.

### Properties

#### Time Reference

##### `TimeManager`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
UTimeManager* TimeManager;
```
Reference to the time manager component (created automatically).

#### Light References

##### `SunLight`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
ADirectionalLight* SunLight;
```
Directional light representing the sun. Auto-detected if not set.

##### `SkyLightActor`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
ASkyLight* SkyLightActor;
```
Sky light for ambient lighting. Auto-detected if not set.

#### Sun Settings

##### `SunriseAngle`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
float SunriseAngle;
```
Sun pitch angle at sunrise. Default: -90.0.

##### `SunsetAngle`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
float SunsetAngle;
```
Sun pitch angle at sunset. Default: 90.0.

##### `SunIntensityCurve`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
UCurveFloat* SunIntensityCurve;
```
Optional curve for sun intensity over normalized time (0-1).

##### `SunColorCurve`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
UCurveLinearColor* SunColorCurve;
```
Optional curve for sun color over normalized time (0-1).

#### Sky Settings

##### `SkyIntensityCurve`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Settings")
UCurveFloat* SkyIntensityCurve;
```
Optional curve for sky light intensity over normalized time (0-1).

#### Update Settings

##### `bAutoUpdateLighting`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
bool bAutoUpdateLighting;
```
Whether to automatically update lighting. Default: true.

##### `UpdateInterval`
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
float UpdateInterval;
```
Update interval in seconds. 0 = every frame. Default: 0.0.

### Functions

#### `UpdateLighting`
```cpp
UFUNCTION(BlueprintCallable, Category = "Lighting")
void UpdateLighting();
```
Manually update all lighting based on current time.

**Example:**
```cpp
LightingController->UpdateLighting();
```

#### `SetSunRotationFromTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Lighting")
void SetSunRotationFromTime(float Time);
```
Update sun rotation for specific time.

**Parameters:**
- `Time`: Time in hours (0.0 - 24.0)

**Example:**
```cpp
LightingController->SetSunRotationFromTime(12.0f); // Noon position
```

#### `SetSunIntensityFromTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Lighting")
void SetSunIntensityFromTime(float Time);
```
Update sun intensity for specific time.

**Parameters:**
- `Time`: Time in hours (0.0 - 24.0)

**Example:**
```cpp
LightingController->SetSunIntensityFromTime(18.0f); // Sunset intensity
```

#### `SetSunColorFromTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Lighting")
void SetSunColorFromTime(float Time);
```
Update sun color for specific time.

**Parameters:**
- `Time`: Time in hours (0.0 - 24.0)

**Example:**
```cpp
LightingController->SetSunColorFromTime(6.0f); // Sunrise color
```

#### `SetSkyIntensityFromTime`
```cpp
UFUNCTION(BlueprintCallable, Category = "Lighting")
void SetSkyIntensityFromTime(float Time);
```
Update sky light intensity for specific time.

**Parameters:**
- `Time`: Time in hours (0.0 - 24.0)

**Example:**
```cpp
LightingController->SetSkyIntensityFromTime(0.0f); // Midnight intensity
```

---

## ETimeOfDay

Enumeration of time periods throughout the day.

```cpp
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
    Dawn UMETA(DisplayName = "Dawn"),
    Morning UMETA(DisplayName = "Morning"),
    Noon UMETA(DisplayName = "Noon"),
    Afternoon UMETA(DisplayName = "Afternoon"),
    Dusk UMETA(DisplayName = "Dusk"),
    Night UMETA(DisplayName = "Night"),
    Midnight UMETA(DisplayName = "Midnight")
};
```

### Values

- **Dawn**: Early morning twilight (5:00 - 7:00)
- **Morning**: Morning hours (7:00 - 11:00)
- **Noon**: Midday (11:00 - 14:00)
- **Afternoon**: Afternoon hours (14:00 - 17:00)
- **Dusk**: Evening twilight (17:00 - 19:00)
- **Night**: Night hours (19:00 - 23:00)
- **Midnight**: Late night (23:00 - 5:00)

---

## Events

### Event Delegates

All events are dynamic multicast delegates that can be bound in both C++ and Blueprints.

### Binding in C++

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    if (TimeManager)
    {
        TimeManager->OnTimeChanged.AddDynamic(this, &AMyActor::HandleTimeChanged);
        TimeManager->OnDayChanged.AddDynamic(this, &AMyActor::HandleDayChanged);
        TimeManager->OnTimeOfDayChanged.AddDynamic(this, &AMyActor::HandleTimeOfDayChanged);
    }
}

void AMyActor::HandleTimeChanged(float NewTime)
{
    // Handle time change
}

void AMyActor::HandleDayChanged(int32 NewDay)
{
    // Handle day change
}

void AMyActor::HandleTimeOfDayChanged(FString TimeOfDay)
{
    // Handle time of day change
}
```

### Binding in Blueprints

1. Get TimeManager reference
2. Drag from TimeManager and search for "Event"
3. Select the event to bind (e.g., "Event On Time Changed")
4. Connect to your custom event or function

---

## Usage Examples

### Example 1: Dynamic Enemy Difficulty

```cpp
void AEnemyManager::BeginPlay()
{
    Super::BeginPlay();

    if (LightingController && LightingController->TimeManager)
    {
        LightingController->TimeManager->OnTimeOfDayChanged.AddDynamic(
            this, &AEnemyManager::OnTimeOfDayChanged
        );
    }
}

void AEnemyManager::OnTimeOfDayChanged(FString TimeOfDay)
{
    if (TimeOfDay == "Night" || TimeOfDay == "Midnight")
    {
        EnemyDamageMultiplier = 1.5f;
        EnemySpawnRate = 2.0f;
    }
    else
    {
        EnemyDamageMultiplier = 1.0f;
        EnemySpawnRate = 1.0f;
    }
}
```

### Example 2: Time-Accelerated Sleep

```cpp
void APlayerCharacter::Sleep()
{
    if (TimeManager)
    {
        // Save original speed
        float OriginalSpeed = TimeManager->TimeSpeed;

        // Fast-forward to morning
        TimeManager->SetTimeSpeed(360.0f); // Very fast

        // After 3 seconds, restore and wake up
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, OriginalSpeed]()
        {
            TimeManager->SetTime(6.0f); // 6 AM
            TimeManager->SetTimeSpeed(OriginalSpeed);
            OnWakeUp();
        }, 3.0f, false);
    }
}
```

### Example 3: Custom Curve-Based Fog Density

```cpp
void AEnvironmentManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (TimeManager && FogDensityCurve)
    {
        float NormalizedTime = TimeManager->GetNormalizedTime();
        float FogDensity = FogDensityCurve->GetFloatValue(NormalizedTime);

        // Apply to exponential height fog
        if (ExponentialHeightFog)
        {
            ExponentialHeightFog->GetComponent()->SetFogDensity(FogDensity);
        }
    }
}
```
