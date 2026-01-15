#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ResourceDropTable.generated.h"

class UItemDefinition;

/**
 * Represents a single item drop entry with quantity range and drop chance
 */
USTRUCT(BlueprintType)
struct FResourceDropEntry
{
	GENERATED_BODY()

	// The item that can be dropped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	TObjectPtr<UItemDefinition> ItemDefinition;

	// Minimum quantity to drop (if dropped)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (ClampMin = "1"))
	int32 MinQuantity = 1;

	// Maximum quantity to drop (if dropped)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (ClampMin = "1"))
	int32 MaxQuantity = 1;

	// Drop chance (0.0 to 1.0, where 1.0 = 100% chance)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 1.0f;
};

/**
 * Defines a table of possible drops for a resource node
 */
UCLASS(BlueprintType)
class RESOURCECOLLECTIONSYSTEM_API UResourceDropTable : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// List of possible drops
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop Table")
	TArray<FResourceDropEntry> Drops;

	// Whether to drop all items that pass their drop chance, or only one random item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop Table")
	bool bDropAllSuccessful = true;

	/**
	 * Roll for drops based on this table
	 * @param OutDrops - Array of items and quantities that were successfully rolled
	 * @param SkillModifier - Multiplier for quantities based on skill level (1.0 = no change)
	 */
	UFUNCTION(BlueprintCallable, Category = "Drop Table")
	void RollDrops(TArray<TPair<UItemDefinition*, int32>>& OutDrops, float SkillModifier = 1.0f) const;
};
