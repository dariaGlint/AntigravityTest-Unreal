#include "FormationFollowerComponent.h"
#include "FormationComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

UFormationFollowerComponent::UFormationFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	FormationLeader = nullptr;
	LeaderFormationComponent = nullptr;
	FormationIndex = 0;
	bFollowingEnabled = true;
	MovementThreshold = 100.0f;
	ArrivalThreshold = 50.0f;
	bUseAINavigation = true;
	bAllowTeleport = true;
	TeleportDistance = 3000.0f;
	CachedAIController = nullptr;
}

void UFormationFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache AI controller if owner is a pawn
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		CachedAIController = Cast<AAIController>(OwnerPawn->GetController());
	}
}

void UFormationFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFollowingEnabled && FormationLeader && LeaderFormationComponent)
	{
		UpdateFollowMovement();
	}
}

void UFormationFollowerComponent::SetFormationLeader(AActor* Leader)
{
	FormationLeader = Leader;

	if (Leader)
	{
		LeaderFormationComponent = Leader->FindComponentByClass<UFormationComponent>();

		if (LeaderFormationComponent)
		{
			// Register with the formation component
			LeaderFormationComponent->AddFollower(GetOwner());
		}
	}
	else
	{
		LeaderFormationComponent = nullptr;
	}
}

FVector UFormationFollowerComponent::GetTargetFormationPosition() const
{
	if (LeaderFormationComponent)
	{
		return LeaderFormationComponent->GetFormationPosition(FormationIndex);
	}

	return GetOwner()->GetActorLocation();
}

void UFormationFollowerComponent::SetFollowingEnabled(bool bEnabled)
{
	bFollowingEnabled = bEnabled;
}

void UFormationFollowerComponent::SnapToFormationPosition()
{
	FVector TargetPosition = GetTargetFormationPosition();
	GetOwner()->SetActorLocation(TargetPosition);
}

void UFormationFollowerComponent::UpdateFollowMovement()
{
	if (!GetOwner())
	{
		return;
	}

	FVector CurrentLocation = GetOwner()->GetActorLocation();
	FVector TargetPosition = GetTargetFormationPosition();
	float Distance = FVector::Dist(CurrentLocation, TargetPosition);

	// Check if we should teleport
	if (ShouldTeleportToFormation())
	{
		SnapToFormationPosition();
		return;
	}

	// Only move if beyond movement threshold
	if (Distance > MovementThreshold)
	{
		MoveToTargetPosition(TargetPosition);
	}
	else if (Distance <= ArrivalThreshold)
	{
		// Stop movement when close enough
		if (CachedAIController)
		{
			CachedAIController->StopMovement();
		}
	}
}

bool UFormationFollowerComponent::ShouldTeleportToFormation() const
{
	if (!bAllowTeleport || !GetOwner())
	{
		return false;
	}

	FVector CurrentLocation = GetOwner()->GetActorLocation();
	FVector TargetPosition = GetTargetFormationPosition();
	float Distance = FVector::Dist(CurrentLocation, TargetPosition);

	return Distance > TeleportDistance;
}

void UFormationFollowerComponent::MoveToTargetPosition(const FVector& TargetPosition)
{
	if (!GetOwner())
	{
		return;
	}

	if (bUseAINavigation && CachedAIController)
	{
		// Use AI navigation for pathfinding with obstacle avoidance
		const float AcceptanceRadius = ArrivalThreshold;
		CachedAIController->MoveToLocation(TargetPosition, AcceptanceRadius,
			true,  // bStopOnOverlap
			true,  // bUsePathfinding
			false, // bProjectDestinationToNavigation
			true,  // bCanStrafe
			nullptr, // FilterClass
			true   // bAllowPartialPath
		);
	}
	else
	{
		// Simple direct movement without pathfinding
		FVector CurrentLocation = GetOwner()->GetActorLocation();
		FVector Direction = (TargetPosition - CurrentLocation).GetSafeNormal();

		// This is a basic implementation - in practice, you'd want to use
		// character movement component or apply velocity
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			// Let the character's movement component handle the actual movement
			// by setting a target direction or velocity
		}
	}
}
