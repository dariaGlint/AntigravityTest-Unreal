#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FormationTypes.h"
#include "FormationLibrary.generated.h"

/**
 * Blueprint function library for formation utilities
 */
UCLASS()
class FORMATIONSYSTEM_API UFormationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Create a default formation configuration for a given type
	UFUNCTION(BlueprintPure, Category = "Formation")
	static FFormationConfig MakeFormationConfig(EFormationType FormationType, float Spacing = 200.0f);

	// Create a custom formation configuration with specific slots
	UFUNCTION(BlueprintPure, Category = "Formation")
	static FFormationConfig MakeCustomFormationConfig(const TArray<FFormationSlot>& Slots, float MaxFollowDistance = 5000.0f);

	// Calculate the ideal formation position for a specific member index
	UFUNCTION(BlueprintPure, Category = "Formation")
	static FVector CalculateFormationSlotPosition(const FVector& LeaderPosition, const FRotator& LeaderRotation, const FFormationSlot& Slot);

	// Get recommended formation type based on combat state
	UFUNCTION(BlueprintPure, Category = "Formation")
	static EFormationType GetRecommendedFormationType(bool bInCombat, int32 PartySize);

	// Check if a formation position is valid (not blocked by obstacles)
	UFUNCTION(BlueprintCallable, Category = "Formation", meta = (WorldContext = "WorldContextObject"))
	static bool IsFormationPositionValid(UObject* WorldContextObject, const FVector& Position, float TestRadius = 50.0f);

	// Smoothly transition between two formation configurations
	UFUNCTION(BlueprintPure, Category = "Formation")
	static FFormationConfig LerpFormationConfig(const FFormationConfig& A, const FFormationConfig& B, float Alpha);
};
