// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISnapshotable.generated.h"

/**
 * Interface for objects that can be saved/restored in snapshots
 */
UINTERFACE(MinimalAPI, Blueprintable)
class USnapshotable : public UInterface
{
	GENERATED_BODY()
};

class DEBUGSTATESNAPSHOT_API ISnapshotable
{
	GENERATED_BODY()

public:
	/**
	 * Save the current state of this object to a byte array
	 * @param OutData - The serialized state data
	 * @return True if state was successfully saved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Snapshot")
	bool SaveState(TArray<uint8>& OutData);

	/**
	 * Restore the state of this object from a byte array
	 * @param InData - The serialized state data to restore from
	 * @return True if state was successfully restored
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Snapshot")
	bool RestoreState(const TArray<uint8>& InData);

	/**
	 * Get a unique identifier for this snapshotable object
	 * Used to match objects when restoring state
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Snapshot")
	FString GetSnapshotID() const;
};
