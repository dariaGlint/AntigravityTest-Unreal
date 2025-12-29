// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreePatrolTask.generated.h"

class UPatrolRouteComponent;
class APatrolWaypoint;
class AAIController;

/** パトロールタスクの状態 */
UENUM()
enum class EPatrolTaskState : uint8
{
	MovingToWaypoint,
	WaitingAtWaypoint
};

/** StateTree用パトロールタスクのインスタンスデータ */
USTRUCT()
struct FStateTreePatrolTaskInstanceData
{
	GENERATED_BODY()

	/** パトロールルートコンポーネント */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UPatrolRouteComponent> PatrolRoute = nullptr;

	/** AIコントローラー */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AAIController> AIController = nullptr;

	/** 移動許容距離 */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 100.0f;

	/** 現在のウェイポイントインデックス */
	UPROPERTY()
	int32 CurrentWaypointIndex = -1;

	/** 逆順フラグ（PingPongモード用） */
	UPROPERTY()
	bool bIsReversed = false;

	/** 現在のタスク状態 */
	UPROPERTY()
	EPatrolTaskState TaskState = EPatrolTaskState::MovingToWaypoint;

	/** 待機開始時刻 */
	UPROPERTY()
	float WaitStartTime = 0.0f;

	/** 現在のターゲットウェイポイント */
	UPROPERTY()
	TObjectPtr<APatrolWaypoint> CurrentWaypoint = nullptr;
};

/**
 * StateTree用パトロールタスク
 * パトロールルートに沿ってAIを移動させる
 */
USTRUCT(meta = (DisplayName = "Patrol Task"))
struct PATROLSYSTEM_API FStateTreePatrolTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreePatrolTaskInstanceData;

	FStateTreePatrolTask() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

protected:
	/** 次のウェイポイントへ移動を開始 */
	bool MoveToNextWaypoint(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData) const;
};
