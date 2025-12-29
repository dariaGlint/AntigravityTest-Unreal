#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProductItem.h"
#include "PriceFluctuationSystem.generated.h"

UENUM(BlueprintType)
enum class EPriceFluctuationMode : uint8
{
	None UMETA(DisplayName = "None"),
	Random UMETA(DisplayName = "Random"),
	SupplyDemand UMETA(DisplayName = "Supply & Demand"),
	TimeBased UMETA(DisplayName = "Time Based"),
	EventBased UMETA(DisplayName = "Event Based")
};

USTRUCT(BlueprintType)
struct FPriceFluctuationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPriceFluctuationMode FluctuationMode = EPriceFluctuationMode::Random;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float MinPriceMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1.0", ClampMax = "10.0"))
	float MaxPriceMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float FluctuationInterval = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAffectsByDemand = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DemandInfluence = 0.3f;
};

/**
 * System for managing dynamic price fluctuations
 */
UCLASS(BlueprintType)
class SHOPTRADINGSYSTEM_API UPriceFluctuationSystem : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	float CalculateCurrentPrice(UProductItem* Product, const FPriceFluctuationConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	float ApplyRandomFluctuation(float BasePrice, float MinMultiplier, float MaxMultiplier);

	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	float ApplySupplyDemandFluctuation(float BasePrice, int32 StockLevel, int32 RecentSales, const FPriceFluctuationConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	float ApplyTimeBasedFluctuation(float BasePrice, float TimeOfDay, const FPriceFluctuationConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	void RegisterProductSale(UProductItem* Product, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	void RegisterProductPurchase(UProductItem* Product, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Price Fluctuation")
	int32 GetRecentSales(UProductItem* Product) const;

	UFUNCTION(BlueprintCallable, Category = "Price Fluctuation")
	void ResetDemandData();

protected:
	UPROPERTY()
	TMap<UProductItem*, int32> RecentSalesData;

	UPROPERTY()
	TMap<UProductItem*, int32> RecentPurchaseData;

private:
	float GetDemandMultiplier(UProductItem* Product, const FPriceFluctuationConfig& Config) const;
};
