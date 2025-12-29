# Day Night Cycle System - Examples

This document provides practical examples for common use cases of the Day Night Cycle System.

## Table of Contents
1. [Basic Time Control](#basic-time-control)
2. [Gameplay Integration](#gameplay-integration)
3. [Visual Effects](#visual-effects)
4. [Advanced Scenarios](#advanced-scenarios)

---

## Basic Time Control

### Example 1: Simple Day/Night Cycle Setup

**Scenario:** Create a basic 10-minute day/night cycle.

**Steps:**
1. Place `DayNightLightingController` in level
2. Configure TimeManager:
   - `DayDurationInSeconds`: 600.0
   - `CurrentTime`: 6.0 (start at sunrise)
   - `TimeSpeed`: 1.0

**Blueprint:**
```
Event BeginPlay
→ Get DayNightLightingController
→ Get TimeManager
→ Set Day Duration In Seconds (600.0)
→ Set Time (6.0)
```

### Example 2: Pause Time on Menu

**Scenario:** Pause time when player opens menu.

**Blueprint:**
```
Event On Menu Opened
→ Get TimeManager
→ Pause Time

Event On Menu Closed
→ Get TimeManager
→ Resume Time
```

### Example 3: Skip to Specific Time

**Scenario:** Skip to sunset with smooth transition.

**Blueprint:**
```
Event Skip To Sunset
→ Get TimeManager
→ Set Time Speed (60.0)  // Speed up
→ Delay (2.0 seconds)
→ Set Time (18.0)        // Force to sunset
→ Set Time Speed (1.0)   // Normal speed
```

---

## Gameplay Integration

### Example 4: Time-Gated Quest

**Scenario:** Quest only available during daytime.

**Blueprint:**
```
Event On Quest Interact
→ Get TimeManager
→ Get Current Time Of Day
→ Branch (TimeOfDay == Morning OR TimeOfDay == Noon OR TimeOfDay == Afternoon)
  → True: Start Quest
  → False: Show Message "Come back during daylight"
```

### Example 5: Vampire Enemy AI

**Scenario:** Vampires take damage in sunlight.

**C++ Implementation:**
```cpp
void AVampireEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Find and bind to time manager
    if (TimeManager)
    {
        TimeManager->OnTimeOfDayChanged.AddDynamic(
            this, &AVampireEnemy::OnTimeOfDayChanged
        );

        // Check initial time
        CheckSunlightDamage();
    }
}

void AVampireEnemy::OnTimeOfDayChanged(FString TimeOfDay)
{
    CheckSunlightDamage();
}

void AVampireEnemy::CheckSunlightDamage()
{
    ETimeOfDay CurrentTimeOfDay = TimeManager->GetCurrentTimeOfDay();

    bool bInSunlight = (CurrentTimeOfDay == ETimeOfDay::Morning ||
                        CurrentTimeOfDay == ETimeOfDay::Noon ||
                        CurrentTimeOfDay == ETimeOfDay::Afternoon);

    if (bInSunlight)
    {
        // Take damage over time
        GetWorld()->GetTimerManager().SetTimer(
            SunlightDamageTimer,
            this,
            &AVampireEnemy::TakeSunlightDamage,
            1.0f,
            true
        );
    }
    else
    {
        // Stop taking damage
        GetWorld()->GetTimerManager().ClearTimer(SunlightDamageTimer);
    }
}

void AVampireEnemy::TakeSunlightDamage()
{
    TakeDamage(10.0f, FDamageEvent(), nullptr, nullptr);
}
```

### Example 6: Shop Opening Hours

**Scenario:** Shop opens 9 AM - 5 PM.

**Blueprint:**
```
Event BeginPlay
→ Get TimeManager
→ Bind Event to OnTimeChanged
→ Custom Event: Update Shop Status

Custom Event: Update Shop Status
→ Get TimeManager → Get Current Time
→ Branch (Time >= 9.0 AND Time <= 17.0)
  → True:
    → Set Shop Open (true)
    → Enable Shop Interaction Widget
  → False:
    → Set Shop Open (false)
    → Disable Shop Interaction Widget
```

### Example 7: Werewolf Transformation

**Scenario:** Player transforms into werewolf at night.

**Blueprint:**
```
Event BeginPlay
→ Get TimeManager
→ Bind Event to OnTimeOfDayChanged
→ Custom Event: Check Transformation

Custom Event: Check Transformation (TimeOfDay: String)
→ Branch (TimeOfDay == "Night" OR TimeOfDay == "Midnight")
  → True: Transform To Werewolf
  → False: Transform To Human
```

---

## Visual Effects

### Example 8: Dynamic Fog Based on Time

**Scenario:** Fog is denser at dawn and dusk.

**C++ Implementation:**
```cpp
void AEnvironmentController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TimeManager || !ExponentialHeightFog)
        return;

    float CurrentTime = TimeManager->CurrentTime;
    float FogDensity = 0.02f; // Default

    // Dense fog at dawn and dusk
    if (CurrentTime >= 5.0f && CurrentTime <= 7.0f)
    {
        // Dawn fog
        float Alpha = (CurrentTime - 5.0f) / 2.0f;
        FogDensity = FMath::Lerp(0.05f, 0.02f, Alpha);
    }
    else if (CurrentTime >= 17.0f && CurrentTime <= 19.0f)
    {
        // Dusk fog
        float Alpha = (CurrentTime - 17.0f) / 2.0f;
        FogDensity = FMath::Lerp(0.02f, 0.05f, Alpha);
    }

    ExponentialHeightFog->GetComponent()->SetFogDensity(FogDensity);
}
```

### Example 9: Star Visibility

**Scenario:** Stars appear at night, fade during day.

**Blueprint:**
```
Event Tick
→ Get TimeManager → Get Current Time
→ Branch (Time >= 19.0 OR Time <= 5.0)
  → True:
    → Set Star Particle System Active (true)
    → Calculate Star Opacity (based on time)
    → Set Particle Opacity
  → False:
    → Set Star Particle System Active (false)
```

### Example 10: Firefly Spawning

**Scenario:** Fireflies appear at dusk.

**Blueprint:**
```
Event BeginPlay
→ Get TimeManager
→ Bind Event to OnTimeOfDayChanged
→ Custom Event: Handle Fireflies

Custom Event: Handle Fireflies (TimeOfDay: String)
→ Branch (TimeOfDay == "Dusk")
  → True: Spawn Firefly Particles
  → False: Despawn Firefly Particles
```

---

## Advanced Scenarios

### Example 11: Multi-Zone Time System

**Scenario:** Different time zones in different regions.

**C++ Implementation:**
```cpp
UCLASS()
class ATimeZone : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeOffset; // Hours offset from main time

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTimeManager* LocalTimeManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTimeManager* GlobalTimeManager;

    void Tick(float DeltaTime) override
    {
        if (LocalTimeManager && GlobalTimeManager)
        {
            float GlobalTime = GlobalTimeManager->CurrentTime;
            float LocalTime = GlobalTime + TimeOffset;

            if (LocalTime >= 24.0f)
                LocalTime -= 24.0f;
            else if (LocalTime < 0.0f)
                LocalTime += 24.0f;

            LocalTimeManager->SetTime(LocalTime);
        }
    }
};
```

### Example 12: Time Freeze Ability

**Scenario:** Player can temporarily freeze time.

**Blueprint:**
```
Event Activate Time Freeze
→ Get TimeManager
→ Get Current TimeSpeed (store as OriginalSpeed)
→ Set Time Speed (0.0)
→ Set Timer by Event (Duration: 5.0)
  → Timer Event: Restore Time Speed (OriginalSpeed)

Event Deactivate Time Freeze
→ Get TimeManager
→ Set Time Speed (OriginalSpeed)
```

### Example 13: Seasonal System

**Scenario:** Seasons change every 30 game days.

**C++ Implementation:**
```cpp
UCLASS()
class USeasonManager : public UActorComponent
{
    GENERATED_BODY()

public:
    enum class ESeason : uint8
    {
        Spring,
        Summer,
        Autumn,
        Winter
    };

    UPROPERTY()
    UTimeManager* TimeManager;

    ESeason CurrentSeason;
    int32 DaysPerSeason = 30;

    void BeginPlay() override
    {
        Super::BeginPlay();

        if (TimeManager)
        {
            TimeManager->OnDayChanged.AddDynamic(this, &USeasonManager::OnDayChanged);
        }
    }

    UFUNCTION()
    void OnDayChanged(int32 NewDay)
    {
        int32 SeasonIndex = (NewDay / DaysPerSeason) % 4;
        ESeason NewSeason = static_cast<ESeason>(SeasonIndex);

        if (NewSeason != CurrentSeason)
        {
            CurrentSeason = NewSeason;
            OnSeasonChanged();
        }
    }

    void OnSeasonChanged()
    {
        // Update environment based on season
        switch (CurrentSeason)
        {
        case ESeason::Spring:
            // Add flowers, green grass
            break;
        case ESeason::Summer:
            // Bright sun, clear skies
            break;
        case ESeason::Autumn:
            // Orange leaves, rain
            break;
        case ESeason::Winter:
            // Snow, ice
            break;
        }
    }
};
```

### Example 14: Time Travel Mechanic

**Scenario:** Travel back/forward in time.

**Blueprint:**
```
Event Travel To Past
→ Get TimeManager
→ Get Current Time (Store as ReturnTime)
→ Get Current Day (Store as ReturnDay)
→ Set Time (ReturnTime - 12.0)  // 12 hours ago
→ Set Time Speed (0.0)  // Freeze in past
→ Store Return Point

Event Return To Present
→ Get TimeManager
→ Set Time (ReturnTime)
→ Set Day (ReturnDay)
→ Set Time Speed (1.0)
```

### Example 15: Realistic Sunrise/Sunset Times

**Scenario:** Sunrise/sunset times change based on season.

**C++ Implementation:**
```cpp
void ARealisticTimeManager::UpdateSeasonalSunTimes()
{
    // Calculate day of year (0-365)
    int32 DayOfYear = (CurrentDay % 365);

    // Calculate sun times based on sine wave
    // Longest day at day 172 (summer), shortest at day 355 (winter)
    float DayProgress = (DayOfYear / 365.0f) * 2.0f * PI;
    float SunlightHours = 12.0f + (4.0f * FMath::Sin(DayProgress));

    // Calculate sunrise and sunset
    float SunriseTime = 12.0f - (SunlightHours / 2.0f);
    float SunsetTime = 12.0f + (SunlightHours / 2.0f);

    // Update time manager thresholds
    if (TimeManager)
    {
        TimeManager->DawnStartTime = SunriseTime - 1.0f;
        TimeManager->MorningStartTime = SunriseTime;
        TimeManager->DuskStartTime = SunsetTime - 1.0f;
        TimeManager->NightStartTime = SunsetTime;
    }

    UE_LOG(LogTemp, Log, TEXT("Day %d: Sunrise %.1f, Sunset %.1f"),
           DayOfYear, SunriseTime, SunsetTime);
}
```

### Example 16: Performance-Optimized Time System

**Scenario:** Update lighting less frequently for better performance.

**Blueprint:**
```
// On DayNightLightingController:
- Set Auto Update Lighting: true
- Set Update Interval: 0.5  // Update every 0.5 seconds

// For very smooth transitions, use curves:
1. Create SunIntensityCurve
2. Create SunColorCurve
3. Assign to controller
4. Set Update Interval: 1.0  // Can update less frequently with curves
```

---

## Complete Integration Example

### Example 17: Full Survival Game Integration

**Scenario:** Complete day/night system for survival game.

**Features:**
- Zombies spawn at night
- Cold temperature at night
- Shop hours
- Time-based hunger/thirst
- Solar panels work during day

**C++ Implementation:**
```cpp
UCLASS()
class ASurvivalGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    UPROPERTY()
    UTimeManager* TimeManager;

    UPROPERTY()
    ADayNightLightingController* LightingController;

    void BeginPlay() override
    {
        Super::BeginPlay();

        // Find or spawn lighting controller
        LightingController = GetWorld()->SpawnActor<ADayNightLightingController>();
        TimeManager = LightingController->TimeManager;

        // Configure time
        TimeManager->DayDurationInSeconds = 1200.0f; // 20 min days
        TimeManager->CurrentTime = 6.0f; // Start at sunrise

        // Bind events
        TimeManager->OnTimeOfDayChanged.AddDynamic(
            this, &ASurvivalGameMode::OnTimeOfDayChanged
        );
        TimeManager->OnTimeChanged.AddDynamic(
            this, &ASurvivalGameMode::OnTimeChanged
        );
    }

    UFUNCTION()
    void OnTimeOfDayChanged(FString TimeOfDay)
    {
        if (TimeOfDay == "Night" || TimeOfDay == "Midnight")
        {
            // Increase zombie spawns
            SetZombieSpawnRate(2.0f);

            // Lower temperature
            SetWorldTemperature(-5.0f);

            // Close shops
            CloseAllShops();
        }
        else if (TimeOfDay == "Morning")
        {
            // Normal spawns
            SetZombieSpawnRate(1.0f);

            // Normal temperature
            SetWorldTemperature(20.0f);

            // Open shops
            OpenAllShops();
        }
    }

    UFUNCTION()
    void OnTimeChanged(float CurrentTime)
    {
        // Update all players
        for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
        {
            if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(PC->GetPawn()))
            {
                Character->UpdateTimeBasedEffects(CurrentTime);
            }
        }
    }

    void SetZombieSpawnRate(float Multiplier)
    {
        // Implement zombie spawning logic
    }

    void SetWorldTemperature(float Temp)
    {
        // Update temperature for player effects
    }

    void CloseAllShops()
    {
        // Disable shop interactions
    }

    void OpenAllShops()
    {
        // Enable shop interactions
    }
};
```

**Player Character:**
```cpp
void ASurvivalCharacter::UpdateTimeBasedEffects(float CurrentTime)
{
    // Hunger/thirst based on time
    float HoursSinceLastMeal = CurrentTime - LastMealTime;
    Hunger += HoursSinceLastMeal * HungerRate;
    Thirst += HoursSinceLastMeal * ThirstRate;

    // Solar panels work during day (6 AM - 6 PM)
    if (CurrentTime >= 6.0f && CurrentTime <= 18.0f)
    {
        ChargeBattery(SolarChargeRate);
    }

    // Cold at night affects health
    if (CurrentTime < 6.0f || CurrentTime > 20.0f)
    {
        if (!bNearHeatSource)
        {
            Temperature -= ColdRate;
            if (Temperature < 15.0f)
            {
                TakeDamage(ColdDamage, FDamageEvent(), nullptr, nullptr);
            }
        }
    }
}
```

---

## Tips for Implementation

1. **Event-Driven vs Tick**: Use events (OnTimeOfDayChanged) for infrequent checks, use Tick for smooth visual updates
2. **Performance**: Set UpdateInterval > 0 for better performance
3. **Network**: Replicate time from server in multiplayer
4. **Curves**: Use curves for complex lighting transitions
5. **Modular**: Keep time logic separate from game logic for reusability
