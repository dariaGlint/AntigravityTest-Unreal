#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "ResourceCollectionLibrary.generated.h"

class UResourceNodeComponent;
class UHarvestingSkillComponent;
class UItemDefinition;
class UInventoryComponent;

/**
 * Blueprint function library for resource collection system
 * Provides convenient helper functions for common operations
 */
UCLASS()
class RESOURCECOLLECTIONSYSTEM_API UResourceCollectionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Attempt to harvest a resource node and add items to an inventory
	 * @param Harvester - The actor attempting to harvest
	 * @param ResourceNode - The resource node component to harvest
	 * @param Inventory - The inventory to add items to (optional)
	 * @param SkillTag - The skill tag to use for modifiers (e.g., "Skill.Mining")
	 * @return True if harvest was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Resource Collection", meta = (AutoCreateRefTerm = "SkillTag"))
	static bool HarvestResourceNode(
		AActor* Harvester,
		UResourceNodeComponent* ResourceNode,
		UInventoryComponent* Inventory,
		const FGameplayTag& SkillTag
	);

	/**
	 * Get the skill modifier for a harvester
	 * Checks for HarvestingSkillComponent or ResourceHarvesterInterface
	 * @param Harvester - The harvesting actor
	 * @param SkillTag - The skill tag to check
	 * @return Skill modifier (1.0 if no skill found)
	 */
	UFUNCTION(BlueprintPure, Category = "Resource Collection", meta = (AutoCreateRefTerm = "SkillTag"))
	static float GetHarvesterSkillModifier(AActor* Harvester, const FGameplayTag& SkillTag);

	/**
	 * Add harvested items to an inventory
	 * @param Inventory - The inventory component
	 * @param Drops - Array of items and quantities to add
	 */
	UFUNCTION(BlueprintCallable, Category = "Resource Collection")
	static void AddDropsToInventory(UInventoryComponent* Inventory, const TArray<TPair<UItemDefinition*, int32>>& Drops);
};
