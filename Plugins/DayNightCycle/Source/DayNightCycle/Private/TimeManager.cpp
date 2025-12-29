// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeManager.h"

UTimeManager::UTimeManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Default values
	CurrentTime = 12.0f; // Start at noon
	CurrentDay = 1;
	TimeSpeed = 1.0f;
	bPaused = false;
	DayDurationInSeconds = 1200.0f; // 20 minutes real-time = 1 day

	// Time period thresholds
	DawnStartTime = 5.0f;
	MorningStartTime = 7.0f;
	NoonStartTime = 11.0f;
	AfternoonStartTime = 14.0f;
	DuskStartTime = 17.0f;
	NightStartTime = 19.0f;
	MidnightStartTime = 23.0f;

	LastTimeOfDay = ETimeOfDay::Noon;
}

void UTimeManager::BeginPlay()
{
	Super::BeginPlay();
	LastTimeOfDay = GetCurrentTimeOfDay();
}

void UTimeManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bPaused)
	{
		UpdateTime(DeltaTime);
	}
}

void UTimeManager::UpdateTime(float DeltaTime)
{
	// Calculate time progression
	// DayDurationInSeconds is how long a full day takes in real seconds
	// We want to convert DeltaTime (real seconds) into game hours
	float HoursPerSecond = 24.0f / DayDurationInSeconds;
	float TimeIncrement = DeltaTime * HoursPerSecond * TimeSpeed;

	CurrentTime += TimeIncrement;

	// Handle day overflow
	if (CurrentTime >= 24.0f)
	{
		CurrentTime -= 24.0f;
		CurrentDay++;
		OnDayChanged.Broadcast(CurrentDay);
	}
	else if (CurrentTime < 0.0f)
	{
		CurrentTime += 24.0f;
		CurrentDay--;
		OnDayChanged.Broadcast(CurrentDay);
	}

	OnTimeChanged.Broadcast(CurrentTime);
	CheckTimeOfDayTransition();
}

void UTimeManager::CheckTimeOfDayTransition()
{
	ETimeOfDay NewTimeOfDay = GetCurrentTimeOfDay();

	if (NewTimeOfDay != LastTimeOfDay)
	{
		LastTimeOfDay = NewTimeOfDay;
		OnTimeOfDayChanged.Broadcast(GetTimeOfDayString());
	}
}

void UTimeManager::SetTime(float NewTime)
{
	CurrentTime = FMath::Clamp(NewTime, 0.0f, 24.0f);
	OnTimeChanged.Broadcast(CurrentTime);
	CheckTimeOfDayTransition();
}

void UTimeManager::SetDay(int32 NewDay)
{
	CurrentDay = NewDay;
	OnDayChanged.Broadcast(CurrentDay);
}

void UTimeManager::SetTimeSpeed(float NewSpeed)
{
	TimeSpeed = FMath::Max(0.0f, NewSpeed);
}

void UTimeManager::PauseTime()
{
	bPaused = true;
}

void UTimeManager::ResumeTime()
{
	bPaused = false;
}

void UTimeManager::TogglePause()
{
	bPaused = !bPaused;
}

ETimeOfDay UTimeManager::GetCurrentTimeOfDay() const
{
	if (CurrentTime >= MidnightStartTime || CurrentTime < DawnStartTime)
	{
		return ETimeOfDay::Midnight;
	}
	else if (CurrentTime >= DawnStartTime && CurrentTime < MorningStartTime)
	{
		return ETimeOfDay::Dawn;
	}
	else if (CurrentTime >= MorningStartTime && CurrentTime < NoonStartTime)
	{
		return ETimeOfDay::Morning;
	}
	else if (CurrentTime >= NoonStartTime && CurrentTime < AfternoonStartTime)
	{
		return ETimeOfDay::Noon;
	}
	else if (CurrentTime >= AfternoonStartTime && CurrentTime < DuskStartTime)
	{
		return ETimeOfDay::Afternoon;
	}
	else if (CurrentTime >= DuskStartTime && CurrentTime < NightStartTime)
	{
		return ETimeOfDay::Dusk;
	}
	else
	{
		return ETimeOfDay::Night;
	}
}

FString UTimeManager::GetTimeOfDayString() const
{
	switch (GetCurrentTimeOfDay())
	{
	case ETimeOfDay::Dawn:
		return TEXT("Dawn");
	case ETimeOfDay::Morning:
		return TEXT("Morning");
	case ETimeOfDay::Noon:
		return TEXT("Noon");
	case ETimeOfDay::Afternoon:
		return TEXT("Afternoon");
	case ETimeOfDay::Dusk:
		return TEXT("Dusk");
	case ETimeOfDay::Night:
		return TEXT("Night");
	case ETimeOfDay::Midnight:
		return TEXT("Midnight");
	default:
		return TEXT("Unknown");
	}
}

float UTimeManager::GetNormalizedTime() const
{
	return CurrentTime / 24.0f;
}
