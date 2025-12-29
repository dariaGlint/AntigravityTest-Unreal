#include "ResourceHarvesterInterface.h"

// Default implementations
FGameplayTagContainer IResourceHarvesterInterface::GetHarvesterTags_Implementation() const
{
	return FGameplayTagContainer();
}

float IResourceHarvesterInterface::GetSkillModifier_Implementation(const FGameplayTag& ResourceTag) const
{
	return 1.0f;
}

void IResourceHarvesterInterface::OnResourceHarvested_Implementation(UResourceNodeComponent* NodeComponent)
{
	// Default: do nothing
}
