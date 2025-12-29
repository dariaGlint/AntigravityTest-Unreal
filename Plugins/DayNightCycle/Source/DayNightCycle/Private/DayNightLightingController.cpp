// Copyright Epic Games, Inc. All Rights Reserved.

#include "DayNightLightingController.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/GameplayStatics.h"

ADayNightLightingController::ADayNightLightingController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create time manager component
	TimeManager = CreateDefaultSubobject<UTimeManager>(TEXT("TimeManager"));

	// Default values
	SunriseAngle = -90.0f;
	SunsetAngle = 90.0f;
	bAutoUpdateLighting = true;
	UpdateInterval = 0.0f; // Update every frame by default
	TimeSinceLastUpdate = 0.0f;

	SunIntensityCurve = nullptr;
	SunColorCurve = nullptr;
	SkyIntensityCurve = nullptr;
}

void ADayNightLightingController::BeginPlay()
{
	Super::BeginPlay();

	InitializeTimeManager();
	FindLightActors();
}

void ADayNightLightingController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAutoUpdateLighting && TimeManager)
	{
		TimeSinceLastUpdate += DeltaTime;

		if (UpdateInterval <= 0.0f || TimeSinceLastUpdate >= UpdateInterval)
		{
			UpdateLighting();
			TimeSinceLastUpdate = 0.0f;
		}
	}
}

void ADayNightLightingController::InitializeTimeManager()
{
	if (TimeManager)
	{
		// Bind to time changed event
		TimeManager->OnTimeChanged.AddDynamic(this, &ADayNightLightingController::UpdateLighting);
	}
}

void ADayNightLightingController::FindLightActors()
{
	// Try to find directional light if not set
	if (!SunLight)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			SunLight = Cast<ADirectionalLight>(FoundActors[0]);
		}
	}

	// Try to find sky light if not set
	if (!SkyLightActor)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkyLight::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			SkyLightActor = Cast<ASkyLight>(FoundActors[0]);
		}
	}
}

void ADayNightLightingController::UpdateLighting()
{
	if (!TimeManager)
	{
		return;
	}

	float CurrentTime = TimeManager->CurrentTime;

	SetSunRotationFromTime(CurrentTime);
	SetSunIntensityFromTime(CurrentTime);
	SetSunColorFromTime(CurrentTime);
	SetSkyIntensityFromTime(CurrentTime);
}

void ADayNightLightingController::SetSunRotationFromTime(float Time)
{
	if (!SunLight)
	{
		return;
	}

	FRotator NewRotation = CalculateSunRotation(Time);
	SunLight->SetActorRotation(NewRotation);
}

FRotator ADayNightLightingController::CalculateSunRotation(float Time) const
{
	// Normalize time to 0-1 range
	float NormalizedTime = Time / 24.0f;

	// Calculate pitch: -90 at sunrise (6:00), 0 at noon (12:00), 90 at sunset (18:00)
	// Sun rises at 6:00 and sets at 18:00
	float SunProgress = (Time - 6.0f) / 12.0f; // 0 at sunrise, 1 at sunset
	float Pitch = FMath::Lerp(SunriseAngle, SunsetAngle, SunProgress);

	// Keep yaw at 0 for simplicity (sun moves along one axis)
	float Yaw = 0.0f;
	float Roll = 0.0f;

	return FRotator(Pitch, Yaw, Roll);
}

void ADayNightLightingController::SetSunIntensityFromTime(float Time)
{
	if (!SunLight || !SunLight->GetLightComponent())
	{
		return;
	}

	float Intensity = 10.0f; // Default intensity

	if (SunIntensityCurve)
	{
		float NormalizedTime = Time / 24.0f;
		Intensity = SunIntensityCurve->GetFloatValue(NormalizedTime);
	}
	else
	{
		// Simple intensity calculation without curve
		// Full intensity during day, low at night
		if (Time >= 6.0f && Time <= 18.0f)
		{
			// Day time: fade in/out at sunrise and sunset
			if (Time < 7.0f)
			{
				// Sunrise fade in
				Intensity = FMath::Lerp(0.1f, 10.0f, (Time - 6.0f));
			}
			else if (Time > 17.0f)
			{
				// Sunset fade out
				Intensity = FMath::Lerp(10.0f, 0.1f, (Time - 17.0f));
			}
			else
			{
				Intensity = 10.0f;
			}
		}
		else
		{
			// Night time: moon light
			Intensity = 0.1f;
		}
	}

	SunLight->GetLightComponent()->SetIntensity(Intensity);
}

void ADayNightLightingController::SetSunColorFromTime(float Time)
{
	if (!SunLight || !SunLight->GetLightComponent())
	{
		return;
	}

	FLinearColor Color = FLinearColor::White;

	if (SunColorCurve)
	{
		float NormalizedTime = Time / 24.0f;
		Color = SunColorCurve->GetLinearColorValue(NormalizedTime);
	}
	else
	{
		// Simple color calculation without curve
		if (Time >= 5.0f && Time < 7.0f)
		{
			// Dawn: orange/red tint
			Color = FLinearColor(1.0f, 0.6f, 0.4f);
		}
		else if (Time >= 7.0f && Time < 17.0f)
		{
			// Day: white/yellow
			Color = FLinearColor(1.0f, 0.95f, 0.9f);
		}
		else if (Time >= 17.0f && Time < 19.0f)
		{
			// Dusk: orange/red tint
			Color = FLinearColor(1.0f, 0.5f, 0.3f);
		}
		else
		{
			// Night: blue tint
			Color = FLinearColor(0.7f, 0.8f, 1.0f);
		}
	}

	SunLight->GetLightComponent()->SetLightColor(Color);
}

void ADayNightLightingController::SetSkyIntensityFromTime(float Time)
{
	if (!SkyLightActor || !SkyLightActor->GetLightComponent())
	{
		return;
	}

	float Intensity = 1.0f;

	if (SkyIntensityCurve)
	{
		float NormalizedTime = Time / 24.0f;
		Intensity = SkyIntensityCurve->GetFloatValue(NormalizedTime);
	}
	else
	{
		// Simple intensity calculation
		if (Time >= 6.0f && Time <= 18.0f)
		{
			// Day time
			Intensity = 1.0f;
		}
		else
		{
			// Night time
			Intensity = 0.3f;
		}
	}

	SkyLightActor->GetLightComponent()->SetIntensity(Intensity);
}
