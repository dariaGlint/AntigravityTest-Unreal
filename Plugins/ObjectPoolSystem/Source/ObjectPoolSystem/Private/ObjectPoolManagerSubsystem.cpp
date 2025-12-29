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
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolManagerSubsystem::SpawnFromPool - Invalid ActorClass"));
		return nullptr;
	}

	UObjectPool* Pool = GetOrCreatePool(ActorClass, InitialPoolSize, MaxPoolSize);
	if (!Pool)
	{
		return nullptr;
	}

	return Pool->Spawn(SpawnTransform);
}

bool UObjectPoolManagerSubsystem::ReturnToPool(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	UObjectPool* Pool = FindPoolForActor(Actor);
	if (!Pool)
	{
		UE_LOG(LogTemp, Warning, TEXT("ObjectPoolManagerSubsystem::ReturnToPool - No pool found for actor %s"), *Actor->GetName());
		return false;
	}

	return Pool->Despawn(Actor);
}

void UObjectPoolManagerSubsystem::PrewarmPool(TSubclassOf<AActor> ActorClass, int32 Count, int32 InitialPoolSize, int32 MaxPoolSize)
{
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolManagerSubsystem::PrewarmPool - Invalid ActorClass"));
		return;
	}

	UObjectPool* Pool = GetOrCreatePool(ActorClass, InitialPoolSize, MaxPoolSize);
	if (Pool)
	{
		Pool->Prewarm(Count);
	}
}

void UObjectPoolManagerSubsystem::ClearPool(TSubclassOf<AActor> ActorClass)
{
	if (UObjectPool** PoolPtr = Pools.Find(ActorClass))
	{
		if (*PoolPtr)
		{
			(*PoolPtr)->Clear();
		}
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
}

FPoolStats UObjectPoolManagerSubsystem::GetPoolStats(TSubclassOf<AActor> ActorClass) const
{
	if (UObjectPool* const* PoolPtr = Pools.Find(ActorClass))
	{
		if (*PoolPtr)
		{
			return (*PoolPtr)->GetStats();
		}
	}
	return FPoolStats();
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
	UE_LOG(LogTemp, Log, TEXT("=== Object Pool Manager Stats ==="));
	UE_LOG(LogTemp, Log, TEXT("Total Pools: %d"), Pools.Num());

	for (const auto& Pair : Pools)
	{
		if (Pair.Value)
		{
			Pair.Value->LogStats();
		}
	}
}

UObjectPool* UObjectPoolManagerSubsystem::GetOrCreatePool(TSubclassOf<AActor> ActorClass, int32 InitialPoolSize, int32 MaxPoolSize)
{
	// 既存のプールがあればそれを返す
	if (UObjectPool** ExistingPool = Pools.Find(ActorClass))
	{
		return *ExistingPool;
	}

	// 新しいプールを作成
	UObjectPool* NewPool = NewObject<UObjectPool>(this);
	if (NewPool)
	{
		NewPool->Initialize(GetWorld(), ActorClass, InitialPoolSize, MaxPoolSize);
		Pools.Add(ActorClass, NewPool);
		UE_LOG(LogTemp, Log, TEXT("Created new object pool for %s"), *ActorClass->GetName());
	}

	return NewPool;
}

UObjectPool* UObjectPoolManagerSubsystem::FindPoolForActor(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	TSubclassOf<AActor> ActorClass = Actor->GetClass();
	if (UObjectPool* const* PoolPtr = Pools.Find(ActorClass))
	{
		return *PoolPtr;
	}

	return nullptr;
}
