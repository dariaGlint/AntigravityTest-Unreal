// Copyright Epic Games, Inc. All Rights Reserved.

#include "PatrolWaypoint.h"
#include "Components/BillboardComponent.h"

APatrolWaypoint::APatrolWaypoint()
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	// エディタでの可視化用のスプライトコンポーネント
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		SpriteComponent->SetupAttachment(RootComponent);
		SpriteComponent->bIsScreenSizeScaled = true;
	}
#endif
}

void APatrolWaypoint::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
void APatrolWaypoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// プロパティ変更時の処理（必要に応じて追加）
}
#endif
