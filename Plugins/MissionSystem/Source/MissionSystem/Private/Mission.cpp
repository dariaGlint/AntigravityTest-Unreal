// Copyright Epic Games, Inc. All Rights Reserved.

#include "Mission.h"

UMission::UMission()
{
}

void UMission::Initialize(const FMissionData& InMissionData)
{
	MissionData = InMissionData;

	// Create objective instances
	Objectives.Empty();
	for (const FObjectiveData& ObjData : MissionData.Objectives)
	{
		UObjective* NewObjective = NewObject<UObjective>(this);
		if (NewObjective)
		{
			NewObjective->Initialize(ObjData);
			NewObjective->OnObjectiveStateChanged.AddDynamic(this, &UMission::OnObjectiveStateChangedInternal);
			Objectives.Add(NewObjective);
		}
	}

	CompletedSubMissions.Empty();
}

void UMission::StartMission()
{
	if (MissionData.State == EMissionState::NotStarted)
	{
		SetState(EMissionState::InProgress);
	}
}

void UMission::CompleteMission()
{
	if (MissionData.State != EMissionState::Completed)
	{
		SetState(EMissionState::Completed);
	}
}

void UMission::FailMission()
{
	if (MissionData.State != EMissionState::Failed)
	{
		SetState(EMissionState::Failed);
	}
}

void UMission::ResetMission()
{
	for (UObjective* Objective : Objectives)
	{
		if (Objective)
		{
			Objective->ResetObjective();
		}
	}

	CompletedSubMissions.Empty();
	SetState(EMissionState::NotStarted);
}

void UMission::UpdateObjectiveProgress(FName ObjectiveID, int32 IncrementValue)
{
	UObjective* Objective = GetObjective(ObjectiveID);
	if (Objective)
	{
		Objective->UpdateProgress(IncrementValue);
	}
}

void UMission::SetObjectiveProgress(FName ObjectiveID, int32 NewCurrentValue)
{
	UObjective* Objective = GetObjective(ObjectiveID);
	if (Objective)
	{
		Objective->SetProgress(NewCurrentValue);
	}
}

void UMission::CompleteObjective(FName ObjectiveID)
{
	UObjective* Objective = GetObjective(ObjectiveID);
	if (Objective)
	{
		Objective->CompleteObjective();
	}
}

void UMission::FailObjective(FName ObjectiveID)
{
	UObjective* Objective = GetObjective(ObjectiveID);
	if (Objective)
	{
		Objective->FailObjective();
	}
}

UObjective* UMission::GetObjective(FName ObjectiveID) const
{
	for (UObjective* Objective : Objectives)
	{
		if (Objective && Objective->GetObjectiveData().ObjectiveID == ObjectiveID)
		{
			return Objective;
		}
	}
	return nullptr;
}

bool UMission::AreAllRequiredObjectivesComplete() const
{
	for (UObjective* Objective : Objectives)
	{
		if (Objective && !Objective->IsOptional())
		{
			if (!Objective->IsComplete())
			{
				return false;
			}
		}
	}
	return true;
}

void UMission::AddSubMission(FName SubMissionID)
{
	if (!MissionData.SubMissionIDs.Contains(SubMissionID))
	{
		MissionData.SubMissionIDs.Add(SubMissionID);
	}
}

void UMission::NotifySubMissionCompleted(FName SubMissionID)
{
	if (!CompletedSubMissions.Contains(SubMissionID))
	{
		CompletedSubMissions.Add(SubMissionID);
		OnSubMissionCompleted.Broadcast(SubMissionID);

		// Check if all sub-missions are complete
		bool bAllSubMissionsComplete = true;
		for (const FName& SubMissionID : MissionData.SubMissionIDs)
		{
			if (!CompletedSubMissions.Contains(SubMissionID))
			{
				bAllSubMissionsComplete = false;
				break;
			}
		}

		// Auto-complete if all objectives and sub-missions are complete
		if (bAllSubMissionsComplete && AreAllRequiredObjectivesComplete())
		{
			CompleteMission();
		}
	}
}

bool UMission::EvaluateConditions(FName& OutNextMissionID)
{
	for (const FMissionCondition& Condition : MissionData.Conditions)
	{
		if (EvaluateCondition(Condition))
		{
			OutNextMissionID = Condition.NextMissionID;
			OnConditionTriggered.Broadcast(OutNextMissionID);
			return true;
		}
	}
	return false;
}

void UMission::SetState(EMissionState NewState)
{
	if (MissionData.State != NewState)
	{
		MissionData.State = NewState;
		OnMissionStateChanged.Broadcast(NewState);
	}
}

void UMission::OnObjectiveStateChangedInternal(EObjectiveState NewState)
{
	// Find which objective changed
	for (UObjective* Objective : Objectives)
	{
		if (Objective && Objective->GetState() == NewState)
		{
			OnMissionObjectiveUpdated.Broadcast(
				Objective->GetObjectiveData().ObjectiveID,
				NewState
			);
			break;
		}
	}

	// Auto-complete if all required objectives are complete and no sub-missions
	if (MissionData.SubMissionIDs.Num() == 0 && AreAllRequiredObjectivesComplete())
	{
		CompleteMission();
	}

	// Check for any conditions that may have been triggered
	FName NextMissionID;
	EvaluateConditions(NextMissionID);
}

bool UMission::EvaluateCondition(const FMissionCondition& Condition) const
{
	switch (Condition.ConditionType)
	{
		case EConditionType::AllObjectivesComplete:
			return AreAllRequiredObjectivesComplete();

		case EConditionType::AnyObjectiveComplete:
		{
			for (UObjective* Objective : Objectives)
			{
				if (Objective && Objective->IsComplete())
				{
					return true;
				}
			}
			return false;
		}

		case EConditionType::SpecificObjectiveComplete:
		{
			UObjective* TargetObjective = GetObjective(Condition.TargetObjectiveID);
			return TargetObjective && TargetObjective->IsComplete();
		}

		case EConditionType::None:
		default:
			return false;
	}
}
