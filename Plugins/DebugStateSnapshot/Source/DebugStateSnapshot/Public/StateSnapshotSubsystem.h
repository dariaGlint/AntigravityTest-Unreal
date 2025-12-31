// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StateSnapshotTypes.h"
#include "StateSnapshotSubsystem.generated.h"

/**
 * Game Instance Subsystem for managing state snapshots
 */
UCLASS()
class DEBUGSTATESNAPSHOT_API UStateSnapshotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem lifecycle
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Save a snapshot of the current game state
	 * @param SlotName - Name of the slot to save to
	 * @param Options - Save options
	 * @return True if snapshot was successfully saved
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool SaveSnapshot(const FString& SlotName, const FSnapshotSaveOptions& Options);

	/**
	 * Load a snapshot and restore game state
	 * @param SlotName - Name of the slot to load from
	 * @param Options - Load options
	 * @return True if snapshot was successfully loaded
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool LoadSnapshot(const FString& SlotName, const FSnapshotLoadOptions& Options);

	/**
	 * Quick save to the default quick save slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool QuickSave();

	/**
	 * Quick load from the default quick save slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool QuickLoad();

	/**
	 * Get list of available snapshot slots
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	TArray<FString> GetSnapshotSlots() const;

	/**
	 * Get snapshot info without loading it
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool GetSnapshotInfo(const FString& SlotName, FStateSnapshot& OutSnapshot) const;

	/**
	 * Delete a snapshot
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool DeleteSnapshot(const FString& SlotName);

	/**
	 * Export snapshot to a file
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool ExportSnapshot(const FString& SlotName, const FString& FilePath);

	/**
	 * Import snapshot from a file
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool ImportSnapshot(const FString& FilePath, const FString& SlotName);

	/**
	 * Check if a snapshot exists in the given slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool DoesSnapshotExist(const FString& SlotName) const;

protected:
	/** Save snapshot to disk */
	bool SaveSnapshotToDisk(const FString& SlotName, const FStateSnapshot& Snapshot);

	/** Load snapshot from disk */
	bool LoadSnapshotFromDisk(const FString& SlotName, FStateSnapshot& OutSnapshot) const;

	/** Get the save directory path */
	FString GetSaveDirectory() const;

	/** Get the full path for a snapshot file */
	FString GetSnapshotFilePath(const FString& SlotName) const;

	/** Serialize player state */
	bool SerializePlayerState(TArray<uint8>& OutData);

	/** Deserialize player state */
	bool DeserializePlayerState(const TArray<uint8>& InData);

	/** Serialize enemy states */
	bool SerializeEnemyStates(TArray<uint8>& OutData);

	/** Deserialize enemy states */
	bool DeserializeEnemyStates(const TArray<uint8>& InData);

	/** Serialize world state */
	bool SerializeWorldState(TArray<uint8>& OutData);

	/** Deserialize world state */
	bool DeserializeWorldState(const TArray<uint8>& InData);

	/** Generate thumbnail for current view */
	bool GenerateThumbnail(TArray<uint8>& OutThumbnailData);

private:
	/** Quick save slot name */
	static const FString QuickSaveSlotName;

	/** File extension for snapshot files */
	static const FString SnapshotFileExtension;
};
