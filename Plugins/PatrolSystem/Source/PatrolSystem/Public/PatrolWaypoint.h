// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolWaypoint.generated.h"

/**
 * ウェイポイントアクター
 * パトロールルート上の目標地点を表す
 */
UCLASS(Blueprintable, BlueprintType)
class PATROLSYSTEM_API APatrolWaypoint : public AActor
{
	GENERATED_BODY()

public:
	APatrolWaypoint();

	/** ウェイポイントに到達したときの待機時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float WaitTime = 2.0f;

	/** このウェイポイントの優先度（ランダムモードで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float Priority = 1.0f;

	/** ウェイポイントが有効かどうか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool bIsEnabled = true;

	/** デバッグ表示用の色 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FColor DebugColor = FColor::Green;

	/** デバッグ表示のサイズ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float DebugDisplayRadius = 50.0f;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	/** エディタでのビジュアル表示用 */
	UPROPERTY()
	class UBillboardComponent* SpriteComponent;
#endif
};
