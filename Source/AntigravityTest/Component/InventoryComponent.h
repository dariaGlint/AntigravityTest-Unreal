#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANTIGRAVITYTEST_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	// Add an item to the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(UItemDefinition* ItemDef, int32 Quantity = 1);

	// Remove an item from the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemDefinition* ItemDef, int32 Quantity = 1);

	// Get all items (const reference)
	const TArray<FInventoryItem>& GetItems() const { return Items; }

	// Delegate fired when inventory changes
	FOnInventoryUpdated OnInventoryUpdated;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FInventoryItem> Items;
};
