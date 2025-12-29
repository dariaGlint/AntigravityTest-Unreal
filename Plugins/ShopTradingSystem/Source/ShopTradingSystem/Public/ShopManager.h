#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ProductItem.h"
#include "PriceFluctuationSystem.h"
#include "InventoryComponent.h"
#include "ShopManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTransactionComplete, bool, bSuccess, UProductItem*, Product, int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShopInventoryChanged, UProductItem*, Product, int32, NewStock);

USTRUCT(BlueprintType)
struct FShopProductEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProductItem* Product;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Stock = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsUnlimited = false;

	UPROPERTY(BlueprintReadOnly)
	float CurrentPrice = 0.0f;

	FShopProductEntry()
		: Product(nullptr)
		, Stock(0)
		, bIsUnlimited(false)
		, CurrentPrice(0.0f)
	{}

	FShopProductEntry(UProductItem* InProduct, int32 InStock, bool bInUnlimited)
		: Product(InProduct)
		, Stock(InStock)
		, bIsUnlimited(bInUnlimited)
		, CurrentPrice(InProduct ? InProduct->BasePrice : 0.0f)
	{}
};

/**
 * Main subsystem for managing shop operations, inventory, and transactions
 */
UCLASS()
class SHOPTRADINGSYSTEM_API UShopManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Shop Inventory Management
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void AddProductToShop(UProductItem* Product, int32 InitialStock, bool bIsUnlimited = false);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RemoveProductFromShop(UProductItem* Product);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpdateProductStock(UProductItem* Product, int32 NewStock);

	UFUNCTION(BlueprintPure, Category = "Shop")
	TArray<FShopProductEntry> GetAvailableProducts() const;

	UFUNCTION(BlueprintPure, Category = "Shop")
	bool IsProductAvailable(UProductItem* Product) const;

	UFUNCTION(BlueprintPure, Category = "Shop")
	int32 GetProductStock(UProductItem* Product) const;

	// Transaction Operations
	UFUNCTION(BlueprintCallable, Category = "Shop|Transaction")
	bool BuyProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Shop|Transaction")
	bool SellProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Shop|Transaction")
	bool CanBuyProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity) const;

	UFUNCTION(BlueprintPure, Category = "Shop|Transaction")
	bool CanSellProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity) const;

	UFUNCTION(BlueprintPure, Category = "Shop|Transaction")
	float GetBuyPrice(UProductItem* Product, int32 Quantity) const;

	UFUNCTION(BlueprintPure, Category = "Shop|Transaction")
	float GetSellPrice(UProductItem* Product, int32 Quantity) const;

	// Price Fluctuation
	UFUNCTION(BlueprintCallable, Category = "Shop|Pricing")
	void SetPriceFluctuationConfig(const FPriceFluctuationConfig& NewConfig);

	UFUNCTION(BlueprintPure, Category = "Shop|Pricing")
	FPriceFluctuationConfig GetPriceFluctuationConfig() const { return FluctuationConfig; }

	UFUNCTION(BlueprintCallable, Category = "Shop|Pricing")
	void UpdatePrices();

	UFUNCTION(BlueprintPure, Category = "Shop|Pricing")
	float GetCurrentProductPrice(UProductItem* Product) const;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FOnTransactionComplete OnTransactionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FOnShopInventoryChanged OnShopInventoryChanged;

protected:
	UPROPERTY()
	TMap<UProductItem*, FShopProductEntry> ShopInventory;

	UPROPERTY()
	UPriceFluctuationSystem* PriceFluctuationSystem;

	UPROPERTY(EditAnywhere, Category = "Shop Settings")
	FPriceFluctuationConfig FluctuationConfig;

	UPROPERTY(EditAnywhere, Category = "Shop Settings")
	float ShopBuybackMultiplier = 0.7f;

private:
	void UpdateProductPrice(UProductItem* Product);
	bool ProcessBuyTransaction(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity, float TotalPrice);
	bool ProcessSellTransaction(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity, float TotalPrice);
};
