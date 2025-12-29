// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateTreePatrolTask.h"
#include "PatrolRouteComponent.h"
#include "PatrolWaypoint.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FStateTreePatrolTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 必要なコンポーネントの検証
	if (!InstanceData.PatrolRoute || !InstanceData.AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!InstanceData.PatrolRoute->IsRouteValid())
	{
		return EStateTreeRunStatus::Failed;
	}

	// 初期化
	InstanceData.CurrentWaypointIndex = -1;
	InstanceData.bIsReversed = false;
	InstanceData.TaskState = EPatrolTaskState::MovingToWaypoint;

	// 最初のウェイポイントから開始しない場合、最も近いウェイポイントを見つける
	if (!InstanceData.PatrolRoute->bStartFromFirstWaypoint)
	{
		APawn* Pawn = InstanceData.AIController->GetPawn();
		if (Pawn)
		{
			InstanceData.CurrentWaypointIndex = InstanceData.PatrolRoute->FindNearestWaypointIndex(Pawn->GetActorLocation());
		}
	}

	// 最初のウェイポイントへ移動開始
	if (!MoveToNextWaypoint(Context, InstanceData))
	{
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreePatrolTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.PatrolRoute || !InstanceData.AIController || !InstanceData.CurrentWaypoint)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* Pawn = InstanceData.AIController->GetPawn();
	if (!Pawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	switch (InstanceData.TaskState)
	{
	case EPatrolTaskState::MovingToWaypoint:
		{
			// ウェイポイントに到達したか確認
			const float DistanceSq = FVector::DistSquared(Pawn->GetActorLocation(), InstanceData.CurrentWaypoint->GetActorLocation());
			const float AcceptanceRadiusSq = InstanceData.AcceptanceRadius * InstanceData.AcceptanceRadius;

			if (DistanceSq <= AcceptanceRadiusSq)
			{
				// 待機状態に遷移
				InstanceData.TaskState = EPatrolTaskState::WaitingAtWaypoint;
				InstanceData.WaitStartTime = Context.GetWorld()->GetTimeSeconds();

				// 移動を停止
				InstanceData.AIController->StopMovement();
			}
			break;
		}

	case EPatrolTaskState::WaitingAtWaypoint:
		{
			// 待機時間が経過したか確認
			const float CurrentTime = Context.GetWorld()->GetTimeSeconds();
			const float ElapsedTime = CurrentTime - InstanceData.WaitStartTime;

			if (ElapsedTime >= InstanceData.CurrentWaypoint->WaitTime)
			{
				// 次のウェイポイントへ移動
				if (!MoveToNextWaypoint(Context, InstanceData))
				{
					return EStateTreeRunStatus::Succeeded; // パトロール完了
				}

				InstanceData.TaskState = EPatrolTaskState::MovingToWaypoint;
			}
			break;
		}
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreePatrolTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 移動を停止
	if (InstanceData.AIController)
	{
		InstanceData.AIController->StopMovement();
	}
}

bool FStateTreePatrolTask::MoveToNextWaypoint(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData) const
{
	if (!InstanceData.PatrolRoute || !InstanceData.AIController)
	{
		return false;
	}

	// 次のウェイポイントを取得
	APatrolWaypoint* NextWaypoint = InstanceData.PatrolRoute->GetNextWaypoint(
		InstanceData.CurrentWaypointIndex,
		InstanceData.bIsReversed
	);

	if (!NextWaypoint)
	{
		return false;
	}

	InstanceData.CurrentWaypoint = NextWaypoint;

	// ウェイポイントへの移動を開始
	const FVector TargetLocation = NextWaypoint->GetActorLocation();

	// カスタム移動速度が設定されている場合は適用
	if (InstanceData.PatrolRoute->MovementSpeed > 0.0f)
	{
		if (APawn* Pawn = InstanceData.AIController->GetPawn())
		{
			if (UCharacterMovementComponent* MovementComp = Pawn->FindComponentByClass<UCharacterMovementComponent>())
			{
				MovementComp->MaxWalkSpeed = InstanceData.PatrolRoute->MovementSpeed;
			}
		}
	}

	const EPathFollowingRequestResult::Type Result = InstanceData.AIController->MoveToLocation(
		TargetLocation,
		InstanceData.AcceptanceRadius,
		true, // bStopOnOverlap
		true, // bUsePathfinding
		false, // bProjectDestinationToNavigation
		true, // bCanStrafe
		nullptr, // FilterClass
		true // bAllowPartialPath
	);

	return Result != EPathFollowingRequestResult::Failed;
}
