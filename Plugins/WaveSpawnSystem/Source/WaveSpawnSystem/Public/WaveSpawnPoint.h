// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveSpawnPoint.generated.h"

class UBillboardComponent;
class UArrowComponent;

/**
 * Marks a location where enemies can be spawned
 * Can be placed in the level to define spawn locations
 */
UCLASS(Blueprintable, ClassGroup = "WaveSpawn")
class WAVESPAWNSYSTEM_API AWaveSpawnPoint : public AActor
{
	GENERATED_BODY()

protected:

#if WITH_EDITORONLY_DATA
	/** Billboard component for editor visibility */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* Billboard;
#endif

	/** Arrow component to indicate spawn direction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* DirectionArrow;

public:

	/** Whether this spawn point is currently available for use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn")
	bool bIsEnabled = true;

	/** Priority of this spawn point (higher priority = more likely to be selected when using weighted random) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn", meta = (ClampMin = 0))
	int32 Priority = 1;

	/** Tag to filter spawn points (can be used to spawn specific enemy types at specific locations) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn")
	FName SpawnTag;

	/** Radius around this spawn point where other spawns should be avoided */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn", meta = (ClampMin = 0.0))
	float ExclusionRadius = 100.0f;

public:

	AWaveSpawnPoint();

	/** Returns the spawn transform for this point */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	FTransform GetSpawnTransform() const;

	/** Check if this spawn point can be used for spawning */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	bool CanSpawn() const;

	/** Called when an enemy is spawned at this location */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	void OnEnemySpawned();

	/** Called when the spawned enemy at this location is destroyed */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	void OnEnemyDestroyed();

protected:

	/** Current number of enemies spawned at this point */
	UPROPERTY(BlueprintReadOnly, Category = "Wave Spawn")
	int32 CurrentOccupants = 0;
};
