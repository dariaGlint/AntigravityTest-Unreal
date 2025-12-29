// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MissionTypes.h"
#include "Objective.h"
#include "Mission.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionStateChanged, EMissionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionObjectiveUpdated, FName, ObjectiveID, EObjectiveState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubMissionCompleted, FName, SubMissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConditionTriggered, FName, NextMissionID);

/**
 * Runtime mission instance that manages objectives and sub-missions
 */
UCLASS(BlueprintType, Blueprintable)
class MISSIONSYSTEM_API UMission : public UObject
{
	GENERATED_BODY()

public:
	UMission();

	/** Initialize the mission with data */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void Initialize(const FMissionData& InMissionData);

	/** Start the mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void StartMission();

	/** Complete the mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void CompleteMission();

	/** Fail the mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void FailMission();

	/** Reset the mission to its initial state */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void ResetMission();

	/** Update progress for a specific objective */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void UpdateObjectiveProgress(FName ObjectiveID, int32 IncrementValue);

	/** Set progress for a specific objective */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void SetObjectiveProgress(FName ObjectiveID, int32 NewCurrentValue);

	/** Complete a specific objective */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void CompleteObjective(FName ObjectiveID);

	/** Fail a specific objective */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void FailObjective(FName ObjectiveID);

	/** Get an objective by ID */
	UFUNCTION(BlueprintPure, Category = "Mission System|Mission")
	UObjective* GetObjective(FName ObjectiveID) const;

	/** Get all objectives */
	UFUNCTION(BlueprintPure, Category = "Mission System|Mission")
	TArray<UObjective*> GetAllObjectives() const { return Objectives; }

	/** Get the current mission data */
	UFUNCTION(BlueprintPure, Category = "Mission System|Mission")
	const FMissionData& GetMissionData() const { return MissionData; }

	/** Get the current state */
	UFUNCTION(BlueprintPure, Category = "Mission System|Mission")
	EMissionState GetState() const { return MissionData.State; }

	/** Check if the mission is complete */
	UFUNCTION(BlueprintPure, Category = "Mission System|Mission")
	bool IsComplete() const { return MissionData.State == EMissionState::Completed; }

	/** Check if all required objectives are complete */
	UFUNCTION(BlueprintPure, Category = "Mission System|Mission")
	bool AreAllRequiredObjectivesComplete() const;

	/** Add a sub-mission ID */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void AddSubMission(FName SubMissionID);

	/** Mark a sub-mission as completed */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	void NotifySubMissionCompleted(FName SubMissionID);

	/** Check conditions and return the next mission ID if any condition is met */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Mission")
	bool EvaluateConditions(FName& OutNextMissionID);

	/** Event fired when the mission state changes */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Mission")
	FOnMissionStateChanged OnMissionStateChanged;

	/** Event fired when an objective is updated */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Mission")
	FOnMissionObjectiveUpdated OnMissionObjectiveUpdated;

	/** Event fired when a sub-mission is completed */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Mission")
	FOnSubMissionCompleted OnSubMissionCompleted;

	/** Event fired when a condition is triggered */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Mission")
	FOnConditionTriggered OnConditionTriggered;

protected:
	/** The mission data */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Mission")
	FMissionData MissionData;

	/** Runtime objective instances */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Mission")
	TArray<UObjective*> Objectives;

	/** Completed sub-missions */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Mission")
	TArray<FName> CompletedSubMissions;

	/** Update the state of the mission */
	void SetState(EMissionState NewState);

	/** Handle objective state change */
	UFUNCTION()
	void OnObjectiveStateChangedInternal(EObjectiveState NewState);

	/** Check if a specific condition is met */
	bool EvaluateCondition(const FMissionCondition& Condition) const;
};
