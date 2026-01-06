#include "FormationLibrary.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

FFormationConfig UFormationLibrary::MakeFormationConfig(EFormationType FormationType, float Spacing)
{
	FFormationConfig Config;
	Config.FormationType = FormationType;
	Config.MemberSpacing = Spacing;
	Config.bEnableObstacleAvoidance = true;
	Config.MaxFollowDistance = 5000.0f;
	return Config;
}

FFormationConfig UFormationLibrary::MakeCustomFormationConfig(const TArray<FFormationSlot>& Slots, float MaxFollowDistance)
{
	FFormationConfig Config;
	Config.FormationType = EFormationType::Custom;
	Config.Slots = Slots;
	Config.bEnableObstacleAvoidance = true;
	Config.MaxFollowDistance = MaxFollowDistance;
	return Config;
}

FVector UFormationLibrary::CalculateFormationSlotPosition(const FVector& LeaderPosition, const FRotator& LeaderRotation, const FFormationSlot& Slot)
{
	FVector WorldOffset = LeaderRotation.RotateVector(Slot.RelativePosition);
	return LeaderPosition + WorldOffset;
}

EFormationType UFormationLibrary::GetRecommendedFormationType(bool bInCombat, int32 PartySize)
{
	if (bInCombat)
	{
		// Use combat spread in combat for better positioning
		return EFormationType::CombatSpread;
	}
	else
	{
		// Use different formations based on party size
		if (PartySize <= 3)
		{
			return EFormationType::Line;
		}
		else if (PartySize <= 6)
		{
			return EFormationType::Column;
		}
		else
		{
			return EFormationType::Wedge;
		}
	}
}

bool UFormationLibrary::IsFormationPositionValid(UObject* WorldContextObject, const FVector& Position, float TestRadius)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	if (!NavSys)
	{
		// If no nav system, assume position is valid
		return true;
	}

	// Check if position is on navmesh
	FNavLocation NavLocation;
	bool bOnNavMesh = NavSys->ProjectPointToNavigation(Position, NavLocation, FVector(TestRadius, TestRadius, TestRadius));

	return bOnNavMesh;
}

FFormationConfig UFormationLibrary::LerpFormationConfig(const FFormationConfig& A, const FFormationConfig& B, float Alpha)
{
	FFormationConfig Result;

	// Can't lerp between different formation types, so choose based on alpha
	Result.FormationType = (Alpha < 0.5f) ? A.FormationType : B.FormationType;

	// Lerp numeric values
	Result.MemberSpacing = FMath::Lerp(A.MemberSpacing, B.MemberSpacing, Alpha);
	Result.MaxFollowDistance = FMath::Lerp(A.MaxFollowDistance, B.MaxFollowDistance, Alpha);

	// Boolean values - choose based on alpha
	Result.bEnableObstacleAvoidance = (Alpha < 0.5f) ? A.bEnableObstacleAvoidance : B.bEnableObstacleAvoidance;

	// Lerp slot positions if both have the same number of slots
	if (A.Slots.Num() == B.Slots.Num())
	{
		Result.Slots.Reserve(A.Slots.Num());
		for (int32 i = 0; i < A.Slots.Num(); ++i)
		{
			FFormationSlot LerpedSlot;
			LerpedSlot.RelativePosition = FMath::Lerp(A.Slots[i].RelativePosition, B.Slots[i].RelativePosition, Alpha);
			LerpedSlot.Priority = (Alpha < 0.5f) ? A.Slots[i].Priority : B.Slots[i].Priority;
			LerpedSlot.RelativeRotation = FMath::Lerp(A.Slots[i].RelativeRotation, B.Slots[i].RelativeRotation, Alpha);
			Result.Slots.Add(LerpedSlot);
		}
	}
	else
	{
		// If slot counts differ, use the target configuration's slots
		Result.Slots = B.Slots;
	}

	return Result;
}
