#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(UItemDefinition* ItemDef, int32 Quantity)
{
	if (!ItemDef || Quantity <= 0)
	{
		return;
	}

	// Calculate check: Do we already have this item?
	FInventoryItem* ExistingItem = Items.FindByPredicate([ItemDef](const FInventoryItem& Item)
	{
		return Item.ItemDef == ItemDef;
	});

	if (ExistingItem)
	{
		ExistingItem->Quantity += Quantity;
	}
	else
	{
		FInventoryItem NewItem;
		NewItem.ItemDef = ItemDef;
		NewItem.Quantity = Quantity;
		Items.Add(NewItem);
	}

	OnInventoryUpdated.Broadcast();
}

bool UInventoryComponent::RemoveItem(UItemDefinition* ItemDef, int32 Quantity)
{
	if (!ItemDef || Quantity <= 0)
	{
		return false;
	}

	FInventoryItem* ExistingItem = Items.FindByPredicate([ItemDef](const FInventoryItem& Item)
	{
		return Item.ItemDef == ItemDef;
	});

	if (ExistingItem)
	{
		if (ExistingItem->Quantity >= Quantity)
		{
			ExistingItem->Quantity -= Quantity;
			
			if (ExistingItem->Quantity == 0)
			{
				Items.RemoveAll([ItemDef](const FInventoryItem& Item)
				{
					return Item.ItemDef == ItemDef;
				});
			}

			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	return false;
}
