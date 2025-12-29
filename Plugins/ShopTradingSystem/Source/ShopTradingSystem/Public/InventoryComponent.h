#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProductItem.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, UProductItem*, Product, int32, NewQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrencyChanged, float, NewAmount);

/**
 * Component for managing player/NPC inventory
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOPTRADINGSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UProductItem* Product, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UProductItem* Product, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(UProductItem* Product, int32 Quantity = 1) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(UProductItem* Product) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FProductInstance> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();

	UFUNCTION(BlueprintCallable, Category = "Currency")
	bool AddCurrency(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Currency")
	bool RemoveCurrency(float Amount);

	UFUNCTION(BlueprintPure, Category = "Currency")
	float GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void SetCurrency(float NewAmount);

	UFUNCTION(BlueprintPure, Category = "Currency")
	bool HasEnoughCurrency(float Amount) const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Currency")
	FOnCurrencyChanged OnCurrencyChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Settings")
	int32 MaxSlots = 100;

protected:
	UPROPERTY(SaveGame)
	TMap<UProductItem*, int32> Items;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Currency")
	float Currency = 0.0f;

private:
	bool CanAddItem(UProductItem* Product, int32 Quantity) const;
	int32 GetUsedSlots() const;
};
