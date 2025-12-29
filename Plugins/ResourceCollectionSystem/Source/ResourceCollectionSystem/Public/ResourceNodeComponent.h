#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ResourceNodeComponent.generated.h"

class UResourceDropTable;
class UItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceNodeHarvested, AActor*, Harvester);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourceNodeDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourceNodeRespawned);

/**
 * Component that makes an actor harvestable as a resource node
 * Supports mining, gathering, woodcutting, etc.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RESOURCECOLLECTIONSYSTEM_API UResourceNodeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceNodeComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Drop table for this resource node
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node")
	TObjectPtr<UResourceDropTable> DropTable;

	// How many times this node can be harvested before depleting (0 = infinite)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node", meta = (ClampMin = "0"))
	int32 MaxHarvestCount = 1;

	// Time in seconds before the node respawns after depletion (0 = no respawn)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Respawn", meta = (ClampMin = "0.0"))
	float RespawnTime = 60.0f;

	// Random variance in respawn time (±seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Respawn", meta = (ClampMin = "0.0"))
	float RespawnTimeVariance = 10.0f;

	// Gameplay tags required on the harvester to collect from this node
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Requirements")
	FGameplayTagContainer RequiredHarvesterTags;

	// Visual feedback when harvested (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Feedback")
	TObjectPtr<UParticleSystem> HarvestParticleEffect;

	// Sound to play when harvested (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Feedback")
	TObjectPtr<USoundBase> HarvestSound;

	// Whether the node is currently available for harvesting
	UPROPERTY(BlueprintReadOnly, Category = "Resource Node")
	bool bIsAvailable = true;

	// Current harvest count
	UPROPERTY(BlueprintReadOnly, Category = "Resource Node")
	int32 CurrentHarvestCount = 0;

	/**
	 * Attempt to harvest this resource node
	 * @param Harvester - The actor attempting to harvest (typically a player character)
	 * @param SkillModifier - Multiplier for drop quantities based on harvester's skill level
	 * @param OutDrops - Array of items and quantities that were dropped
	 * @return True if harvest was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Resource Node")
	bool TryHarvest(AActor* Harvester, float SkillModifier, TArray<TPair<UItemDefinition*, int32>>& OutDrops);

	/**
	 * Force respawn the node immediately
	 */
	UFUNCTION(BlueprintCallable, Category = "Resource Node")
	void ForceRespawn();

	/**
	 * Check if an actor meets the requirements to harvest this node
	 */
	UFUNCTION(BlueprintPure, Category = "Resource Node")
	bool CanHarvest(AActor* Harvester) const;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Resource Node|Events")
	FOnResourceNodeHarvested OnHarvested;

	UPROPERTY(BlueprintAssignable, Category = "Resource Node|Events")
	FOnResourceNodeDepleted OnDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Resource Node|Events")
	FOnResourceNodeRespawned OnRespawned;

protected:
	/**
	 * Called when the node is depleted
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Resource Node")
	void OnNodeDepleted();

	/**
	 * Called when the node respawns
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Resource Node")
	void OnNodeRespawned();

	/**
	 * Start the respawn timer
	 */
	void StartRespawnTimer();

	/**
	 * Handle respawn timer completion
	 */
	void HandleRespawn();

private:
	FTimerHandle RespawnTimerHandle;
};
