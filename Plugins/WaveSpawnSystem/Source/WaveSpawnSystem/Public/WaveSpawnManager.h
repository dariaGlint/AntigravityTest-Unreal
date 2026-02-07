// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveSpawnTypes.h"
#include "WaveSpawnManager.generated.h"

class AWaveSpawnPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCompleted, int32, WaveNumber, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemySpawned, AActor*, Enemy, int32, WaveNumber, int32, EnemyIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyDefeated, AActor*, Enemy, int32, WaveNumber, int32, RemainingEnemies);

/**
 * Main manager class for wave-based enemy spawning system
 * Handles wave progression, difficulty scaling, and spawn point selection
 */
UCLASS(Blueprintable, ClassGroup = "WaveSpawn")
class WAVESPAWNSYSTEM_API AWaveSpawnManager : public AActor
{
	GENERATED_BODY()

public:

	/** Event fired when a new wave starts */
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawn|Events")
	FOnWaveStarted OnWaveStarted;

	/** Event fired when a wave is completed */
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawn|Events")
	FOnWaveCompleted OnWaveCompleted;

	/** Event fired when all waves are completed */
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawn|Events")
	FOnAllWavesCompleted OnAllWavesCompleted;

	/** Event fired when an enemy is spawned */
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawn|Events")
	FOnEnemySpawned OnEnemySpawned;

	/** Event fired when an enemy is defeated */
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawn|Events")
	FOnEnemyDefeated OnEnemyDefeated;

protected:

	/** Configuration for each wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Configuration")
	TArray<FWaveConfiguration> Waves;

	/** Difficulty scaling settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Configuration")
	FDifficultyScaling DifficultyScaling;

	/** How spawn points should be selected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Configuration")
	ESpawnPointSelectionMode SpawnPointSelectionMode = ESpawnPointSelectionMode::Random;

	/** Whether to automatically start the first wave on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Configuration")
	bool bAutoStartFirstWave = false;

	/** Delay before auto-starting the first wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Configuration", meta = (ClampMin = 0.0, EditCondition = "bAutoStartFirstWave"))
	float AutoStartDelay = 3.0f;

	/** Whether to loop waves infinitely after completing all configured waves */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Configuration")
	bool bLoopWaves = false;

	/** Array of spawn points to use (automatically populated if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Spawn Points")
	TArray<AWaveSpawnPoint*> SpawnPoints;

	/** Tag to filter spawn points (if empty, all spawn points are used) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawn|Spawn Points")
	FName SpawnPointFilterTag;

	/** Current wave index */
	UPROPERTY(BlueprintReadOnly, Category = "Wave Spawn|State")
	int32 CurrentWaveIndex = -1;

	/** Current number of active enemies */
	UPROPERTY(BlueprintReadOnly, Category = "Wave Spawn|State")
	int32 ActiveEnemyCount = 0;

	/** Total number of enemies spawned in current wave */
	UPROPERTY(BlueprintReadOnly, Category = "Wave Spawn|State")
	int32 TotalEnemiesSpawnedThisWave = 0;

	/** Whether a wave is currently active */
	UPROPERTY(BlueprintReadOnly, Category = "Wave Spawn|State")
	bool bWaveActive = false;

	/** Tracked spawned enemies */
	UPROPERTY()
	TArray<AActor*> SpawnedEnemies;

	/** Timer handle for wave delays */
	FTimerHandle WaveDelayTimer;

	/** Timer handle for spawn intervals */
	FTimerHandle SpawnIntervalTimer;

	/** Timer handle for wave time limit */
	FTimerHandle WaveTimeLimitTimer;

	/** Index for sequential spawn point selection */
	int32 SequentialSpawnIndex = 0;

public:

	AWaveSpawnManager();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Start the wave system */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	void StartWaves();

	/** Start a specific wave */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	void StartWave(int32 WaveIndex);

	/** Stop the current wave */
	UFUNCTION(BlueprintCallable, Category = "Wave Spawn")
	void StopCurrentWave();

	/** Get the current wave number (1-based for display) */
	UFUNCTION(BlueprintPure, Category = "Wave Spawn")
	int32 GetCurrentWaveNumber() const { return CurrentWaveIndex + 1; }

	/** Get total number of waves configured */
	UFUNCTION(BlueprintPure, Category = "Wave Spawn")
	int32 GetTotalWaveCount() const { return Waves.Num(); }

	/** Check if waves are currently active */
	UFUNCTION(BlueprintPure, Category = "Wave Spawn")
	bool IsWaveActive() const { return bWaveActive; }

	/** Get the number of active enemies */
	UFUNCTION(BlueprintPure, Category = "Wave Spawn")
	int32 GetActiveEnemyCount() const { return ActiveEnemyCount; }

protected:

	/** Initialize spawn points */
	void InitializeSpawnPoints();

	/** Start the next wave in sequence */
	void StartNextWave();

	/** Internal wave start implementation */
	void BeginWave(int32 WaveIndex);

	/** Spawn enemies for the current wave */
	void SpawnWaveEnemies();

	/** Spawn a single enemy */
	void SpawnEnemy(const FEnemySpawnInfo& EnemyInfo, int32 EnemyIndex);

	/** Select a spawn point based on the current selection mode */
	AWaveSpawnPoint* SelectSpawnPoint();

	/** Called when an enemy is destroyed */
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);

	/** Check if the current wave is completed */
	void CheckWaveCompletion();

	/** Complete the current wave */
	void CompleteWave(bool bSuccess);

	/** Called when wave time limit is reached */
	void OnWaveTimeExpired();

	/** Calculate difficulty-scaled values for an enemy */
	void ApplyDifficultyScaling(FEnemySpawnInfo& EnemyInfo, int32 WaveIndex);

	/** Get player pawn for spawn point selection */
	APawn* GetPlayerPawn() const;
};
