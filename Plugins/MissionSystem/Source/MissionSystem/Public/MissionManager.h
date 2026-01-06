// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MissionTypes.h"
#include "Mission.h"
#include "MissionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionStarted, FName, MissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionCompleted, FName, MissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionFailed, FName, MissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionObjectiveCompleted, FName, MissionID, FName, ObjectiveID);

/**
 * Component that manages all missions and objectives in the game
 * Handles mission flow, progress tracking, sub-missions, and conditional branching
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MISSIONSYSTEM_API UMissionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UMissionManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Register a new mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	void RegisterMission(const FMissionData& MissionData);

	/** Start a mission by ID */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool StartMission(FName MissionID);

	/** Complete a mission by ID */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool CompleteMission(FName MissionID);

	/** Fail a mission by ID */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool FailMission(FName MissionID);

	/** Reset a mission by ID */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool ResetMission(FName MissionID);

	/** Update objective progress for a specific mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool UpdateMissionObjectiveProgress(FName MissionID, FName ObjectiveID, int32 IncrementValue);

	/** Set objective progress for a specific mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool SetMissionObjectiveProgress(FName MissionID, FName ObjectiveID, int32 NewCurrentValue);

	/** Complete a specific objective in a mission */
	UFUNCTION(BlueprintCallable, Category = "Mission System|Manager")
	bool CompleteMissionObjective(FName MissionID, FName ObjectiveID);

	/** Get a mission by ID */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	UMission* GetMission(FName MissionID) const;

	/** Get all active missions */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	TArray<UMission*> GetActiveMissions() const;

	/** Get all completed missions */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	TArray<UMission*> GetCompletedMissions() const;

	/** Get all missions */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	TArray<UMission*> GetAllMissions() const;

	/** Check if a mission exists */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	bool HasMission(FName MissionID) const;

	/** Check if a mission is active */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	bool IsMissionActive(FName MissionID) const;

	/** Check if a mission is completed */
	UFUNCTION(BlueprintPure, Category = "Mission System|Manager")
	bool IsMissionCompleted(FName MissionID) const;

	/** Event fired when a mission is started */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Manager")
	FOnMissionStarted OnMissionStarted;

	/** Event fired when a mission is completed */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Manager")
	FOnMissionCompleted OnMissionCompleted;

	/** Event fired when a mission is failed */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Manager")
	FOnMissionFailed OnMissionFailed;

	/** Event fired when an objective is completed */
	UPROPERTY(BlueprintAssignable, Category = "Mission System|Manager")
	FOnMissionObjectiveCompleted OnMissionObjectiveCompleted;

protected:
	/** All registered missions */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Manager")
	TMap<FName, UMission*> Missions;

	/** Active mission IDs */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Manager")
	TArray<FName> ActiveMissionIDs;

	/** Completed mission IDs */
	UPROPERTY(BlueprintReadOnly, Category = "Mission System|Manager")
	TArray<FName> CompletedMissionIDs;

	/** Handle mission state change */
	UFUNCTION()
	void OnMissionStateChangedInternal(EMissionState NewState);

	/** Handle objective state change */
	UFUNCTION()
	void OnMissionObjectiveUpdatedInternal(FName ObjectiveID, EObjectiveState NewState);

	/** Handle sub-mission completion */
	UFUNCTION()
	void OnSubMissionCompletedInternal(FName SubMissionID);

	/** Handle condition triggered */
	UFUNCTION()
	void OnConditionTriggeredInternal(FName NextMissionID);

	/** Find the mission that triggered an event (helper for callbacks) */
	UMission* FindMissionByCallback() const;
};
