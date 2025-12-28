#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDefinition.generated.h"

/**
 * Definition of an item in the game.
 */
UCLASS(BlueprintType)
class ANTIGRAVITYTEST_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Display Name of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	// Description of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	// Icon to display in UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon;
};
