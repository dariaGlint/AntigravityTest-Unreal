// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MissionTypes.h"
#include "Objective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStateChanged, EObjectiveState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveProgressUpdated, int32, CurrentValue, int32, TargetValue);

/**
 * Runtime objective instance that tracks progress and state
 */
UCLASS(BlueprintType, Blueprintable)
class MISSIONSYSTEM_API UObjective : public UObject
{
	GENERATED_BODY()

public:
	UObjective();

	/** Initialize the objective with data */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Objective")
	void Initialize(const FObjectiveData& InObjectiveData);

	/** Update the progress of this objective */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Objective")
	void UpdateProgress(int32 IncrementValue);

	/** Set the progress directly */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Objective")
	void SetProgress(int32 NewCurrentValue);

	/** Mark the objective as completed */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Objective")
	void CompleteObjective();

	/** Mark the objective as failed */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Objective")
	void FailObjective();

	/** Reset the objective to its initial state */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Objective")
	void ResetObjective();

	/** Get the current objective data */
	UFUNCTION(BlueprintPure, Category = "Mission System|Objective")
	const FObjectiveData& GetObjectiveData() const { return ObjectiveData; }

	/** Get the current state */
	UFUNCTION(BlueprintPure, Category = "Mission System|Objective")
	EObjectiveState GetState() const { return ObjectiveData.State; }

	/** Get the current progress (0.0 to 1.0) */
	UFUNCTION(BlueprintPure, Category = "Mission System|Objective")
	float GetProgress() const { return ObjectiveData.Progress; }

	/** Check if the objective is complete */
	UFUNCTION(BlueprintPure, Category = "Mission System|Objective")
	bool IsComplete() const { return ObjectiveData.State == EObjectiveState::Completed; }

	/** Check if the objective is optional */
	UFUNCTION(BlueprintPure, Category = "Mission System|Objective")
	bool IsOptional() const { return ObjectiveData.bIsOptional; }

	/** Check if the objective is hidden */
	UFUNCTION(BlueprintPure, Category = "Mission System|Objective")
	bool IsHidden() const { return ObjectiveData.bIsHidden; }

	/** Event fired when the objective state changes */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Objective")
	FOnObjectiveStateChanged OnObjectiveStateChanged;

	/** Event fired when the objective progress is updated */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Objective")
	FOnObjectiveProgressUpdated OnObjectiveProgressUpdated;

protected:
	/** The objective data */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Objective")
	FObjectiveData ObjectiveData;

	/** Update the state of the objective */
	void SetState(EObjectiveState NewState);

	/** Calculate and update the progress percentage */
	void UpdateProgressPercentage();
};
