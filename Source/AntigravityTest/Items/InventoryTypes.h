#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "InventoryTypes.generated.h"

/**
 * Represents a specific instance of an item in an inventory (Item Definition + Quantity)
 */
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemDefinition> ItemDef = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;

	bool IsValid() const
	{
		return ItemDef != nullptr && Quantity > 0;
	}

	bool operator==(const FInventoryItem& Other) const
	{
		return ItemDef == Other.ItemDef;
	}
};
