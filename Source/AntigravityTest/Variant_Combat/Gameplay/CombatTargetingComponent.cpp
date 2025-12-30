// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatTargetingComponent.h"
#include "Variant_Combat/AI/CombatEnemy.h"
#include "Variant_Combat/CombatCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UCombatTargetingComponent::UCombatTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Update 10 times per second
}

void UCombatTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Scan for enemies periodically
	ScanForEnemies();

	// Validate current target if locked
	if (bIsTargetLocked && !IsCurrentTargetValid())
	{
		ClearTargetLock();
	}
}

void UCombatTargetingComponent::ToggleTargetLock()
{
	if (bIsTargetLocked)
	{
		ClearTargetLock();
	}
	else
	{
		// Find and lock onto best target
		AActor* BestTarget = FindBestTarget();
		if (BestTarget)
		{
			SetTargetLock(BestTarget);
		}
	}
}

void UCombatTargetingComponent::SetTargetLock(AActor* NewTarget)
{
	if (!NewTarget)
	{
		ClearTargetLock();
		return;
	}

	CurrentTarget = NewTarget;
	bIsTargetLocked = true;

	// Notify listeners
	OnTargetLockChanged.Broadcast(CurrentTarget);
	BP_OnTargetLockChanged(CurrentTarget);
}

void UCombatTargetingComponent::ClearTargetLock()
{
	CurrentTarget = nullptr;
	bIsTargetLocked = false;

	// Notify listeners
	OnTargetLockChanged.Broadcast(nullptr);
	BP_OnTargetLockChanged(nullptr);
}

void UCombatTargetingComponent::SwitchToNextTarget()
{
	if (DetectedEnemies.Num() == 0)
	{
		ClearTargetLock();
		return;
	}

	// Find current target index
	int32 CurrentIndex = DetectedEnemies.IndexOfByKey(CurrentTarget);

	// Move to next target (wrap around)
	int32 NextIndex = (CurrentIndex + 1) % DetectedEnemies.Num();
	SetTargetLock(DetectedEnemies[NextIndex]);
}

void UCombatTargetingComponent::SwitchToPreviousTarget()
{
	if (DetectedEnemies.Num() == 0)
	{
		ClearTargetLock();
		return;
	}

	// Find current target index
	int32 CurrentIndex = DetectedEnemies.IndexOfByKey(CurrentTarget);

	// Move to previous target (wrap around)
	int32 PrevIndex = (CurrentIndex - 1 + DetectedEnemies.Num()) % DetectedEnemies.Num();
	SetTargetLock(DetectedEnemies[PrevIndex]);
}

void UCombatTargetingComponent::ScanForEnemies()
{
	DetectedEnemies.Empty();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Get camera info for direction-based filtering
	FVector CameraLocation;
	FVector CameraForward;
	if (!GetCameraInfo(CameraLocation, CameraForward))
	{
		return;
	}

	// Sphere overlap to find nearby enemies
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(DetectionRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	World->OverlapMultiByChannel(
		OverlapResults,
		Owner->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);

	// Filter results by angle and type
	const float MaxAngleCos = FMath::Cos(FMath::DegreesToRadians(DetectionAngle));

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* HitActor = Result.GetActor();
		if (!HitActor)
		{
			continue;
		}

		// Check if it's a combat enemy
		ACombatEnemy* Enemy = Cast<ACombatEnemy>(HitActor);
		if (!Enemy)
		{
			continue;
		}

		// Skip dead enemies (CurrentHP <= 0)
		if (Enemy->CurrentHP <= 0.0f)
		{
			continue;
		}

		// Check if enemy is within angle cone
		FVector ToEnemy = (HitActor->GetActorLocation() - CameraLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(CameraForward, ToEnemy);

		if (DotProduct >= MaxAngleCos)
		{
			DetectedEnemies.Add(HitActor);
		}
	}
}

AActor* UCombatTargetingComponent::FindBestTarget() const
{
	if (DetectedEnemies.Num() == 0)
	{
		return nullptr;
	}

	AActor* BestTarget = nullptr;
	float BestScore = -1.0f;

	for (AActor* PotentialTarget : DetectedEnemies)
	{
		if (!PotentialTarget)
		{
			continue;
		}

		float Score = CalculateTargetPriority(PotentialTarget);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = PotentialTarget;
		}
	}

	return BestTarget;
}

float UCombatTargetingComponent::CalculateTargetPriority(AActor* PotentialTarget) const
{
	if (!PotentialTarget)
	{
		return 0.0f;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return 0.0f;
	}

	FVector CameraLocation;
	FVector CameraForward;
	if (!GetCameraInfo(CameraLocation, CameraForward))
	{
		return 0.0f;
	}

	FVector TargetLocation = PotentialTarget->GetActorLocation();
	FVector ToTarget = TargetLocation - CameraLocation;
	float Distance = ToTarget.Size();

	// Normalize distance (closer = higher score)
	float DistanceScore = 1.0f - FMath::Clamp(Distance / DetectionRadius, 0.0f, 1.0f);

	// Calculate angle score (more centered = higher score)
	FVector ToTargetNormalized = ToTarget.GetSafeNormal();
	float DotProduct = FVector::DotProduct(CameraForward, ToTargetNormalized);
	float AngleScore = (DotProduct + 1.0f) / 2.0f; // Normalize to 0-1

	// Combine scores with weights
	float TotalScore = (DistanceScore * DistancePriorityWeight) + (AngleScore * AnglePriorityWeight);

	return TotalScore;
}

bool UCombatTargetingComponent::IsCurrentTargetValid() const
{
	if (!CurrentTarget)
	{
		return false;
	}

	// Check if target is still alive
	ACombatEnemy* Enemy = Cast<ACombatEnemy>(CurrentTarget);
	if (Enemy && Enemy->CurrentHP <= 0.0f)
	{
		return false;
	}

	// Check if target is within max lock distance
	AActor* Owner = GetOwner();
	if (Owner)
	{
		float Distance = FVector::Distance(Owner->GetActorLocation(), CurrentTarget->GetActorLocation());
		if (Distance > MaxLockDistance)
		{
			return false;
		}
	}

	return true;
}

bool UCombatTargetingComponent::GetCameraInfo(FVector& OutLocation, FVector& OutForward) const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	// Try to get camera component from owner
	ACombatCharacter* CombatChar = Cast<ACombatCharacter>(Owner);
	if (CombatChar)
	{
		UCameraComponent* Camera = CombatChar->GetFollowCamera();
		if (Camera)
		{
			OutLocation = Camera->GetComponentLocation();
			OutForward = Camera->GetForwardVector();
			return true;
		}
	}

	// Fallback to actor location and rotation
	OutLocation = Owner->GetActorLocation();
	OutForward = Owner->GetActorForwardVector();
	return true;
}
