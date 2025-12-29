#include "PriceFluctuationSystem.h"
#include "Kismet/KismetMathLibrary.h"

float UPriceFluctuationSystem::CalculateCurrentPrice(UProductItem* Product, const FPriceFluctuationConfig& Config)
{
	if (!Product)
	{
		return 0.0f;
	}

	float CurrentPrice = Product->BasePrice;

	switch (Config.FluctuationMode)
	{
	case EPriceFluctuationMode::Random:
		CurrentPrice = ApplyRandomFluctuation(CurrentPrice, Config.MinPriceMultiplier, Config.MaxPriceMultiplier);
		break;

	case EPriceFluctuationMode::SupplyDemand:
		{
			int32 Sales = GetRecentSales(Product);
			CurrentPrice = ApplySupplyDemandFluctuation(CurrentPrice, 0, Sales, Config);
		}
		break;

	case EPriceFluctuationMode::TimeBased:
		CurrentPrice = ApplyTimeBasedFluctuation(CurrentPrice, 0.5f, Config);
		break;

	case EPriceFluctuationMode::EventBased:
	case EPriceFluctuationMode::None:
	default:
		break;
	}

	return FMath::Max(1.0f, CurrentPrice);
}

float UPriceFluctuationSystem::ApplyRandomFluctuation(float BasePrice, float MinMultiplier, float MaxMultiplier)
{
	float RandomMultiplier = FMath::FRandRange(MinMultiplier, MaxMultiplier);
	return BasePrice * RandomMultiplier;
}

float UPriceFluctuationSystem::ApplySupplyDemandFluctuation(float BasePrice, int32 StockLevel, int32 RecentSales, const FPriceFluctuationConfig& Config)
{
	float DemandMultiplier = 1.0f;

	if (Config.bAffectsByDemand && RecentSales > 0)
	{
		float DemandFactor = FMath::Min(RecentSales / 10.0f, 5.0f);
		DemandMultiplier = 1.0f + (DemandFactor * Config.DemandInfluence);
	}

	float FinalMultiplier = FMath::Clamp(DemandMultiplier, Config.MinPriceMultiplier, Config.MaxPriceMultiplier);
	return BasePrice * FinalMultiplier;
}

float UPriceFluctuationSystem::ApplyTimeBasedFluctuation(float BasePrice, float TimeOfDay, const FPriceFluctuationConfig& Config)
{
	float TimeMultiplier = 1.0f + FMath::Sin(TimeOfDay * PI * 2.0f) * 0.2f;
	TimeMultiplier = FMath::Clamp(TimeMultiplier, Config.MinPriceMultiplier, Config.MaxPriceMultiplier);
	return BasePrice * TimeMultiplier;
}

void UPriceFluctuationSystem::RegisterProductSale(UProductItem* Product, int32 Quantity)
{
	if (!Product || Quantity <= 0)
	{
		return;
	}

	int32* ExistingSales = RecentSalesData.Find(Product);
	if (ExistingSales)
	{
		*ExistingSales += Quantity;
	}
	else
	{
		RecentSalesData.Add(Product, Quantity);
	}
}

void UPriceFluctuationSystem::RegisterProductPurchase(UProductItem* Product, int32 Quantity)
{
	if (!Product || Quantity <= 0)
	{
		return;
	}

	int32* ExistingPurchases = RecentPurchaseData.Find(Product);
	if (ExistingPurchases)
	{
		*ExistingPurchases += Quantity;
	}
	else
	{
		RecentPurchaseData.Add(Product, Quantity);
	}
}

int32 UPriceFluctuationSystem::GetRecentSales(UProductItem* Product) const
{
	if (!Product)
	{
		return 0;
	}

	const int32* Sales = RecentSalesData.Find(Product);
	return Sales ? *Sales : 0;
}

void UPriceFluctuationSystem::ResetDemandData()
{
	RecentSalesData.Empty();
	RecentPurchaseData.Empty();
}

float UPriceFluctuationSystem::GetDemandMultiplier(UProductItem* Product, const FPriceFluctuationConfig& Config) const
{
	if (!Product || !Config.bAffectsByDemand)
	{
		return 1.0f;
	}

	int32 Sales = GetRecentSales(Product);
	if (Sales <= 0)
	{
		return 1.0f;
	}

	float DemandFactor = FMath::Min(Sales / 10.0f, 5.0f);
	return 1.0f + (DemandFactor * Config.DemandInfluence);
}
