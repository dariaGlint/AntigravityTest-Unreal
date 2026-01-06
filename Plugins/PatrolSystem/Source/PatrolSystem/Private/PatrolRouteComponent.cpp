// Copyright Epic Games, Inc. All Rights Reserved.

#include "PatrolRouteComponent.h"
#include "PatrolWaypoint.h"

UPatrolRouteComponent::UPatrolRouteComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPatrolRouteComponent::BeginPlay()
{
	Super::BeginPlay();
}

APatrolWaypoint* UPatrolRouteComponent::GetNextWaypoint(int32& CurrentIndex, bool& bReversed)
{
	if (!IsRouteValid())
	{
		return nullptr;
	}

	// 有効なウェイポイントのみをフィルタリング
	TArray<APatrolWaypoint*> EnabledWaypoints;
	for (APatrolWaypoint* Waypoint : Waypoints)
	{
		if (Waypoint && Waypoint->bIsEnabled)
		{
			EnabledWaypoints.Add(Waypoint);
		}
	}

	if (EnabledWaypoints.Num() == 0)
	{
		return nullptr;
	}

	// 初回の場合
	if (CurrentIndex < 0)
	{
		CurrentIndex = 0;
		bReversed = false;
		return EnabledWaypoints[0];
	}

	switch (PatrolMode)
	{
	case EPatrolMode::Sequential:
		{
			if (bReversed)
			{
				CurrentIndex--;
				if (CurrentIndex < 0)
				{
					if (bLoopRoute)
					{
						CurrentIndex = EnabledWaypoints.Num() - 1;
						bReversed = false;
					}
					else
					{
						CurrentIndex = 0;
						return nullptr;
					}
				}
			}
			else
			{
				CurrentIndex++;
				if (CurrentIndex >= EnabledWaypoints.Num())
				{
					if (bLoopRoute)
					{
						CurrentIndex = 0;
					}
					else
					{
						CurrentIndex = EnabledWaypoints.Num() - 1;
						return nullptr;
					}
				}
			}
			break;
		}

	case EPatrolMode::PingPong:
		{
			if (bReversed)
			{
				CurrentIndex--;
				if (CurrentIndex < 0)
				{
					CurrentIndex = 1;
					bReversed = false;
				}
			}
			else
			{
				CurrentIndex++;
				if (CurrentIndex >= EnabledWaypoints.Num())
				{
					CurrentIndex = EnabledWaypoints.Num() - 2;
					bReversed = true;
					if (CurrentIndex < 0)
					{
						CurrentIndex = 0;
					}
				}
			}
			break;
		}

	case EPatrolMode::Random:
		{
			CurrentIndex = SelectRandomWaypoint(CurrentIndex);
			break;
		}
	}

	if (CurrentIndex >= 0 && CurrentIndex < EnabledWaypoints.Num())
	{
		return EnabledWaypoints[CurrentIndex];
	}

	return nullptr;
}

int32 UPatrolRouteComponent::FindNearestWaypointIndex(const FVector& Location) const
{
	if (Waypoints.Num() == 0)
	{
		return -1;
	}

	int32 NearestIndex = -1;
	float NearestDistanceSq = MAX_FLT;

	for (int32 i = 0; i < Waypoints.Num(); ++i)
	{
		if (Waypoints[i] && Waypoints[i]->bIsEnabled)
		{
			const float DistanceSq = FVector::DistSquared(Location, Waypoints[i]->GetActorLocation());
			if (DistanceSq < NearestDistanceSq)
			{
				NearestDistanceSq = DistanceSq;
				NearestIndex = i;
			}
		}
	}

	return NearestIndex;
}

int32 UPatrolRouteComponent::GetEnabledWaypointCount() const
{
	int32 Count = 0;
	for (const APatrolWaypoint* Waypoint : Waypoints)
	{
		if (Waypoint && Waypoint->bIsEnabled)
		{
			Count++;
		}
	}
	return Count;
}

bool UPatrolRouteComponent::IsRouteValid() const
{
	return GetEnabledWaypointCount() > 0;
}

int32 UPatrolRouteComponent::SelectRandomWaypoint(int32 CurrentIndex) const
{
	// 有効なウェイポイントのインデックスと優先度を収集
	TArray<int32> EnabledIndices;
	TArray<float> Weights;
	float TotalWeight = 0.0f;

	for (int32 i = 0; i < Waypoints.Num(); ++i)
	{
		// 現在のウェイポイントは除外
		if (i != CurrentIndex && Waypoints[i] && Waypoints[i]->bIsEnabled)
		{
			EnabledIndices.Add(i);
			const float Weight = FMath::Max(0.01f, Waypoints[i]->Priority);
			Weights.Add(Weight);
			TotalWeight += Weight;
		}
	}

	if (EnabledIndices.Num() == 0)
	{
		return CurrentIndex;
	}

	// 重み付きランダム選択
	const float RandomValue = FMath::FRand() * TotalWeight;
	float CurrentWeight = 0.0f;

	for (int32 i = 0; i < EnabledIndices.Num(); ++i)
	{
		CurrentWeight += Weights[i];
		if (RandomValue <= CurrentWeight)
		{
			return EnabledIndices[i];
		}
	}

	// フォールバック
	return EnabledIndices[EnabledIndices.Num() - 1];
}
