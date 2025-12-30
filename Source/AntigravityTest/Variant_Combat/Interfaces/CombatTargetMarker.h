// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatTargetMarker.generated.h"

/**
 * Interface for UI widgets that display target markers
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UCombatTargetMarker : public UInterface
{
	GENERATED_BODY()
};

class ICombatTargetMarker
{
	GENERATED_BODY()

public:

	/**
	 * Called when a target is locked
	 * @param Target The actor that was locked onto
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target Marker")
	void ShowTargetMarker(AActor* Target);

	/**
	 * Called when target lock is released
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target Marker")
	void HideTargetMarker();

	/**
	 * Updates the marker position for the current target
	 * @param Target The current target actor
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target Marker")
	void UpdateTargetMarker(AActor* Target);
};
