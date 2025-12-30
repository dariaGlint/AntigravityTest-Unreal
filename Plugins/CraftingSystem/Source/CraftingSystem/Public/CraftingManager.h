// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftingRecipe.h"
#include "CraftingManager.generated.h"

/**
 * クラフト完了時のデリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCraftingCompleted, FName, ItemID, int32, Amount);

/**
 * クラフト失敗時のデリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingFailed, FName, RecipeID);

/**
 * クラフトシステムを管理するコンポーネント
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRAFTINGSYSTEM_API UCraftingManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftingManager();

	/** クラフト完了時のイベント */
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnCraftingCompleted OnCraftingCompleted;

	/** クラフト失敗時のイベント */
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnCraftingFailed OnCraftingFailed;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
	 * レシピを登録する
	 * @param Recipe 登録するレシピ
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void RegisterRecipe(UCraftingRecipe* Recipe);

	/**
	 * レシピを削除する
	 * @param RecipeID 削除するレシピのID
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void UnregisterRecipe(FName RecipeID);

	/**
	 * レシピを取得する
	 * @param RecipeID レシピのID
	 * @return レシピオブジェクト（見つからない場合はnullptr）
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	UCraftingRecipe* GetRecipe(FName RecipeID) const;

	/**
	 * 全てのレシピを取得する
	 * @return 登録されているすべてのレシピ
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	TArray<UCraftingRecipe*> GetAllRecipes() const;

	/**
	 * アンロック済みのレシピのみを取得する
	 * @return アンロック済みのレシピ
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	TArray<UCraftingRecipe*> GetUnlockedRecipes() const;

	/**
	 * 素材を追加する
	 * @param MaterialID 素材のID
	 * @param Amount 追加する数量
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddMaterial(FName MaterialID, int32 Amount);

	/**
	 * 素材を削除する
	 * @param MaterialID 素材のID
	 * @param Amount 削除する数量
	 * @return 実際に削除された数量
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	int32 RemoveMaterial(FName MaterialID, int32 Amount);

	/**
	 * 素材の所持数を取得する
	 * @param MaterialID 素材のID
	 * @return 所持数
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	int32 GetMaterialAmount(FName MaterialID) const;

	/**
	 * すべての素材を取得する
	 * @return 所持している素材のマップ
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	TMap<FName, int32> GetAllMaterials() const;

	/**
	 * クラフトを開始する
	 * @param RecipeID レシピのID
	 * @return クラフトが開始された場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool StartCrafting(FName RecipeID);

	/**
	 * レシピがクラフト可能かチェックする
	 * @param RecipeID レシピのID
	 * @return クラフト可能な場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CanCraftRecipe(FName RecipeID) const;

	/**
	 * 現在クラフト中かチェックする
	 * @return クラフト中の場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool IsCrafting() const { return bIsCrafting; }

	/**
	 * クラフトの進捗を取得する（0.0 - 1.0）
	 * @return 進捗率
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	float GetCraftingProgress() const;

	/**
	 * 現在クラフト中のレシピを取得する
	 * @return クラフト中のレシピ（クラフト中でない場合はnullptr）
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	UCraftingRecipe* GetCurrentCraftingRecipe() const { return CurrentCraftingRecipe; }

	/**
	 * クラフトをキャンセルする
	 * @param bRefundMaterials 素材を返却するか
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void CancelCrafting(bool bRefundMaterials = true);

protected:
	/** 登録されているレシピのマップ */
	UPROPERTY()
	TMap<FName, UCraftingRecipe*> Recipes;

	/** 所持している素材のマップ（MaterialID -> Amount） */
	UPROPERTY()
	TMap<FName, int32> Materials;

	/** 現在クラフト中かどうか */
	UPROPERTY()
	bool bIsCrafting;

	/** 現在クラフト中のレシピ */
	UPROPERTY()
	UCraftingRecipe* CurrentCraftingRecipe;

	/** クラフト開始からの経過時間 */
	UPROPERTY()
	float CraftingElapsedTime;

	/** クラフトに使用した素材（キャンセル時の返却用） */
	UPROPERTY()
	TMap<FName, int32> UsedMaterials;

	/**
	 * クラフトを完了する
	 */
	void CompleteCrafting();
};
