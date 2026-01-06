// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MissionTypes.generated.h"

/**
 * Enum representing the current state of an objective
 */
UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),
	InProgress UMETA(DisplayName = "In Progress"),
	Completed UMETA(DisplayName = "Completed"),
	Failed UMETA(DisplayName = "Failed")
};

/**
 * Enum representing the current state of a mission
 */
UENUM(BlueprintType)
enum class EMissionState : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),
	InProgress UMETA(DisplayName = "In Progress"),
	Completed UMETA(DisplayName = "Completed"),
	Failed UMETA(DisplayName = "Failed")
};

/**
 * Enum for conditional branch types
 */
UENUM(BlueprintType)
enum class EConditionType : uint8
{
	None UMETA(DisplayName = "None"),
	AllObjectivesComplete UMETA(DisplayName = "All Objectives Complete"),
	AnyObjectiveComplete UMETA(DisplayName = "Any Objective Complete"),
	SpecificObjectiveComplete UMETA(DisplayName = "Specific Objective Complete"),
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Structure representing a single objective within a mission
 */
USTRUCT(BlueprintType)
struct MISSIONSYSTEM_API FObjectiveData
{
	GENERATED_BODY()

	/** Unique identifier for this objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FName ObjectiveID;

	/** Display name for the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText DisplayName;

	/** Description of the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText Description;

	/** Current state of the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	EObjectiveState State;

	/** Current progress value (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	float Progress;

	/** Target value for completion (e.g., number of items to collect) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	int32 TargetValue;

	/** Current value (e.g., number of items collected) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	int32 CurrentValue;

	/** Whether this objective is optional */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsOptional;

	/** Whether this objective is hidden from the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsHidden;

	FObjectiveData()
		: ObjectiveID(NAME_None)
		, State(EObjectiveState::NotStarted)
		, Progress(0.0f)
		, TargetValue(1)
		, CurrentValue(0)
		, bIsOptional(false)
		, bIsHidden(false)
	{
	}
};

/**
 * Structure representing a conditional branch in mission flow
 */
USTRUCT(BlueprintType)
struct MISSIONSYSTEM_API FMissionCondition
{
	GENERATED_BODY()

	/** Type of condition to check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EConditionType ConditionType;

	/** ID of the objective to check (for SpecificObjectiveComplete) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName TargetObjectiveID;

	/** ID of the mission to activate if condition is met */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName NextMissionID;

	FMissionCondition()
		: ConditionType(EConditionType::None)
		, TargetObjectiveID(NAME_None)
		, NextMissionID(NAME_None)
	{
	}
};

/**
 * Structure representing a complete mission with objectives and sub-missions
 */
USTRUCT(BlueprintType)
struct MISSIONSYSTEM_API FMissionData
{
	GENERATED_BODY()

	/** Unique identifier for this mission */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FName MissionID;

	/** Display name for the mission */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FText DisplayName;

	/** Description of the mission */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FText Description;

	/** Current state of the mission */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	EMissionState State;

	/** List of objectives for this mission */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FObjectiveData> Objectives;

	/** IDs of sub-missions that are part of this mission */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FName> SubMissionIDs;

	/** Conditional branches for mission flow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FMissionCondition> Conditions;

	/** ID of the parent mission (if this is a sub-mission) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FName ParentMissionID;

	/** Priority level for mission ordering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	int32 Priority;

	FMissionData()
		: MissionID(NAME_None)
		, State(EMissionState::NotStarted)
		, ParentMissionID(NAME_None)
		, Priority(0)
	{
	}
};
