// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaveSpawnPoint.h"
#include "Components/ArrowComponent.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

AWaveSpawnPoint::AWaveSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

#if WITH_EDITORONLY_DATA
	// Create billboard for editor visibility
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
	Billboard->bHiddenInGame = true;
#endif

	// Create direction arrow
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	DirectionArrow->SetupAttachment(RootComponent);
	DirectionArrow->ArrowColor = FColor::Red;
	DirectionArrow->bIsScreenSizeScaled = true;
}

FTransform AWaveSpawnPoint::GetSpawnTransform() const
{
	return GetActorTransform();
}

bool AWaveSpawnPoint::CanSpawn() const
{
	return bIsEnabled;
}

void AWaveSpawnPoint::OnEnemySpawned()
{
	CurrentOccupants++;
}

void AWaveSpawnPoint::OnEnemyDestroyed()
{
	CurrentOccupants = FMath::Max(0, CurrentOccupants - 1);
}
