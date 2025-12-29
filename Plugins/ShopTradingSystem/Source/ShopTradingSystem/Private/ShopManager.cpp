#include "ShopManager.h"

void UShopManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PriceFluctuationSystem = NewObject<UPriceFluctuationSystem>(this);

	FluctuationConfig.FluctuationMode = EPriceFluctuationMode::Random;
	FluctuationConfig.MinPriceMultiplier = 0.8f;
	FluctuationConfig.MaxPriceMultiplier = 1.5f;
	FluctuationConfig.FluctuationInterval = 300.0f;
	FluctuationConfig.bAffectsByDemand = true;
	FluctuationConfig.DemandInfluence = 0.3f;
}

void UShopManager::Deinitialize()
{
	ShopInventory.Empty();
	PriceFluctuationSystem = nullptr;

	Super::Deinitialize();
}

void UShopManager::AddProductToShop(UProductItem* Product, int32 InitialStock, bool bIsUnlimited)
{
	if (!Product)
	{
		return;
	}

	FShopProductEntry Entry(Product, InitialStock, bIsUnlimited);
	Entry.CurrentPrice = Product->BasePrice;

	ShopInventory.Add(Product, Entry);
	OnShopInventoryChanged.Broadcast(Product, InitialStock);
}

void UShopManager::RemoveProductFromShop(UProductItem* Product)
{
	if (!Product)
	{
		return;
	}

	ShopInventory.Remove(Product);
	OnShopInventoryChanged.Broadcast(Product, 0);
}

void UShopManager::UpdateProductStock(UProductItem* Product, int32 NewStock)
{
	if (!Product)
	{
		return;
	}

	FShopProductEntry* Entry = ShopInventory.Find(Product);
	if (Entry)
	{
		Entry->Stock = NewStock;
		OnShopInventoryChanged.Broadcast(Product, NewStock);
	}
}

TArray<FShopProductEntry> UShopManager::GetAvailableProducts() const
{
	TArray<FShopProductEntry> Result;

	for (const auto& Pair : ShopInventory)
	{
		if (Pair.Value.bIsUnlimited || Pair.Value.Stock > 0)
		{
			Result.Add(Pair.Value);
		}
	}

	return Result;
}

bool UShopManager::IsProductAvailable(UProductItem* Product) const
{
	if (!Product)
	{
		return false;
	}

	const FShopProductEntry* Entry = ShopInventory.Find(Product);
	return Entry && (Entry->bIsUnlimited || Entry->Stock > 0);
}

int32 UShopManager::GetProductStock(UProductItem* Product) const
{
	if (!Product)
	{
		return 0;
	}

	const FShopProductEntry* Entry = ShopInventory.Find(Product);
	return Entry ? Entry->Stock : 0;
}

bool UShopManager::BuyProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity)
{
	if (!CanBuyProduct(PlayerInventory, Product, Quantity))
	{
		OnTransactionComplete.Broadcast(false, Product, Quantity);
		return false;
	}

	float TotalPrice = GetBuyPrice(Product, Quantity);

	if (ProcessBuyTransaction(PlayerInventory, Product, Quantity, TotalPrice))
	{
		if (PriceFluctuationSystem)
		{
			PriceFluctuationSystem->RegisterProductPurchase(Product, Quantity);
		}

		OnTransactionComplete.Broadcast(true, Product, Quantity);
		return true;
	}

	OnTransactionComplete.Broadcast(false, Product, Quantity);
	return false;
}

bool UShopManager::SellProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity)
{
	if (!CanSellProduct(PlayerInventory, Product, Quantity))
	{
		OnTransactionComplete.Broadcast(false, Product, Quantity);
		return false;
	}

	float TotalPrice = GetSellPrice(Product, Quantity);

	if (ProcessSellTransaction(PlayerInventory, Product, Quantity, TotalPrice))
	{
		if (PriceFluctuationSystem)
		{
			PriceFluctuationSystem->RegisterProductSale(Product, Quantity);
		}

		OnTransactionComplete.Broadcast(true, Product, Quantity);
		return true;
	}

	OnTransactionComplete.Broadcast(false, Product, Quantity);
	return false;
}

