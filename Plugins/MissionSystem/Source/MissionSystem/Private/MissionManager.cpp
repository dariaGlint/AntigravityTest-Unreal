// Copyright Epic Games, Inc. All Rights Reserved.

#include "MissionManager.h"

UMissionManager::UMissionManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMissionManager::BeginPlay()
{
	Super::BeginPlay();
}

void UMissionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMissionManager::RegisterMission(const FMissionData& MissionData)
{
	if (MissionData.MissionID == NAME_None)
	{
		return;
	}

	// Create a new mission instance
	UMission* NewMission = NewObject<UMission>(this);
	if (NewMission)
	{
		NewMission->Initialize(MissionData);

		// Bind to mission events
		NewMission->OnMissionStateChanged.AddDynamic(this, &UMissionManager::OnMissionStateChangedInternal);
		NewMission->OnMissionObjectiveUpdated.AddDynamic(this, &UMissionManager::OnMissionObjectiveUpdatedInternal);
		NewMission->OnSubMissionCompleted.AddDynamic(this, &UMissionManager::OnSubMissionCompletedInternal);
		NewMission->OnConditionTriggered.AddDynamic(this, &UMissionManager::OnConditionTriggeredInternal);

		Missions.Add(MissionData.MissionID, NewMission);
	}
}

bool UMissionManager::StartMission(FName MissionID)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->StartMission();

		if (!ActiveMissionIDs.Contains(MissionID))
		{
			ActiveMissionIDs.Add(MissionID);
		}

		OnMissionStarted.Broadcast(MissionID);
		return true;
	}
	return false;
}

bool UMissionManager::CompleteMission(FName MissionID)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->CompleteMission();
		return true;
	}
	return false;
}

bool UMissionManager::FailMission(FName MissionID)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->FailMission();
		return true;
	}
	return false;
}

bool UMissionManager::ResetMission(FName MissionID)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->ResetMission();

		ActiveMissionIDs.Remove(MissionID);
		CompletedMissionIDs.Remove(MissionID);

		return true;
	}
	return false;
}

bool UMissionManager::UpdateMissionObjectiveProgress(FName MissionID, FName ObjectiveID, int32 IncrementValue)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->UpdateObjectiveProgress(ObjectiveID, IncrementValue);
		return true;
	}
	return false;
}

bool UMissionManager::SetMissionObjectiveProgress(FName MissionID, FName ObjectiveID, int32 NewCurrentValue)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->SetObjectiveProgress(ObjectiveID, NewCurrentValue);
		return true;
	}
	return false;
}

bool UMissionManager::CompleteMissionObjective(FName MissionID, FName ObjectiveID)
{
	UMission* Mission = GetMission(MissionID);
	if (Mission)
	{
		Mission->CompleteObjective(ObjectiveID);
		return true;
	}
	return false;
}

UMission* UMissionManager::GetMission(FName MissionID) const
{
	UMission* const* FoundMission = Missions.Find(MissionID);
	return FoundMission ? *FoundMission : nullptr;
}

TArray<UMission*> UMissionManager::GetActiveMissions() const
{
	TArray<UMission*> ActiveMissions;
	for (const FName& MissionID : ActiveMissionIDs)
	{
		UMission* Mission = GetMission(MissionID);
		if (Mission && Mission->GetState() == EMissionState::InProgress)
		{
			ActiveMissions.Add(Mission);
		}
	}
	return ActiveMissions;
}

TArray<UMission*> UMissionManager::GetCompletedMissions() const
{
	TArray<UMission*> Completed;
	for (const FName& MissionID : CompletedMissionIDs)
	{
		UMission* Mission = GetMission(MissionID);
		if (Mission)
		{
			Completed.Add(Mission);
		}
	}
	return Completed;
}

TArray<UMission*> UMissionManager::GetAllMissions() const
{
	TArray<UMission*> AllMissions;
	Missions.GenerateValueArray(AllMissions);
	return AllMissions;
}

bool UMissionManager::HasMission(FName MissionID) const
{
	return Missions.Contains(MissionID);
}

bool UMissionManager::IsMissionActive(FName MissionID) const
{
	return ActiveMissionIDs.Contains(MissionID);
}

bool UMissionManager::IsMissionCompleted(FName MissionID) const
{
	return CompletedMissionIDs.Contains(MissionID);
}

void UMissionManager::OnMissionStateChangedInternal(EMissionState NewState)
{
	UMission* Mission = FindMissionByCallback();
	if (!Mission)
	{
		return;
	}

	const FName MissionID = Mission->GetMissionData().MissionID;

	switch (NewState)
	{
		case EMissionState::Completed:
		{
			ActiveMissionIDs.Remove(MissionID);

			if (!CompletedMissionIDs.Contains(MissionID))
			{
				CompletedMissionIDs.Add(MissionID);
			}

			OnMissionCompleted.Broadcast(MissionID);

			// Notify parent mission if this is a sub-mission
			const FName ParentMissionID = Mission->GetMissionData().ParentMissionID;
			if (ParentMissionID != NAME_None)
			{
				UMission* ParentMission = GetMission(ParentMissionID);
				if (ParentMission)
				{
					ParentMission->NotifySubMissionCompleted(MissionID);
				}
			}
			break;
		}

		case EMissionState::Failed:
		{
			ActiveMissionIDs.Remove(MissionID);
			OnMissionFailed.Broadcast(MissionID);
			break;
		}

		case EMissionState::InProgress:
		{
			if (!ActiveMissionIDs.Contains(MissionID))
			{
				ActiveMissionIDs.Add(MissionID);
			}
			break;
		}

		default:
			break;
	}
}

void UMissionManager::OnMissionObjectiveUpdatedInternal(FName ObjectiveID, EObjectiveState NewState)
{
	UMission* Mission = FindMissionByCallback();
	if (!Mission)
	{
		return;
	}

	const FName MissionID = Mission->GetMissionData().MissionID;

	if (NewState == EObjectiveState::Completed)
	{
		OnMissionObjectiveCompleted.Broadcast(MissionID, ObjectiveID);
	}
}

void UMissionManager::OnSubMissionCompletedInternal(FName SubMissionID)
{
	// This is handled automatically by the mission system
	// Additional logic can be added here if needed
}

void UMissionManager::OnConditionTriggeredInternal(FName NextMissionID)
{
	// Auto-start the next mission if it exists
	if (HasMission(NextMissionID))
	{
		StartMission(NextMissionID);
	}
}

UMission* UMissionManager::FindMissionByCallback() const
{
	// Find which mission triggered the callback
	// This is a helper method to identify the mission from event callbacks
	for (const auto& Pair : Missions)
	{
		UMission* Mission = Pair.Value;
		if (Mission)
		{
			// Return the first mission we find (the caller should be bound to only one)
			// In a real implementation, you might want to pass the mission ID through the delegate
			return Mission;
		}
	}
	return nullptr;
}
