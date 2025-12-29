// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveSpawnTypes.generated.h"

/**
 * Represents a single enemy type and count within a wave
 */
USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	/** The enemy class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn")
	TSubclassOf<AActor> EnemyClass;

	/** Number of enemies of this type to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn", meta = (ClampMin = 1))
	int32 Count = 1;

	/** Delay between spawning each enemy of this type (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn", meta = (ClampMin = 0.0))
	float SpawnInterval = 1.0f;

	/** Multiplier for enemy health (applied per wave) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Difficulty", meta = (ClampMin = 0.1))
	float HealthMultiplier = 1.0f;

	/** Multiplier for enemy damage (applied per wave) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Difficulty", meta = (ClampMin = 0.1))
	float DamageMultiplier = 1.0f;
};

/**
 * Configuration for a single wave
 */
USTRUCT(BlueprintType)
struct FWaveConfiguration
{
	GENERATED_BODY()

	/** Array of enemy types and counts to spawn in this wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn")
	TArray<FEnemySpawnInfo> Enemies;

	/** Delay before starting this wave (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn", meta = (ClampMin = 0.0))
	float PreWaveDelay = 5.0f;

	/** Time limit for completing this wave (0 = no limit) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn", meta = (ClampMin = 0.0))
	float TimeLimit = 0.0f;

	/** Whether all enemies must be defeated to complete the wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn")
	bool bRequireAllEnemiesDefeated = true;

	/** Reward to grant upon wave completion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn")
	FString RewardIdentifier;
};

/**
 * Difficulty scaling settings
 */
USTRUCT(BlueprintType)
struct FDifficultyScaling
{
	GENERATED_BODY()

	/** Health multiplier increase per wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty Scaling", meta = (ClampMin = 0.0))
	float HealthScalePerWave = 0.1f;

	/** Damage multiplier increase per wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty Scaling", meta = (ClampMin = 0.0))
	float DamageScalePerWave = 0.05f;

	/** Enemy count multiplier per wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty Scaling", meta = (ClampMin = 0.0))
	float CountScalePerWave = 0.0f;

	/** Spawn interval reduction per wave (makes spawns faster) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty Scaling", meta = (ClampMin = 0.0))
	float SpawnIntervalReductionPerWave = 0.0f;

	/** Maximum multiplier cap for health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty Scaling", meta = (ClampMin = 1.0))
	float MaxHealthMultiplier = 5.0f;

	/** Maximum multiplier cap for damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty Scaling", meta = (ClampMin = 1.0))
	float MaxDamageMultiplier = 3.0f;
};

/**
 * Enum for spawn point selection strategies
 */
UENUM(BlueprintType)
enum class ESpawnPointSelectionMode : uint8
{
	/** Select spawn points in sequential order */
	Sequential UMETA(DisplayName = "Sequential"),

	/** Select spawn points randomly */
	Random UMETA(DisplayName = "Random"),

	/** Select the spawn point farthest from the player */
	FarthestFromPlayer UMETA(DisplayName = "Farthest From Player"),

	/** Select the spawn point outside player's view */
	OutOfSight UMETA(DisplayName = "Out Of Sight")
};
