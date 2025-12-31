// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SnapshotInputHandler.generated.h"

class UStateSnapshotSubsystem;
class USnapshotManagerWidget;
class UInputAction;
class UInputMappingContext;

/**
 * Component for handling snapshot-related input (F5/F9 keys)
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEBUGSTATESNAPSHOT_API USnapshotInputHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	USnapshotInputHandler();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** Quick save input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> QuickSaveAction;

	/** Quick load input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> QuickLoadAction;

	/** Toggle snapshot manager UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleManagerAction;

	/** Input mapping context for snapshot controls */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> SnapshotMappingContext;

	/** Widget class for snapshot manager */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USnapshotManagerWidget> SnapshotManagerWidgetClass;

protected:
	/** Handle quick save input */
	void OnQuickSave();

	/** Handle quick load input */
	void OnQuickLoad();

	/** Handle toggle manager input */
	void OnToggleManager();

	/** Show notification on screen */
	void ShowNotification(const FString& Message, bool bSuccess);

private:
	UPROPERTY()
	TObjectPtr<UStateSnapshotSubsystem> SnapshotSubsystem;

	UPROPERTY()
	TObjectPtr<USnapshotManagerWidget> ManagerWidget;
};