bool UShopManager::CanBuyProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity) const
{
	if (!PlayerInventory || !Product || Quantity <= 0)
	{
		return false;
	}

	if (!Product->bCanBuy)
	{
		return false;
	}

	const FShopProductEntry* Entry = ShopInventory.Find(Product);
	if (!Entry)
	{
		return false;
	}

	if (!Entry->bIsUnlimited && Entry->Stock < Quantity)
	{
		return false;
	}

	float TotalPrice = GetBuyPrice(Product, Quantity);
	return PlayerInventory->HasEnoughCurrency(TotalPrice);
}

bool UShopManager::CanSellProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity) const
{
	if (!PlayerInventory || !Product || Quantity <= 0)
	{
		return false;
	}

	if (!Product->bCanSell)
	{
		return false;
	}

	return PlayerInventory->HasItem(Product, Quantity);
}

float UShopManager::GetBuyPrice(UProductItem* Product, int32 Quantity) const
{
	if (!Product || Quantity <= 0)
	{
		return 0.0f;
	}

	float UnitPrice = GetCurrentProductPrice(Product);
	return UnitPrice * Quantity;
}

float UShopManager::GetSellPrice(UProductItem* Product, int32 Quantity) const
{
	if (!Product || Quantity <= 0)
	{
		return 0.0f;
	}

	float UnitPrice = GetCurrentProductPrice(Product) * Product->SellPriceMultiplier * ShopBuybackMultiplier;
	return UnitPrice * Quantity;
}

void UShopManager::SetPriceFluctuationConfig(const FPriceFluctuationConfig& NewConfig)
{
	FluctuationConfig = NewConfig;
	UpdatePrices();
}

void UShopManager::UpdatePrices()
{
	if (!PriceFluctuationSystem)
	{
		return;
	}

	for (auto& Pair : ShopInventory)
	{
		UpdateProductPrice(Pair.Key);
	}
}

float UShopManager::GetCurrentProductPrice(UProductItem* Product) const
{
	if (!Product)
	{
		return 0.0f;
	}

	const FShopProductEntry* Entry = ShopInventory.Find(Product);
	if (Entry)
	{
		return Entry->CurrentPrice;
	}

	return Product->BasePrice;
}

void UShopManager::UpdateProductPrice(UProductItem* Product)
{
	if (!Product || !PriceFluctuationSystem)
	{
		return;
	}

	FShopProductEntry* Entry = ShopInventory.Find(Product);
	if (Entry)
	{
		Entry->CurrentPrice = PriceFluctuationSystem->CalculateCurrentPrice(Product, FluctuationConfig);
	}
}

bool UShopManager::ProcessBuyTransaction(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity, float TotalPrice)
{
	if (!PlayerInventory->RemoveCurrency(TotalPrice))
	{
		return false;
	}

	if (!PlayerInventory->AddItem(Product, Quantity))
	{
		PlayerInventory->AddCurrency(TotalPrice);
		return false;
	}

	FShopProductEntry* Entry = ShopInventory.Find(Product);
	if (Entry && !Entry->bIsUnlimited)
	{
		Entry->Stock -= Quantity;
		OnShopInventoryChanged.Broadcast(Product, Entry->Stock);
	}

	return true;
}

bool UShopManager::ProcessSellTransaction(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity, float TotalPrice)
{
	if (!PlayerInventory->RemoveItem(Product, Quantity))
	{
		return false;
	}

	PlayerInventory->AddCurrency(TotalPrice);

	FShopProductEntry* Entry = ShopInventory.Find(Product);
	if (Entry && !Entry->bIsUnlimited)
	{
		Entry->Stock += Quantity;
		OnShopInventoryChanged.Broadcast(Product, Entry->Stock);
	}

	return true;
}
