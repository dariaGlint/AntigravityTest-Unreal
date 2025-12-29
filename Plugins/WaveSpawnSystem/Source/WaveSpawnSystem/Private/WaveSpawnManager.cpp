// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaveSpawnManager.h"
#include "WaveSpawnPoint.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

AWaveSpawnManager::AWaveSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set default root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void AWaveSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	// Initialize spawn points
	InitializeSpawnPoints();

	// Auto-start if configured
	if (bAutoStartFirstWave && Waves.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaveDelayTimer,
			this,
			&AWaveSpawnManager::StartWaves,
			AutoStartDelay,
			false
		);
	}
}

void AWaveSpawnManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clean up timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveDelayTimer);
		World->GetTimerManager().ClearTimer(SpawnIntervalTimer);
		World->GetTimerManager().ClearTimer(WaveTimeLimitTimer);
	}

	// Unsubscribe from all enemy death events
	for (AActor* Enemy : SpawnedEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->OnDestroyed.RemoveDynamic(this, &AWaveSpawnManager::OnEnemyDestroyed);
		}
	}
	SpawnedEnemies.Empty();
}

void AWaveSpawnManager::InitializeSpawnPoints()
{
	// If spawn points array is empty, find all spawn points in the level
	if (SpawnPoints.Num() == 0)
	{
		for (TActorIterator<AWaveSpawnPoint> It(GetWorld()); It; ++It)
		{
			AWaveSpawnPoint* SpawnPoint = *It;
			if (IsValid(SpawnPoint))
			{
				// Filter by tag if specified
				if (SpawnPointFilterTag.IsNone() || SpawnPoint->SpawnTag == SpawnPointFilterTag)
				{
					SpawnPoints.Add(SpawnPoint);
				}
			}
		}
	}

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawnManager: No spawn points found!"));
	}
}

void AWaveSpawnManager::StartWaves()
{
	if (Waves.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawnManager: No waves configured!"));
		return;
	}

	CurrentWaveIndex = -1;
	StartNextWave();
}

void AWaveSpawnManager::StartWave(int32 WaveIndex)
{
	if (!Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawnManager: Invalid wave index %d"), WaveIndex);
		return;
	}

	CurrentWaveIndex = WaveIndex - 1;
	StartNextWave();
}

void AWaveSpawnManager::StopCurrentWave()
{
	if (!bWaveActive)
	{
		return;
	}

	// Clear timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveDelayTimer);
		World->GetTimerManager().ClearTimer(SpawnIntervalTimer);
		World->GetTimerManager().ClearTimer(WaveTimeLimitTimer);
	}

	// Destroy all spawned enemies
	for (AActor* Enemy : SpawnedEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->OnDestroyed.RemoveDynamic(this, &AWaveSpawnManager::OnEnemyDestroyed);
			Enemy->Destroy();
		}
	}
	SpawnedEnemies.Empty();

	bWaveActive = false;
	ActiveEnemyCount = 0;
	TotalEnemiesSpawnedThisWave = 0;
}

void AWaveSpawnManager::StartNextWave()
{
	CurrentWaveIndex++;

	// Check if we've completed all waves
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		if (bLoopWaves)
		{
			CurrentWaveIndex = 0;
		}
		else
		{
			OnAllWavesCompleted.Broadcast();
			return;
		}
	}

	// Schedule wave start after pre-wave delay
	const float PreWaveDelay = Waves[CurrentWaveIndex].PreWaveDelay;
	GetWorld()->GetTimerManager().SetTimer(
		WaveDelayTimer,
		[this]() { BeginWave(CurrentWaveIndex); },
		PreWaveDelay,
		false
	);
}

