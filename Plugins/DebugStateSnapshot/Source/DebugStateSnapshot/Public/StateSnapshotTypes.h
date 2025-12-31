// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateSnapshotTypes.generated.h"

/**
 * Represents a snapshot of game state at a specific point in time
 */
USTRUCT(BlueprintType)
struct DEBUGSTATESNAPSHOT_API FStateSnapshot
{
	GENERATED_BODY()

	/** Unique identifier for this snapshot */
	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	FString SnapshotID;

	/** Display name for this snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	FString SnapshotName;

	/** Timestamp when snapshot was created */
	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	FDateTime Timestamp;

	/** Current level/map name */
	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	FString LevelName;

	/** Serialized player state data */
	UPROPERTY()
	TArray<uint8> PlayerStateData;

	/** Serialized enemy/AI states */
	UPROPERTY()
	TArray<uint8> EnemyStatesData;

	/** Serialized world state data */
	UPROPERTY()
	TArray<uint8> WorldStateData;

	/** Optional thumbnail data (PNG format) */
	UPROPERTY()
	TArray<uint8> ThumbnailData;

	/** Custom metadata */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	TMap<FString, FString> Metadata;

	FStateSnapshot()
		: Timestamp(FDateTime::Now())
	{
	}
};

/**
 * Options for saving a snapshot
 */
USTRUCT(BlueprintType)
struct DEBUGSTATESNAPSHOT_API FSnapshotSaveOptions
{
	GENERATED_BODY()

	/** Include player state in snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bIncludePlayerState = true;

	/** Include enemy/AI states in snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bIncludeEnemyStates = true;

	/** Include world state in snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bIncludeWorldState = true;

	/** Generate thumbnail for snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bGenerateThumbnail = false;

	/** Custom metadata to include */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	TMap<FString, FString> CustomMetadata;
};

/**
 * Options for loading a snapshot
 */
USTRUCT(BlueprintType)
struct DEBUGSTATESNAPSHOT_API FSnapshotLoadOptions
{
	GENERATED_BODY()

	/** Restore player state from snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bRestorePlayerState = true;

	/** Restore enemy/AI states from snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bRestoreEnemyStates = true;

	/** Restore world state from snapshot */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bRestoreWorldState = true;

	/** Reload level if different from current */
	UPROPERTY(BlueprintReadWrite, Category = "Snapshot")
	bool bReloadLevelIfNeeded = true;
};
