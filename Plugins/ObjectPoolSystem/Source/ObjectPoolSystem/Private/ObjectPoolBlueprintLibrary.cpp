// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPoolBlueprintLibrary.h"
#include "ObjectPoolManagerSubsystem.h"
#include "Engine/World.h"

AActor* UObjectPoolBlueprintLibrary::SpawnPooledActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, int32 InitialPoolSize, int32 MaxPoolSize)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::SpawnPooledActor - Failed to get ObjectPoolManagerSubsystem"));
		return nullptr;
	}

	return PoolManager->SpawnFromPool(ActorClass, SpawnTransform, InitialPoolSize, MaxPoolSize);
}

bool UObjectPoolBlueprintLibrary::DespawnPooledActor(UObject* WorldContextObject, AActor* Actor)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return false;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::DespawnPooledActor - Failed to get ObjectPoolManagerSubsystem"));
		return false;
	}

	return PoolManager->ReturnToPool(Actor);
}

void UObjectPoolBlueprintLibrary::PrewarmPool(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, int32 Count, int32 InitialPoolSize, int32 MaxPoolSize)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::PrewarmPool - Failed to get ObjectPoolManagerSubsystem"));
		return;
	}

	PoolManager->PrewarmPool(ActorClass, Count, InitialPoolSize, MaxPoolSize);
}

void UObjectPoolBlueprintLibrary::ClearPool(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::ClearPool - Failed to get ObjectPoolManagerSubsystem"));
		return;
	}

	PoolManager->ClearPool(ActorClass);
}

void UObjectPoolBlueprintLibrary::ClearAllPools(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::ClearAllPools - Failed to get ObjectPoolManagerSubsystem"));
		return;
	}

	PoolManager->ClearAllPools();
}

FPoolStats UObjectPoolBlueprintLibrary::GetPoolStats(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return FPoolStats();
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::GetPoolStats - Failed to get ObjectPoolManagerSubsystem"));
		return FPoolStats();
	}

	return PoolManager->GetPoolStats(ActorClass);
}

void UObjectPoolBlueprintLibrary::LogPoolStats(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolBlueprintLibrary::LogPoolStats - Failed to get ObjectPoolManagerSubsystem"));
		return;
	}

	PoolManager->LogAllPoolStats();
}