void AWaveSpawnManager::BeginWave(int32 WaveIndex)
{
	if (!Waves.IsValidIndex(WaveIndex))
	{
		return;
	}

	bWaveActive = true;
	ActiveEnemyCount = 0;
	TotalEnemiesSpawnedThisWave = 0;
	SpawnedEnemies.Empty();

	OnWaveStarted.Broadcast(WaveIndex + 1);

	// Set wave time limit if configured
	const FWaveConfiguration& Wave = Waves[WaveIndex];
	if (Wave.TimeLimit > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaveTimeLimitTimer,
			this,
			&AWaveSpawnManager::OnWaveTimeExpired,
			Wave.TimeLimit,
			false
		);
	}

	// Start spawning enemies
	SpawnWaveEnemies();
}

void AWaveSpawnManager::SpawnWaveEnemies()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		return;
	}

	const FWaveConfiguration& Wave = Waves[CurrentWaveIndex];

	// Spawn all enemy types configured for this wave
	for (int32 EnemyTypeIndex = 0; EnemyTypeIndex < Wave.Enemies.Num(); ++EnemyTypeIndex)
	{
		FEnemySpawnInfo EnemyInfo = Wave.Enemies[EnemyTypeIndex];

		// Apply difficulty scaling
		ApplyDifficultyScaling(EnemyInfo, CurrentWaveIndex);

		// Spawn each enemy of this type
		for (int32 i = 0; i < EnemyInfo.Count; ++i)
		{
			// Use timer for spawn interval
			float SpawnDelay = i * EnemyInfo.SpawnInterval;
			GetWorld()->GetTimerManager().SetTimer(
				SpawnIntervalTimer,
				[this, EnemyInfo, i]() { SpawnEnemy(EnemyInfo, i); },
				SpawnDelay,
				false
			);
		}
	}
}

void AWaveSpawnManager::SpawnEnemy(const FEnemySpawnInfo& EnemyInfo, int32 EnemyIndex)
{
	if (!EnemyInfo.EnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawnManager: No enemy class specified!"));
		return;
	}

	// Select spawn point
	AWaveSpawnPoint* SpawnPoint = SelectSpawnPoint();
	if (!SpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawnManager: No valid spawn point available!"));
		return;
	}

	// Spawn the enemy
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(
		EnemyInfo.EnemyClass,
		SpawnPoint->GetSpawnTransform(),
		SpawnParams
	);

	if (SpawnedEnemy)
	{
		// Track the enemy
		SpawnedEnemies.Add(SpawnedEnemy);
		ActiveEnemyCount++;
		TotalEnemiesSpawnedThisWave++;

		// Subscribe to death event
		SpawnedEnemy->OnDestroyed.AddDynamic(this, &AWaveSpawnManager::OnEnemyDestroyed);

		// Notify spawn point
		SpawnPoint->OnEnemySpawned();

		// Broadcast event
		OnEnemySpawned.Broadcast(SpawnedEnemy, CurrentWaveIndex + 1, EnemyIndex);
	}
}

AWaveSpawnPoint* AWaveSpawnManager::SelectSpawnPoint()
{
	if (SpawnPoints.Num() == 0)
	{
		return nullptr;
	}

	// Filter available spawn points
	TArray<AWaveSpawnPoint*> AvailablePoints;
	for (AWaveSpawnPoint* Point : SpawnPoints)
	{
		if (Point && Point->CanSpawn())
		{
			AvailablePoints.Add(Point);
		}
	}

	if (AvailablePoints.Num() == 0)
	{
		return nullptr;
	}

	AWaveSpawnPoint* SelectedPoint = nullptr;

	switch (SpawnPointSelectionMode)
	{
	case ESpawnPointSelectionMode::Sequential:
		SequentialSpawnIndex = SequentialSpawnIndex % AvailablePoints.Num();
		SelectedPoint = AvailablePoints[SequentialSpawnIndex];
		SequentialSpawnIndex++;
		break;

	case ESpawnPointSelectionMode::Random:
		SelectedPoint = AvailablePoints[FMath::RandRange(0, AvailablePoints.Num() - 1)];
		break;

	case ESpawnPointSelectionMode::FarthestFromPlayer:
	case ESpawnPointSelectionMode::OutOfSight:
	{
		APawn* PlayerPawn = GetPlayerPawn();
		if (!PlayerPawn)
		{
			// Fallback to random if no player found
			SelectedPoint = AvailablePoints[FMath::RandRange(0, AvailablePoints.Num() - 1)];
			break;
		}

		float BestDistance = -1.0f;
		for (AWaveSpawnPoint* Point : AvailablePoints)
		{
			float Distance = FVector::Dist(Point->GetActorLocation(), PlayerPawn->GetActorLocation());
			if (Distance > BestDistance)
			{
				BestDistance = Distance;
				SelectedPoint = Point;
			}
		}
		break;
	}
	}

	return SelectedPoint;
}

