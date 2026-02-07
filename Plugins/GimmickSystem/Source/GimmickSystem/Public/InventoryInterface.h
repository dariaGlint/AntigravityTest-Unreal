#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "InventoryInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * インベントリインターフェース
 * Interface for inventory systems to check for items/keys
 */
class GIMMICKSYSTEM_API IInventoryInterface
{
	GENERATED_BODY()

public:
	/**
	 * 特定のタグを持つアイテムを持っているかチェック
	 * Check if inventory contains an item with the specified tag
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
	bool HasItemWithTag(const FGameplayTag& Tag) const;
};
