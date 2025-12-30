// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "CraftingRecipe.generated.h"

/**
 * 素材アイテムの情報
 */
USTRUCT(BlueprintType)
struct CRAFTINGSYSTEM_API FCraftingMaterial
{
	GENERATED_BODY()

	/** 素材のID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName MaterialID;

	/** 必要な数量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	int32 RequiredAmount;

	FCraftingMaterial()
		: MaterialID(NAME_None)
		, RequiredAmount(1)
	{
	}

	FCraftingMaterial(FName InID, int32 InAmount)
		: MaterialID(InID)
		, RequiredAmount(InAmount)
	{
	}
};

/**
 * クラフト結果アイテムの情報
 */
USTRUCT(BlueprintType)
struct CRAFTINGSYSTEM_API FCraftingResult
{
	GENERATED_BODY()

	/** 生成されるアイテムのID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName ItemID;

	/** 生成される数量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	int32 Amount;

	FCraftingResult()
		: ItemID(NAME_None)
		, Amount(1)
	{
	}

	FCraftingResult(FName InID, int32 InAmount)
		: ItemID(InID)
		, Amount(InAmount)
	{
	}
};

/**
 * クラフトレシピのデータアセット
 */
UCLASS(BlueprintType)
class CRAFTINGSYSTEM_API UCraftingRecipe : public UDataAsset
{
	GENERATED_BODY()

public:
	/** レシピの一意なID */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FName RecipeID;

	/** レシピの表示名 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FText RecipeName;

	/** レシピの説明 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (MultiLine = true))
	FText Description;

	/** 必要な素材のリスト */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	TArray<FCraftingMaterial> RequiredMaterials;

	/** クラフト結果のアイテム */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FCraftingResult Result;

	/** クラフトに必要な時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	float CraftingTime;

	/** レシピがアンロックされているか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	bool bIsUnlocked;

	UCraftingRecipe()
		: RecipeID(NAME_None)
		, CraftingTime(1.0f)
		, bIsUnlocked(true)
	{
	}

	/**
	 * 素材が十分にあるかチェックする
	 * @param AvailableMaterials 利用可能な素材の配列（MaterialID -> Amount）
	 * @return 素材が十分にある場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CanCraft(const TMap<FName, int32>& AvailableMaterials) const;

	/**
	 * 不足している素材のリストを取得する
	 * @param AvailableMaterials 利用可能な素材の配列
	 * @param OutMissingMaterials 不足している素材のリスト（MaterialID -> 不足数）
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void GetMissingMaterials(const TMap<FName, int32>& AvailableMaterials, TMap<FName, int32>& OutMissingMaterials) const;
};
