#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationTypes.h"
#include "FormationComponent.generated.h"

class ACharacter;

/**
 * Component for managing party formation
 * Attach to the party leader to control formation of followers
 */
UCLASS(ClassGroup=(Formation), meta=(BlueprintSpawnableComponent))
class FORMATIONSYSTEM_API UFormationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFormationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Add a follower to the formation
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void AddFollower(AActor* Follower);

	// Remove a follower from the formation
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void RemoveFollower(AActor* Follower);

	// Clear all followers
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void ClearFollowers();

	// Set formation type
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SetFormationType(EFormationType NewFormationType);

	// Get current formation type
	UFUNCTION(BlueprintPure, Category = "Formation")
	EFormationType GetFormationType() const { return CurrentConfig.FormationType; }

	// Set formation configuration
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SetFormationConfig(const FFormationConfig& NewConfig);

	// Get formation configuration
	UFUNCTION(BlueprintPure, Category = "Formation")
	const FFormationConfig& GetFormationConfig() const { return CurrentConfig; }

	// Get formation position for a specific follower index
	UFUNCTION(BlueprintPure, Category = "Formation")
	FVector GetFormationPosition(int32 FollowerIndex) const;

	// Enable or disable formation updates
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SetFormationEnabled(bool bEnabled);

	// Check if formation is enabled
	UFUNCTION(BlueprintPure, Category = "Formation")
	bool IsFormationEnabled() const { return bFormationEnabled; }

	// Get all followers
	UFUNCTION(BlueprintPure, Category = "Formation")
	const TArray<AActor*>& GetFollowers() const { return Followers; }

protected:
	virtual void BeginPlay() override;

	// Generate formation slots based on formation type
	void GenerateFormationSlots();

	// Update formation positions for all followers
	void UpdateFormationPositions();

	// Calculate world position for a formation slot
	FVector CalculateSlotWorldPosition(const FFormationSlot& Slot) const;

private:
	// Current formation configuration
	UPROPERTY(EditAnywhere, Category = "Formation")
	FFormationConfig CurrentConfig;

	// List of followers in the formation
	UPROPERTY()
	TArray<AActor*> Followers;

	// Whether formation updates are enabled
	UPROPERTY(EditAnywhere, Category = "Formation")
	bool bFormationEnabled;

	// Update interval for formation calculations
	UPROPERTY(EditAnywhere, Category = "Formation")
	float UpdateInterval;

	// Time since last update
	float TimeSinceLastUpdate;
};