void AWaveSpawnManager::OnEnemyDestroyed(AActor* DestroyedActor)
{
	SpawnedEnemies.Remove(DestroyedActor);
	ActiveEnemyCount = FMath::Max(0, ActiveEnemyCount - 1);

	OnEnemyDefeated.Broadcast(DestroyedActor, CurrentWaveIndex + 1, ActiveEnemyCount);

	CheckWaveCompletion();
}

void AWaveSpawnManager::CheckWaveCompletion()
{
	if (!bWaveActive)
	{
		return;
	}

	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		return;
	}

	const FWaveConfiguration& Wave = Waves[CurrentWaveIndex];

	// Check if wave is complete
	bool bWaveComplete = false;
	if (Wave.bRequireAllEnemiesDefeated)
	{
		bWaveComplete = (ActiveEnemyCount == 0 && TotalEnemiesSpawnedThisWave > 0);
	}

	if (bWaveComplete)
	{
		CompleteWave(true);
	}
}

void AWaveSpawnManager::CompleteWave(bool bSuccess)
{
	if (!bWaveActive)
	{
		return;
	}

	bWaveActive = false;

	// Clear wave time limit timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveTimeLimitTimer);
	}

	OnWaveCompleted.Broadcast(CurrentWaveIndex + 1, bSuccess);

	if (bSuccess)
	{
		// Start next wave
		StartNextWave();
	}
}

void AWaveSpawnManager::OnWaveTimeExpired()
{
	CompleteWave(false);
}

void AWaveSpawnManager::ApplyDifficultyScaling(FEnemySpawnInfo& EnemyInfo, int32 WaveIndex)
{
	// Apply health scaling
	float HealthScale = 1.0f + (DifficultyScaling.HealthScalePerWave * WaveIndex);
	HealthScale = FMath::Min(HealthScale, DifficultyScaling.MaxHealthMultiplier);
	EnemyInfo.HealthMultiplier *= HealthScale;

	// Apply damage scaling
	float DamageScale = 1.0f + (DifficultyScaling.DamageScalePerWave * WaveIndex);
	DamageScale = FMath::Min(DamageScale, DifficultyScaling.MaxDamageMultiplier);
	EnemyInfo.DamageMultiplier *= DamageScale;

	// Apply count scaling (if configured)
	if (DifficultyScaling.CountScalePerWave > 0.0f)
	{
		float CountScale = 1.0f + (DifficultyScaling.CountScalePerWave * WaveIndex);
		EnemyInfo.Count = FMath::CeilToInt(EnemyInfo.Count * CountScale);
	}

	// Apply spawn interval reduction
	if (DifficultyScaling.SpawnIntervalReductionPerWave > 0.0f)
	{
		float IntervalReduction = DifficultyScaling.SpawnIntervalReductionPerWave * WaveIndex;
		EnemyInfo.SpawnInterval = FMath::Max(0.1f, EnemyInfo.SpawnInterval - IntervalReduction);
	}
}

APawn* AWaveSpawnManager::GetPlayerPawn() const
{
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			return PC->GetPawn();
		}
	}
	return nullptr;
}
