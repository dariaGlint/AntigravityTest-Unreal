// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AntigravityGoalGameMode.h"
#include "GameFramework/Character.h"

// Sets default values
AGoalActor::AGoalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize Box Component
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));

	// Initialize Mesh Component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision")); // Visual only
}

// Called when the game starts or when spawned
void AGoalActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind overlap event
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGoalActor::OnOverlapBegin);
}

void AGoalActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is a character (or player pawn)
	if (OtherActor && OtherActor != this && OtherActor->IsA(ACharacter::StaticClass()))
	{
		// Get GameMode and cast to custom GameMode
		if (AAntigravityGoalGameMode* GameMode = Cast<AAntigravityGoalGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GameMode->CompleteStage();
		}
	}
}
