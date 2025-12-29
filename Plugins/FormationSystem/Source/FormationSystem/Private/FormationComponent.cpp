#include "FormationComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bFormationEnabled = true;
	UpdateInterval = 0.1f; // Update 10 times per second
	TimeSinceLastUpdate = 0.0f;

	// Set default configuration
	CurrentConfig.FormationType = EFormationType::Line;
	CurrentConfig.MemberSpacing = 200.0f;
	CurrentConfig.bEnableObstacleAvoidance = true;
	CurrentConfig.MaxFollowDistance = 5000.0f;
}

void UFormationComponent::BeginPlay()
{
	Super::BeginPlay();
	GenerateFormationSlots();
}

void UFormationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bFormationEnabled || Followers.Num() == 0)
	{
		return;
	}

	TimeSinceLastUpdate += DeltaTime;
	if (TimeSinceLastUpdate >= UpdateInterval)
	{
		UpdateFormationPositions();
		TimeSinceLastUpdate = 0.0f;
	}
}

void UFormationComponent::AddFollower(AActor* Follower)
{
	if (Follower && !Followers.Contains(Follower))
	{
		Followers.Add(Follower);

		// Regenerate slots if we have more followers than slots
		if (Followers.Num() > CurrentConfig.Slots.Num())
		{
			GenerateFormationSlots();
		}
	}
}

void UFormationComponent::RemoveFollower(AActor* Follower)
{
	if (Follower)
	{
		Followers.Remove(Follower);
	}
}

void UFormationComponent::ClearFollowers()
{
	Followers.Empty();
}

void UFormationComponent::SetFormationType(EFormationType NewFormationType)
{
	if (CurrentConfig.FormationType != NewFormationType)
	{
		CurrentConfig.FormationType = NewFormationType;
		GenerateFormationSlots();
	}
}

void UFormationComponent::SetFormationConfig(const FFormationConfig& NewConfig)
{
	CurrentConfig = NewConfig;
	GenerateFormationSlots();
}

void UFormationComponent::SetFormationEnabled(bool bEnabled)
{
	bFormationEnabled = bEnabled;
}

FVector UFormationComponent::GetFormationPosition(int32 FollowerIndex) const
{
	if (!CurrentConfig.Slots.IsValidIndex(FollowerIndex))
	{
		return GetOwner()->GetActorLocation();
	}

	return CalculateSlotWorldPosition(CurrentConfig.Slots[FollowerIndex]);
}

void UFormationComponent::GenerateFormationSlots()
{
	CurrentConfig.Slots.Empty();

	const float Spacing = CurrentConfig.MemberSpacing;
	const int32 MaxSlots = 16; // Generate slots for up to 16 followers

	switch (CurrentConfig.FormationType)
	{
	case EFormationType::Line:
		// Line formation - followers behind leader in a single line
		for (int32 i = 0; i < MaxSlots; ++i)
		{
			FVector Position = FVector(-Spacing * (i + 1), 0.0f, 0.0f);
			CurrentConfig.Slots.Add(FFormationSlot(Position, i));
		}
		break;

	case EFormationType::Column:
		// Column formation - two columns behind leader
		for (int32 i = 0; i < MaxSlots; ++i)
		{
			int32 Row = i / 2;
			float YOffset = (i % 2 == 0) ? -Spacing * 0.5f : Spacing * 0.5f;
			FVector Position = FVector(-Spacing * (Row + 1), YOffset, 0.0f);
			CurrentConfig.Slots.Add(FFormationSlot(Position, i));
		}
		break;

	case EFormationType::Wedge:
		// Wedge formation - V-shape with leader at point
		for (int32 i = 0; i < MaxSlots; ++i)
		{
			int32 Row = i / 2;
			float Side = (i % 2 == 0) ? -1.0f : 1.0f;
			FVector Position = FVector(-Spacing * (Row + 1), Side * Spacing * (Row + 1), 0.0f);
			CurrentConfig.Slots.Add(FFormationSlot(Position, i));
		}
		break;

	case EFormationType::Circle:
		// Circle formation - members arranged in a circle around leader
		for (int32 i = 0; i < MaxSlots; ++i)
		{
			float Angle = (360.0f / FMath::Max(MaxSlots, 1)) * i;
			float RadAngle = FMath::DegreesToRadians(Angle);
			FVector Position = FVector(
				FMath::Cos(RadAngle) * Spacing,
				FMath::Sin(RadAngle) * Spacing,
				0.0f
			);
			FRotator Rotation = FRotator(0.0f, Angle, 0.0f);
			CurrentConfig.Slots.Add(FFormationSlot(Position, i, Rotation));
		}
		break;

	case EFormationType::CombatSpread:
		// Combat spread - wider spacing for combat
		{
			const float CombatSpacing = Spacing * 1.5f;
			int32 SlotsPerRow = 4;
			for (int32 i = 0; i < MaxSlots; ++i)
			{
				int32 Row = i / SlotsPerRow;
				int32 Col = i % SlotsPerRow;
				float XOffset = -CombatSpacing * (Row + 1);
				float YOffset = CombatSpacing * (Col - SlotsPerRow / 2.0f + 0.5f);
				FVector Position = FVector(XOffset, YOffset, 0.0f);
				CurrentConfig.Slots.Add(FFormationSlot(Position, i));
			}
		}
		break;

	case EFormationType::Custom:
		// Custom formation - keep existing slots or create default
		if (CurrentConfig.Slots.Num() == 0)
		{
			// Default to line if no custom slots defined
			for (int32 i = 0; i < MaxSlots; ++i)
			{
				FVector Position = FVector(-Spacing * (i + 1), 0.0f, 0.0f);
				CurrentConfig.Slots.Add(FFormationSlot(Position, i));
			}
		}
		break;
	}
}

void UFormationComponent::UpdateFormationPositions()
{
	AActor* Leader = GetOwner();
	if (!Leader)
	{
		return;
	}

	// This method calculates target positions but doesn't move followers
	// The actual movement should be handled by AI controllers or follower components
	// This allows for flexibility in how followers reach their positions
}

FVector UFormationComponent::CalculateSlotWorldPosition(const FFormationSlot& Slot) const
{
	AActor* Leader = GetOwner();
	if (!Leader)
	{
		return FVector::ZeroVector;
	}

	// Get leader's transform
	FVector LeaderLocation = Leader->GetActorLocation();
	FRotator LeaderRotation = Leader->GetActorRotation();

	// Calculate world position by rotating the relative position by leader's rotation
	FVector WorldOffset = LeaderRotation.RotateVector(Slot.RelativePosition);
	FVector WorldPosition = LeaderLocation + WorldOffset;

	return WorldPosition;
}
