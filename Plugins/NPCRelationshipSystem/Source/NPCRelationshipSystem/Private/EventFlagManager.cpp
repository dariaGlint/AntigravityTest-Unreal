#include "EventFlagManager.h"

void UEventFlagManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EventFlags.Empty();
}

void UEventFlagManager::Deinitialize()
{
	EventFlags.Empty();
	Super::Deinitialize();
}

void UEventFlagManager::SetEventFlag(FName FlagName, bool bValue)
{
	if (FlagName.IsNone())
	{
		return;
	}

	bool bOldValue = GetEventFlag(FlagName);
	EventFlags.Add(FlagName, bValue);

	if (bOldValue != bValue)
	{
		OnEventFlagChanged.Broadcast(FlagName, bValue);
	}
}

bool UEventFlagManager::GetEventFlag(FName FlagName) const
{
	if (const bool* Value = EventFlags.Find(FlagName))
	{
		return *Value;
	}
	return false;
}

void UEventFlagManager::ToggleEventFlag(FName FlagName)
{
	if (FlagName.IsNone())
	{
		return;
	}

	bool bCurrentValue = GetEventFlag(FlagName);
	SetEventFlag(FlagName, !bCurrentValue);
}

void UEventFlagManager::ClearEventFlag(FName FlagName)
{
	if (FlagName.IsNone())
	{
		return;
	}

	EventFlags.Remove(FlagName);
	OnEventFlagChanged.Broadcast(FlagName, false);
}

void UEventFlagManager::ClearAllEventFlags()
{
	TArray<FName> FlagNames;
	EventFlags.GetKeys(FlagNames);

	EventFlags.Empty();

	for (const FName& FlagName : FlagNames)
	{
		OnEventFlagChanged.Broadcast(FlagName, false);
	}
}

bool UEventFlagManager::HasEventFlag(FName FlagName) const
{
	return EventFlags.Contains(FlagName);
}

TMap<FName, bool> UEventFlagManager::GetAllEventFlags() const
{
	return EventFlags;
}

bool UEventFlagManager::CheckAllFlags(const TArray<FName>& FlagNames) const
{
	if (FlagNames.Num() == 0)
	{
		return true;
	}

	for (const FName& FlagName : FlagNames)
	{
		if (!GetEventFlag(FlagName))
		{
			return false;
		}
	}

	return true;
}

bool UEventFlagManager::CheckAnyFlag(const TArray<FName>& FlagNames) const
{
	if (FlagNames.Num() == 0)
	{
		return false;
	}

	for (const FName& FlagName : FlagNames)
	{
		if (GetEventFlag(FlagName))
		{
			return true;
		}
	}

	return false;
}
