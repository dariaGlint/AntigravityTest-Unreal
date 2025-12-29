// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPool.h"
#include "IPoolable.h"
#include "Engine/World.h"

UObjectPool::UObjectPool()
	: MaxPoolSize(0)
{
}

void UObjectPool::Initialize(UWorld* InWorld, TSubclassOf<AActor> InActorClass, int32 InitialSize, int32 InMaxSize)
{
	World = InWorld;
	ActorClass = InActorClass;
	MaxPoolSize = InMaxSize;

	// 初期サイズ分のActorを事前生成
	Prewarm(InitialSize);
}

AActor* UObjectPool::Spawn(const FTransform& SpawnTransform)
{
	if (!World || !ActorClass)
	{
		return nullptr;
	}

	AActor* Actor = nullptr;

	// プールから取得
	if (InactiveActors.Num() > 0)
	{
		Actor = InactiveActors.Pop();
	}
	else
	{
		// プールが空の場合、新規作成
		Actor = CreateNewActor();
		if (!Actor)
		{
			return nullptr;
		}
		Stats.ExpandCount++;
	}

	// Actorを有効化
	Actor->SetActorTransform(SpawnTransform);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);

	// IPoolableインターフェースがあればOnPoolActivateを呼ぶ
	if (Actor->Implements<UPoolable>())
	{
		IPoolable::Execute_OnPoolActivate(Actor);
	}

	// アクティブセットに追加
	ActiveActors.Add(Actor);

	// 統計更新
	Stats.TotalSpawnCount++;
	Stats.ActiveObjects = ActiveActors.Num();
	Stats.InactiveObjects = InactiveActors.Num();
	Stats.PeakActiveObjects = FMath::Max(Stats.PeakActiveObjects, Stats.ActiveObjects);

	return Actor;
}

bool UObjectPool::Despawn(AActor* Actor)
{
	if (!Actor || !ActiveActors.Contains(Actor))
	{
		return false;
	}

	// IPoolableインターフェースがあればCanBePooledをチェック
	if (Actor->Implements<UPoolable>())
	{
		if (!IPoolable::Execute_CanBePooled(Actor))
		{
			return false;
		}

		// OnPoolDeactivateを呼ぶ
		IPoolable::Execute_OnPoolDeactivate(Actor);
	}

	// Actorを無効化
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);

	// アクティブセットから削除
	ActiveActors.Remove(Actor);

	// プールサイズ制限チェック
	if (MaxPoolSize > 0 && InactiveActors.Num() >= MaxPoolSize)
	{
		// 制限を超える場合は破棄
		Actor->Destroy();
		Stats.TotalObjects--;
	}
	else
	{
		// プールに返却
		InactiveActors.Add(Actor);
	}

	// 統計更新
	Stats.ActiveObjects = ActiveActors.Num();
	Stats.InactiveObjects = InactiveActors.Num();

	return true;
}

void UObjectPool::Clear()
{
	// すべてのアクティブActorを破棄
	for (AActor* Actor : ActiveActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	ActiveActors.Empty();

	// すべての非アクティブActorを破棄
	for (AActor* Actor : InactiveActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	InactiveActors.Empty();

	// 統計リセット
	Stats = FPoolStats();
}

void UObjectPool::Prewarm(int32 Count)
{
	if (!World || !ActorClass)
	{
		return;
	}

	for (int32 i = 0; i < Count; i++)
	{
		// 最大サイズチェック
		if (MaxPoolSize > 0 && Stats.TotalObjects >= MaxPoolSize)
		{
			break;
		}

		AActor* Actor = CreateNewActor();
		if (Actor)
		{
			InactiveActors.Add(Actor);
		}
	}

	Stats.InactiveObjects = InactiveActors.Num();
}

FPoolStats UObjectPool::GetStats() const
{
	return Stats;
}

AActor* UObjectPool::CreateNewActor()
{
	if (!World || !ActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Actor = World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (Actor)
	{
		// 初期状態で非アクティブ化
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorTickEnabled(false);

		Stats.TotalObjects++;
	}

	return Actor;
}
