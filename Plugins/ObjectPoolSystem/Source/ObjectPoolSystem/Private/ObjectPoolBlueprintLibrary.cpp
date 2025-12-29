// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPoolBlueprintLibrary.h"
#include "ObjectPoolManagerSubsystem.h"
#include "Engine/World.h"

AActor* UObjectPoolBlueprintLibrary::SpawnPooledActor(
	UObject* WorldContextObject,
	TSubclassOf<AActor> ActorClass,
	FVector Location,
	FRotator Rotation,
	int32 InitialPoolSize,
	int32 MaxPoolSize)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnPooledActor: ObjectPoolManagerSubsystem not found"));
		return nullptr;
	}

	FTransform SpawnTransform(Rotation, Location);
	return PoolManager->SpawnFromPool(ActorClass, SpawnTransform, InitialPoolSize, MaxPoolSize);
}

bool UObjectPoolBlueprintLibrary::DespawnPooledActor(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		return false;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("DespawnPooledActor: ObjectPoolManagerSubsystem not found"));
		return false;
	}

	return PoolManager->ReturnToPool(Actor);
}

void UObjectPoolBlueprintLibrary::PrewarmPool(
	UObject* WorldContextObject,
	TSubclassOf<AActor> ActorClass,
	int32 Count,
	int32 MaxPoolSize)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrewarmPool: ObjectPoolManagerSubsystem not found"));
		return;
	}

	PoolManager->PrewarmPool(ActorClass, Count, MaxPoolSize);
}

bool UObjectPoolBlueprintLibrary::GetPoolStats(
	UObject* WorldContextObject,
	TSubclassOf<AActor> ActorClass,
	FPoolStats& OutStats)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return false;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		return false;
	}

	return PoolManager->GetPoolStats(ActorClass, OutStats);
}

void UObjectPoolBlueprintLibrary::LogPoolStats(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("LogPoolStats: ObjectPoolManagerSubsystem not found"));
		return;
	}

	PoolManager->LogAllPoolStats();
}

void UObjectPoolBlueprintLibrary::ClearPool(
	UObject* WorldContextObject,
	TSubclassOf<AActor> ActorClass)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClearPool: ObjectPoolManagerSubsystem not found"));
		return;
	}

	PoolManager->ClearPool(ActorClass);
}

void UObjectPoolBlueprintLibrary::ClearAllPools(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClearAllPools: ObjectPoolManagerSubsystem not found"));
		return;
	}

	PoolManager->ClearAllPools();
}
