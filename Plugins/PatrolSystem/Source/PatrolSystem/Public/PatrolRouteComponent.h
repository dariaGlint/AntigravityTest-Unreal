// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolRouteComponent.generated.h"

class APatrolWaypoint;

/** パトロールモード */
UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
	/** 順序通りに巡回 */
	Sequential UMETA(DisplayName = "Sequential"),

	/** ランダムに巡回 */
	Random UMETA(DisplayName = "Random"),

	/** 順序通りに巡回し、最後まで到達したら逆順で戻る */
	PingPong UMETA(DisplayName = "PingPong")
};

/**
 * パトロールルートコンポーネント
 * ウェイポイントのリストを管理し、巡回ルートを定義する
 */
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent), Blueprintable)
class PATROLSYSTEM_API UPatrolRouteComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPatrolRouteComponent();

	/** ウェイポイントのリスト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<APatrolWaypoint*> Waypoints;

	/** パトロールモード */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	EPatrolMode PatrolMode = EPatrolMode::Sequential;

	/** ループするかどうか（Sequentialモード時） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", meta = (EditCondition = "PatrolMode == EPatrolMode::Sequential", EditConditionHides))
	bool bLoopRoute = true;

	/** 最初のウェイポイントから開始するかどうか（falseの場合、最も近いウェイポイントから開始） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool bStartFromFirstWaypoint = true;

	/** ウェイポイント間の移動速度（-1の場合、AIの既定速度を使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float MovementSpeed = -1.0f;

	/** 次のウェイポイントを取得 */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	APatrolWaypoint* GetNextWaypoint(int32& CurrentIndex, bool& bReversed);

	/** 最も近いウェイポイントのインデックスを取得 */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	int32 FindNearestWaypointIndex(const FVector& Location) const;

	/** 有効なウェイポイントの数を取得 */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	int32 GetEnabledWaypointCount() const;

	/** ルートが有効かどうか */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	bool IsRouteValid() const;

protected:
	virtual void BeginPlay() override;

private:
	/** ランダムモード用の次のウェイポイントを選択 */
	int32 SelectRandomWaypoint(int32 CurrentIndex) const;
};
