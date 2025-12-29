// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimeManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, float, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, int32, CurrentDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, FString, TimeOfDay);

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

/**
 * TimeManager - Manages time progression and day/night cycle
 */
UCLASS(ClassGroup=(DayNightCycle), meta=(BlueprintSpawnableComponent))
class DAYNIGHTCYCLE_API UTimeManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimeManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Time of day in hours (0-24)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float CurrentTime;

	// Current day number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 CurrentDay;

	// Time progression speed multiplier (1.0 = real-time, 60.0 = 1 minute real = 1 hour game)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeSpeed;

	// Whether time progression is paused
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	bool bPaused;

	// Duration of a full day in seconds (real-time)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float DayDurationInSeconds;

	// Time thresholds for different times of day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float DawnStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float MorningStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float NoonStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float AfternoonStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float DuskStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float NightStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Periods")
	float MidnightStartTime;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Time Events")
	FOnTimeChanged OnTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Time Events")
	FOnDayChanged OnDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Time Events")
	FOnTimeOfDayChanged OnTimeOfDayChanged;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetTime(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetDay(int32 NewDay);

	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetTimeSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Time")
	void PauseTime();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void ResumeTime();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void TogglePause();

	UFUNCTION(BlueprintPure, Category = "Time")
	ETimeOfDay GetCurrentTimeOfDay() const;

	UFUNCTION(BlueprintPure, Category = "Time")
	FString GetTimeOfDayString() const;

	UFUNCTION(BlueprintPure, Category = "Time")
	float GetNormalizedTime() const;

private:
	ETimeOfDay LastTimeOfDay;

	void UpdateTime(float DeltaTime);
	void CheckTimeOfDayTransition();
};
