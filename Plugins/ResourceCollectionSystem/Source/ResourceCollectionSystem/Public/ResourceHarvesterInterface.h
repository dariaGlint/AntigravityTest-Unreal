#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "ResourceHarvesterInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UResourceHarvesterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can harvest resources
 * Implement this on your player character or AI to enable resource harvesting
 */
class RESOURCECOLLECTIONSYSTEM_API IResourceHarvesterInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get the gameplay tags for this harvester (e.g., "Skill.Mining", "Tool.Pickaxe")
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource Harvester")
	FGameplayTagContainer GetHarvesterTags() const;

	/**
	 * Get the skill modifier for a specific resource type
	 * @param ResourceTag - Tag identifying the resource type (e.g., "Resource.Ore", "Resource.Wood")
	 * @return Multiplier for resource quantities (1.0 = normal, 1.5 = 50% bonus, etc.)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource Harvester")
	float GetSkillModifier(const FGameplayTag& ResourceTag) const;

	/**
	 * Called when the harvester successfully harvests a resource
	 * Use this to grant experience, update stats, etc.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource Harvester")
	void OnResourceHarvested(class UResourceNodeComponent* NodeComponent);
};
