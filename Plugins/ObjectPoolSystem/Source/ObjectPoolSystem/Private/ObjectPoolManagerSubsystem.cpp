// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPoolManagerSubsystem.h"
#include "Engine/World.h"

void UObjectPoolManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("ObjectPoolManagerSubsystem initialized"));
}

void UObjectPoolManagerSubsystem::Deinitialize()
{
	ClearAllPools();
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("ObjectPoolManagerSubsystem deinitialized"));
}

AActor* UObjectPoolManagerSubsystem::SpawnFromPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, int32 InitialPoolSize, int32 MaxPoolSize)
{
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnFromPool: ActorClass is null"));
		return nullptr;
	}

	UObjectPool* Pool = GetOrCreatePool(ActorClass, InitialPoolSize, MaxPoolSize);
	if (!Pool)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnFromPool: Failed to create pool for %s"), *ActorClass->GetName());
		return nullptr;
	}

	AActor* Actor = Pool->Spawn(SpawnTransform);
	if (Actor)
	{
		ActorToPoolMap.Add(Actor, Pool);
	}

	return Actor;
}

bool UObjectPoolManagerSubsystem::ReturnToPool(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	// Actorのプールを検索
	UObjectPool** PoolPtr = ActorToPoolMap.Find(Actor);
	if (!PoolPtr || !*PoolPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReturnToPool: Actor %s is not from any pool"), *Actor->GetName());
		return false;
	}

	UObjectPool* Pool = *PoolPtr;
	bool bSuccess = Pool->Despawn(Actor);

	if (bSuccess)
	{
		ActorToPoolMap.Remove(Actor);
	}

	return bSuccess;
}

void UObjectPoolManagerSubsystem::PrewarmPool(TSubclassOf<AActor> ActorClass, int32 Count, int32 MaxPoolSize)
{
	if (!ActorClass)
	{
		return;
	}

	UObjectPool* Pool = GetOrCreatePool(ActorClass, Count, MaxPoolSize);
	if (Pool)
	{
		Pool->Prewarm(Count);
	}
}

void UObjectPoolManagerSubsystem::ClearPool(TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass)
	{
		return;
	}

	UObjectPool** PoolPtr = Pools.Find(ActorClass);
	if (PoolPtr && *PoolPtr)
	{
		UObjectPool* Pool = *PoolPtr;

		// ActorToPoolMapから該当するActorを削除
		TArray<AActor*> ActorsToRemove;
		for (const auto& Pair : ActorToPoolMap)
		{
			if (Pair.Value == Pool)
			{
				ActorsToRemove.Add(Pair.Key);
			}
		}

		for (AActor* Actor : ActorsToRemove)
		{
			ActorToPoolMap.Remove(Actor);
		}

		Pool->Clear();
		Pools.Remove(ActorClass);
	}
}

void UObjectPoolManagerSubsystem::ClearAllPools()
{
	for (auto& Pair : Pools)
	{
		if (Pair.Value)
		{
			Pair.Value->Clear();
		}
	}

	Pools.Empty();
	ActorToPoolMap.Empty();
}

bool UObjectPoolManagerSubsystem::GetPoolStats(TSubclassOf<AActor> ActorClass, FPoolStats& OutStats) const
{
	if (!ActorClass)
	{
		return false;
	}

	const UObjectPool* const* PoolPtr = Pools.Find(ActorClass);
	if (!PoolPtr || !*PoolPtr)
	{
		return false;
	}

	OutStats = (*PoolPtr)->GetStats();
	return true;
}

TMap<TSubclassOf<AActor>, FPoolStats> UObjectPoolManagerSubsystem::GetAllPoolStats() const
{
	TMap<TSubclassOf<AActor>, FPoolStats> AllStats;

	for (const auto& Pair : Pools)
	{
		if (Pair.Value)
		{
			AllStats.Add(Pair.Key, Pair.Value->GetStats());
		}
	}

	return AllStats;
}

void UObjectPoolManagerSubsystem::LogAllPoolStats() const
{
	UE_LOG(LogTemp, Log, TEXT("=== Object Pool Statistics ==="));

	for (const auto& Pair : Pools)
	{
		if (Pair.Value && Pair.Key)
		{
			FPoolStats Stats = Pair.Value->GetStats();
			UE_LOG(LogTemp, Log, TEXT("Class: %s"), *Pair.Key->GetName());
			UE_LOG(LogTemp, Log, TEXT("  Total Objects: %d"), Stats.TotalObjects);
			UE_LOG(LogTemp, Log, TEXT("  Active: %d"), Stats.ActiveObjects);
			UE_LOG(LogTemp, Log, TEXT("  Inactive: %d"), Stats.InactiveObjects);
			UE_LOG(LogTemp, Log, TEXT("  Peak Active: %d"), Stats.PeakActiveObjects);
			UE_LOG(LogTemp, Log, TEXT("  Total Spawns: %d"), Stats.TotalSpawnCount);
			UE_LOG(LogTemp, Log, TEXT("  Expands: %d"), Stats.ExpandCount);
			UE_LOG(LogTemp, Log, TEXT(""));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("=============================="));
}

UObjectPool* UObjectPoolManagerSubsystem::GetOrCreatePool(TSubclassOf<AActor> ActorClass, int32 InitialSize, int32 MaxSize)
{
	if (!ActorClass)
	{
		return nullptr;
	}

	// 既存のプールをチェック
	UObjectPool** PoolPtr = Pools.Find(ActorClass);
	if (PoolPtr && *PoolPtr)
	{
		return *PoolPtr;
	}

	// 新しいプールを作成
	UObjectPool* NewPool = NewObject<UObjectPool>(this);
	if (NewPool)
	{
		NewPool->Initialize(GetWorld(), ActorClass, InitialSize, MaxSize);
		Pools.Add(ActorClass, NewPool);
		UE_LOG(LogTemp, Log, TEXT("Created new pool for %s (Initial: %d, Max: %d)"),
			*ActorClass->GetName(), InitialSize, MaxSize);
	}

	return NewPool;
}
