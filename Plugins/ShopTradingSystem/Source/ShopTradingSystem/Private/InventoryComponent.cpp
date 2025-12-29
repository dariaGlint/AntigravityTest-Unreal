#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(UProductItem* Product, int32 Quantity)
{
	if (!Product || Quantity <= 0)
	{
		return false;
	}

	if (!CanAddItem(Product, Quantity))
	{
		return false;
	}

	int32* ExistingQuantity = Items.Find(Product);
	if (ExistingQuantity)
	{
		*ExistingQuantity += Quantity;
		OnInventoryChanged.Broadcast(Product, *ExistingQuantity);
	}
	else
	{
		Items.Add(Product, Quantity);
		OnInventoryChanged.Broadcast(Product, Quantity);
	}

	return true;
}

bool UInventoryComponent::RemoveItem(UProductItem* Product, int32 Quantity)
{
	if (!Product || Quantity <= 0)
	{
		return false;
	}

	int32* ExistingQuantity = Items.Find(Product);
	if (!ExistingQuantity || *ExistingQuantity < Quantity)
	{
		return false;
	}

	*ExistingQuantity -= Quantity;

	if (*ExistingQuantity <= 0)
	{
		Items.Remove(Product);
		OnInventoryChanged.Broadcast(Product, 0);
	}
	else
	{
		OnInventoryChanged.Broadcast(Product, *ExistingQuantity);
	}

	return true;
}

bool UInventoryComponent::HasItem(UProductItem* Product, int32 Quantity) const
{
	if (!Product || Quantity <= 0)
	{
		return false;
	}

	const int32* ExistingQuantity = Items.Find(Product);
	return ExistingQuantity && *ExistingQuantity >= Quantity;
}

int32 UInventoryComponent::GetItemQuantity(UProductItem* Product) const
{
	if (!Product)
	{
		return 0;
	}

	const int32* ExistingQuantity = Items.Find(Product);
	return ExistingQuantity ? *ExistingQuantity : 0;
}

TArray<FProductInstance> UInventoryComponent::GetAllItems() const
{
	TArray<FProductInstance> Result;

	for (const auto& ItemPair : Items)
	{
		if (ItemPair.Key && ItemPair.Value > 0)
		{
			FProductInstance Instance(ItemPair.Key, ItemPair.Value, ItemPair.Key->BasePrice);
			Result.Add(Instance);
		}
	}

	return Result;
}

void UInventoryComponent::ClearInventory()
{
	Items.Empty();
	OnInventoryChanged.Broadcast(nullptr, 0);
}

bool UInventoryComponent::AddCurrency(float Amount)
{
	if (Amount <= 0.0f)
	{
		return false;
	}

	Currency += Amount;
	OnCurrencyChanged.Broadcast(Currency);
	return true;
}

bool UInventoryComponent::RemoveCurrency(float Amount)
{
	if (Amount <= 0.0f || Currency < Amount)
	{
		return false;
	}

	Currency -= Amount;
	OnCurrencyChanged.Broadcast(Currency);
	return true;
}

void UInventoryComponent::SetCurrency(float NewAmount)
{
	if (NewAmount < 0.0f)
	{
		NewAmount = 0.0f;
	}

	Currency = NewAmount;
	OnCurrencyChanged.Broadcast(Currency);
}

bool UInventoryComponent::HasEnoughCurrency(float Amount) const
{
	return Currency >= Amount;
}

bool UInventoryComponent::CanAddItem(UProductItem* Product, int32 Quantity) const
{
	if (!Product || Quantity <= 0)
	{
		return false;
	}

	int32 CurrentSlots = GetUsedSlots();
	const int32* ExistingQuantity = Items.Find(Product);

	if (ExistingQuantity)
	{
		int32 NewTotal = *ExistingQuantity + Quantity;
		return NewTotal <= Product->MaxStackSize;
	}
	else
	{
		return CurrentSlots < MaxSlots;
	}
}

int32 UInventoryComponent::GetUsedSlots() const
{
	return Items.Num();
}
