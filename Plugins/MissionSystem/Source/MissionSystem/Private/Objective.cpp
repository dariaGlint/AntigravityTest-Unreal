// Copyright Epic Games, Inc. All Rights Reserved.

#include "Objective.h"

UObjective::UObjective()
{
}

void UObjective::Initialize(const FObjectiveData& InObjectiveData)
{
	ObjectiveData = InObjectiveData;
	UpdateProgressPercentage();
}

void UObjective::UpdateProgress(int32 IncrementValue)
{
	if (ObjectiveData.State == EObjectiveState::Completed ||
		ObjectiveData.State == EObjectiveState::Failed)
	{
		return;
	}

	ObjectiveData.CurrentValue = FMath::Clamp(
		ObjectiveData.CurrentValue + IncrementValue,
		0,
		ObjectiveData.TargetValue
	);

	UpdateProgressPercentage();

	OnObjectiveProgressUpdated.Broadcast(ObjectiveData.CurrentValue, ObjectiveData.TargetValue);

	// Auto-complete if target is reached
	if (ObjectiveData.CurrentValue >= ObjectiveData.TargetValue)
	{
		CompleteObjective();
	}
	else if (ObjectiveData.State == EObjectiveState::NotStarted)
	{
		SetState(EObjectiveState::InProgress);
	}
}

void UObjective::SetProgress(int32 NewCurrentValue)
{
	if (ObjectiveData.State == EObjectiveState::Completed ||
		ObjectiveData.State == EObjectiveState::Failed)
	{
		return;
	}

	ObjectiveData.CurrentValue = FMath::Clamp(NewCurrentValue, 0, ObjectiveData.TargetValue);
	UpdateProgressPercentage();

	OnObjectiveProgressUpdated.Broadcast(ObjectiveData.CurrentValue, ObjectiveData.TargetValue);

	// Auto-complete if target is reached
	if (ObjectiveData.CurrentValue >= ObjectiveData.TargetValue)
	{
		CompleteObjective();
	}
	else if (ObjectiveData.State == EObjectiveState::NotStarted)
	{
		SetState(EObjectiveState::InProgress);
	}
}

void UObjective::CompleteObjective()
{
	if (ObjectiveData.State != EObjectiveState::Completed)
	{
		ObjectiveData.CurrentValue = ObjectiveData.TargetValue;
		ObjectiveData.Progress = 1.0f;
		SetState(EObjectiveState::Completed);
	}
}

void UObjective::FailObjective()
{
	if (ObjectiveData.State != EObjectiveState::Failed)
	{
		SetState(EObjectiveState::Failed);
	}
}

void UObjective::ResetObjective()
{
	ObjectiveData.CurrentValue = 0;
	ObjectiveData.Progress = 0.0f;
	SetState(EObjectiveState::NotStarted);
}

void UObjective::SetState(EObjectiveState NewState)
{
	if (ObjectiveData.State != NewState)
	{
		ObjectiveData.State = NewState;
		OnObjectiveStateChanged.Broadcast(NewState);
	}
}

void UObjective::UpdateProgressPercentage()
{
	if (ObjectiveData.TargetValue > 0)
	{
		ObjectiveData.Progress = static_cast<float>(ObjectiveData.CurrentValue) /
								  static_cast<float>(ObjectiveData.TargetValue);
		ObjectiveData.Progress = FMath::Clamp(ObjectiveData.Progress, 0.0f, 1.0f);
	}
	else
	{
		ObjectiveData.Progress = 0.0f;
	}
}
