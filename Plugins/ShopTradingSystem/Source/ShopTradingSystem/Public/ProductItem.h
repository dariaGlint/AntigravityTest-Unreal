#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProductItem.generated.h"

UENUM(BlueprintType)
enum class EProductCategory : uint8
{
	Consumable UMETA(DisplayName = "Consumable"),
	Equipment UMETA(DisplayName = "Equipment"),
	Material UMETA(DisplayName = "Material"),
	Quest UMETA(DisplayName = "Quest"),
	Misc UMETA(DisplayName = "Miscellaneous")
};

UENUM(BlueprintType)
enum class EProductRarity : uint8
{
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

/**
 * Data asset that defines a product item in the shop
 */
UCLASS(BlueprintType)
class SHOPTRADINGSYSTEM_API UProductItem : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Product Info")
	FText ProductName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Product Info")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Product Info")
	EProductCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Product Info")
	EProductRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Product Info")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Product Info")
	int32 MaxStackSize = 99;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pricing")
	float BasePrice = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pricing")
	float SellPriceMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Settings")
	bool bCanBuy = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Settings")
	bool bCanSell = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Settings")
	bool bIsUnlimited = false;
};

/**
 * Runtime instance of a product in inventory or shop
 */
USTRUCT(BlueprintType)
struct SHOPTRADINGSYSTEM_API FProductInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProductItem* ProductData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentPrice = 0.0f;

	FProductInstance()
		: ProductData(nullptr)
		, Quantity(1)
		, CurrentPrice(0.0f)
	{}

	FProductInstance(UProductItem* InProduct, int32 InQuantity, float InPrice)
		: ProductData(InProduct)
		, Quantity(InQuantity)
		, CurrentPrice(InPrice)
	{}

	bool IsValid() const
	{
		return ProductData != nullptr && Quantity > 0;
	}
};
