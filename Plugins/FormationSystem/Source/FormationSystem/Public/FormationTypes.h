#pragma once

#include "CoreMinimal.h"
#include "FormationTypes.generated.h"

/**
 * Formation types available for party arrangement
 */
UENUM(BlueprintType)
enum class EFormationType : uint8
{
	// Line formation - members follow in a line behind the leader
	Line UMETA(DisplayName = "Line"),

	// Column formation - members arrange in a column
	Column UMETA(DisplayName = "Column"),

	// Wedge formation - V-shaped formation with leader at the front
	Wedge UMETA(DisplayName = "Wedge"),

	// Circle formation - members surround the leader
	Circle UMETA(DisplayName = "Circle"),

	// Combat spread formation - members spread out for combat
	CombatSpread UMETA(DisplayName = "Combat Spread"),

	// Custom formation - user-defined positions
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Formation slot data - defines position for a single party member
 */
USTRUCT(BlueprintType)
struct FORMATIONSYSTEM_API FFormationSlot
{
	GENERATED_BODY()

	// Relative position offset from the leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	FVector RelativePosition;

	// Priority of this slot (lower = higher priority)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	int32 Priority;

	// Rotation offset for this slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	FRotator RelativeRotation;

	FFormationSlot()
		: RelativePosition(FVector::ZeroVector)
		, Priority(0)
		, RelativeRotation(FRotator::ZeroRotator)
	{
	}

	FFormationSlot(const FVector& InPosition, int32 InPriority, const FRotator& InRotation = FRotator::ZeroRotator)
		: RelativePosition(InPosition)
		, Priority(InPriority)
		, RelativeRotation(InRotation)
	{
	}
};

/**
 * Formation configuration data
 */
USTRUCT(BlueprintType)
struct FORMATIONSYSTEM_API FFormationConfig
{
	GENERATED_BODY()

	// Type of formation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	EFormationType FormationType;

	// Formation slots defining member positions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	TArray<FFormationSlot> Slots;

	// Spacing between formation members
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	float MemberSpacing;

	// Whether to enable obstacle avoidance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	bool bEnableObstacleAvoidance;

	// Maximum distance from leader before forcing teleport
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	float MaxFollowDistance;

	FFormationConfig()
		: FormationType(EFormationType::Line)
		, MemberSpacing(200.0f)
		, bEnableObstacleAvoidance(true)
		, MaxFollowDistance(5000.0f)
	{
	}
};
