#include "GimmickManager.h"
#include "InteractableBase.h"
#include "DoorGimmick.h"
#include "PuzzleBase.h"
#include "EngineUtils.h"

void UGimmickManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGimmickManager::Deinitialize()
{
	RegisteredGimmicks.Empty();
	GimmickMap.Empty();

	Super::Deinitialize();
}

void UGimmickManager::RegisterGimmick(AInteractableBase* Gimmick)
{
	if (!Gimmick)
	{
		return;
	}

	if (!RegisteredGimmicks.Contains(Gimmick))
	{
		RegisteredGimmicks.Add(Gimmick);

		// Also add to map if it has a valid name
		FName GimmickName = FName(*Gimmick->GetName());
		if (!GimmickName.IsNone())
		{
			GimmickMap.Add(GimmickName, Gimmick);
		}
	}
}

void UGimmickManager::UnregisterGimmick(AInteractableBase* Gimmick)
{
	if (!Gimmick)
	{
		return;
	}

	RegisteredGimmicks.Remove(Gimmick);

	FName GimmickName = FName(*Gimmick->GetName());
	if (GimmickMap.Contains(GimmickName))
	{
		GimmickMap.Remove(GimmickName);
	}
}

AInteractableBase* UGimmickManager::FindGimmickByID(FName GimmickID) const
{
	if (GimmickMap.Contains(GimmickID))
	{
		return GimmickMap[GimmickID];
	}
	return nullptr;
}

TArray<ADoorGimmick*> UGimmickManager::GetAllDoors() const
{
	TArray<ADoorGimmick*> Doors;

	for (AInteractableBase* Gimmick : RegisteredGimmicks)
	{
		if (ADoorGimmick* Door = Cast<ADoorGimmick>(Gimmick))
		{
			Doors.Add(Door);
		}
	}

	return Doors;
}

TArray<APuzzleBase*> UGimmickManager::GetAllPuzzles() const
{
	TArray<APuzzleBase*> Puzzles;

	for (AInteractableBase* Gimmick : RegisteredGimmicks)
	{
		if (APuzzleBase* Puzzle = Cast<APuzzleBase>(Gimmick))
		{
			Puzzles.Add(Puzzle);
		}
	}

	return Puzzles;
}

AInteractableBase* UGimmickManager::GetNearestInteractable(AActor* Interactor, float MaxDistance) const
{
	if (!Interactor)
	{
		return nullptr;
	}

	AInteractableBase* NearestGimmick = nullptr;
	float NearestDistance = MaxDistance;

	for (AInteractableBase* Gimmick : RegisteredGimmicks)
	{
		if (!Gimmick || !Gimmick->CanInteract(Interactor))
		{
			continue;
		}

		const float Distance = FVector::Dist(Gimmick->GetActorLocation(), Interactor->GetActorLocation());

		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestGimmick = Gimmick;
		}
	}

	return NearestGimmick;
}

void UGimmickManager::SaveAllGimmickStates()
{
	// TODO: Implement save system integration
	// This would serialize the state of all gimmicks to a save file
}

void UGimmickManager::LoadAllGimmickStates()
{
	// TODO: Implement save system integration
	// This would deserialize and restore gimmick states from a save file
}
