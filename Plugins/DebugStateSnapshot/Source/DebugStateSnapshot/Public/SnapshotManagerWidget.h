// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StateSnapshotTypes.h"
#include "SnapshotManagerWidget.generated.h"

class UStateSnapshotSubsystem;

/**
 * UI Widget for managing snapshots
 */
UCLASS(Abstract, Blueprintable)
class DEBUGSTATESNAPSHOT_API USnapshotManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Refresh the snapshot list */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void RefreshSnapshotList();

	/** Save a new snapshot */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void OnSaveSnapshot(const FString& SlotName);

	/** Load selected snapshot */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void OnLoadSnapshot(const FString& SlotName);

	/** Delete selected snapshot */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void OnDeleteSnapshot(const FString& SlotName);

	/** Export snapshot to file */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void OnExportSnapshot(const FString& SlotName, const FString& FilePath);

	/** Import snapshot from file */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void OnImportSnapshot(const FString& FilePath, const FString& SlotName);

	/** Toggle widget visibility */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	void ToggleVisibility();

protected:
	/** Called when snapshot list is refreshed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Snapshot")
	void BP_OnSnapshotListRefreshed(const TArray<FString>& SlotNames);

	/** Called when a snapshot operation completes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Snapshot")
	void BP_OnSnapshotOperationComplete(bool bSuccess, const FString& Message);

	/** Get preview info for a snapshot */
	UFUNCTION(BlueprintCallable, Category = "Snapshot")
	bool GetSnapshotPreviewInfo(const FString& SlotName, FStateSnapshot& OutInfo);

private:
	UPROPERTY()
	TObjectPtr<UStateSnapshotSubsystem> SnapshotSubsystem;
};
