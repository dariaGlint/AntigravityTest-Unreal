#include "ResourceCollectionLibrary.h"
#include "ResourceNodeComponent.h"
#include "HarvestingSkillComponent.h"
#include "ResourceHarvesterInterface.h"

// Forward declare to avoid hard dependency
class UInventoryComponent;
class UItemDefinition;

bool UResourceCollectionLibrary::HarvestResourceNode(
	AActor* Harvester,
	UResourceNodeComponent* ResourceNode,
	UInventoryComponent* Inventory,
	const FGameplayTag& SkillTag)
{
	if (!Harvester || !ResourceNode)
	{
		return false;
	}

	// Get skill modifier
	float SkillModifier = GetHarvesterSkillModifier(Harvester, SkillTag);

	// Attempt harvest
	TArray<TPair<UItemDefinition*, int32>> Drops;
	bool bSuccess = ResourceNode->TryHarvest(Harvester, SkillModifier, Drops);

	if (!bSuccess)
	{
		return false;
	}

	// Add items to inventory if provided
	if (Inventory && Drops.Num() > 0)
	{
		AddDropsToInventory(Inventory, Drops);
	}

	// Grant experience if harvester has skill component
	if (UHarvestingSkillComponent* SkillComp = Harvester->FindComponentByClass<UHarvestingSkillComponent>())
	{
		if (SkillTag.IsValid())
		{
			SkillComp->AddSkillExperience(SkillTag, SkillComp->BaseExperiencePerHarvest);
		}
	}

	// Notify harvester interface if implemented
	if (Harvester->GetClass()->ImplementsInterface(UResourceHarvesterInterface::StaticClass()))
	{
		IResourceHarvesterInterface::Execute_OnResourceHarvested(Harvester, ResourceNode);
	}

	return true;
}

float UResourceCollectionLibrary::GetHarvesterSkillModifier(AActor* Harvester, const FGameplayTag& SkillTag)
{
	if (!Harvester || !SkillTag.IsValid())
	{
		return 1.0f;
	}

	// Check for skill component first
	if (UHarvestingSkillComponent* SkillComp = Harvester->FindComponentByClass<UHarvestingSkillComponent>())
	{
		return SkillComp->GetSkillModifier(SkillTag);
	}

	// Check for harvester interface
	if (Harvester->GetClass()->ImplementsInterface(UResourceHarvesterInterface::StaticClass()))
	{
		return IResourceHarvesterInterface::Execute_GetSkillModifier(Harvester, SkillTag);
	}

	return 1.0f;
}

void UResourceCollectionLibrary::AddDropsToInventory(UInventoryComponent* Inventory, const TArray<TPair<UItemDefinition*, int32>>& Drops)
{
	if (!Inventory)
	{
		return;
	}

	// Use reflection to call AddItem if it exists
	// This avoids hard dependency on the main game module
	UFunction* AddItemFunc = Inventory->GetClass()->FindFunctionByName(TEXT("AddItem"));
	if (AddItemFunc)
	{
		for (const TPair<UItemDefinition*, int32>& Drop : Drops)
		{
			if (Drop.Key)
			{
				struct FAddItemParams
				{
					UItemDefinition* ItemDef;
					int32 Quantity;
				};

				FAddItemParams Params;
				Params.ItemDef = Drop.Key;
				Params.Quantity = Drop.Value;

				Inventory->ProcessEvent(AddItemFunc, &Params);
			}
		}
	}
}
