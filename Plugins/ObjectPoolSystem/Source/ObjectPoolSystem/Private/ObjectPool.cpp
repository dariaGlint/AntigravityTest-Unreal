// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPool.h"
#include "Engine/World.h"

UObjectPool::UObjectPool()
	: MaxPoolSize(0)
{
}

void UObjectPool::Initialize(UWorld* InWorld, TSubclassOf<AActor> InActorClass, int32 InInitialSize, int32 InMaxSize)
{
	World = InWorld;
	ActorClass = InActorClass;
	MaxPoolSize = InMaxSize;

	// 初期サイズ分のActorを事前生成
	Prewarm(InInitialSize);
}

AActor* UObjectPool::Spawn(const FTransform& SpawnTransform)
{
	if (!World.IsValid() || !ActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPool::Spawn - Invalid World or ActorClass"));
		return nullptr;
	}

	AActor* Actor = nullptr;

	// プールに利用可能なActorがあればそれを使用
	if (AvailableActors.Num() > 0)
	{
		Actor = AvailableActors.Pop();
	}
	else
	{
		// プールが空の場合は新規作成
		Actor = CreateNewActor();
		if (!Actor)
		{
			return nullptr;
		}
		Stats.PoolExpansionCount++;
	}

	// Actorをアクティブ化
	Actor->SetActorTransform(SpawnTransform);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);

	// IPoolableインターフェースを実装している場合は OnPoolActivate を呼ぶ
	if (Actor->Implements<UPoolable>())
	{
		IPoolable::Execute_OnPoolActivate(Actor);
	}

	// アクティブリストに追加
	ActiveActors.Add(Actor);
	Stats.ActiveObjects = ActiveActors.Num();
	Stats.TotalSpawnCount++;

	// ピーク値を更新
	if (Stats.ActiveObjects > Stats.PeakActiveObjects)
	{
		Stats.PeakActiveObjects = Stats.ActiveObjects;
	}

	return Actor;
}

bool UObjectPool::Despawn(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	// アクティブリストから削除
	int32 RemovedCount = ActiveActors.Remove(Actor);
	if (RemovedCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ObjectPool::Despawn - Actor was not in active list"));
		return false;
	}

	// IPoolableインターフェースのチェック
	if (Actor->Implements<UPoolable>())
	{
		// プールに戻せるかチェック
		if (!IPoolable::Execute_CanBePooled(Actor))
		{
			Actor->Destroy();
			Stats.TotalObjects--;
			Stats.ActiveObjects = ActiveActors.Num();
			return true;
		}

		// OnPoolDeactivate を呼ぶ
		IPoolable::Execute_OnPoolDeactivate(Actor);
	}

	// Actorを非アクティブ化
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);

	// 最大サイズチェック
	if (MaxPoolSize > 0 && Stats.TotalObjects >= MaxPoolSize)
	{
		// 最大サイズに達している場合は破棄
		Actor->Destroy();
		Stats.TotalObjects--;
	}
	else
	{
		// プールに戻す
		AvailableActors.Add(Actor);
	}

	Stats.ActiveObjects = ActiveActors.Num();
	Stats.InactiveObjects = AvailableActors.Num();

	return true;
}

void UObjectPool::Prewarm(int32 Count)
{
	if (!World.IsValid() || !ActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPool::Prewarm - Invalid World or ActorClass"));
		return;
	}

	for (int32 i = 0; i < Count; i++)
	{
		// 最大サイズチェック
		if (MaxPoolSize > 0 && Stats.TotalObjects >= MaxPoolSize)
		{
			break;
		}

		AActor* NewActor = CreateNewActor();
		if (NewActor)
		{
			AvailableActors.Add(NewActor);
		}
	}

	Stats.InactiveObjects = AvailableActors.Num();
}

void UObjectPool::Clear()
{
	// すべてのActorを破棄
	for (AActor* Actor : AvailableActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	for (AActor* Actor : ActiveActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	AvailableActors.Empty();
	ActiveActors.Empty();

	Stats = FPoolStats();
}

FPoolStats UObjectPool::GetStats() const
{
	return Stats;
}

void UObjectPool::LogStats() const
{
	UE_LOG(LogTemp, Log, TEXT("ObjectPool Stats for %s:"), ActorClass ? *ActorClass->GetName() : TEXT("Unknown"));
	UE_LOG(LogTemp, Log, TEXT("  Total Objects: %d"), Stats.TotalObjects);
	UE_LOG(LogTemp, Log, TEXT("  Active Objects: %d"), Stats.ActiveObjects);
	UE_LOG(LogTemp, Log, TEXT("  Inactive Objects: %d"), Stats.InactiveObjects);
	UE_LOG(LogTemp, Log, TEXT("  Peak Active: %d"), Stats.PeakActiveObjects);
	UE_LOG(LogTemp, Log, TEXT("  Total Spawns: %d"), Stats.TotalSpawnCount);
	UE_LOG(LogTemp, Log, TEXT("  Pool Expansions: %d"), Stats.PoolExpansionCount);
}

AActor* UObjectPool::CreateNewActor()
{
	if (!World.IsValid() || !ActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (NewActor)
	{
		// 初期状態は非アクティブ
		NewActor->SetActorHiddenInGame(true);
		NewActor->SetActorEnableCollision(false);
		NewActor->SetActorTickEnabled(false);

		Stats.TotalObjects++;
	}

	return NewActor;
}
