// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeManager.h"
#include "DayNightLightingController.generated.h"

class ADirectionalLight;
class ASkyLight;

/**
 * DayNightLightingController - Controls lighting based on time of day
 */
UCLASS()
class DAYNIGHTCYCLE_API ADayNightLightingController : public AActor
{
	GENERATED_BODY()

public:
	ADayNightLightingController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Reference to the time manager component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	UTimeManager* TimeManager;

	// Directional light (sun)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	ADirectionalLight* SunLight;

	// Sky light for ambient lighting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	ASkyLight* SkyLightActor;

	// Sun rotation settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	float SunriseAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	float SunsetAngle;

	// Sun intensity curve over time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	UCurveFloat* SunIntensityCurve;

	// Sun color curve over time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	UCurveLinearColor* SunColorCurve;

	// Sky light intensity curve over time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Settings")
	UCurveFloat* SkyIntensityCurve;

	// Whether to automatically update lighting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAutoUpdateLighting;

	// Update interval in seconds (0 = every frame)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float UpdateInterval;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void UpdateLighting();

	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void SetSunRotationFromTime(float Time);

	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void SetSunIntensityFromTime(float Time);

	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void SetSunColorFromTime(float Time);

	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void SetSkyIntensityFromTime(float Time);

private:
	float TimeSinceLastUpdate;

	void InitializeTimeManager();
	void FindLightActors();
	FRotator CalculateSunRotation(float Time) const;
};
