// Copyright Epic Games, Inc. All Rights Reserved.

#include "CraftingManager.h"

UCraftingManager::UCraftingManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsCrafting = false;
	CurrentCraftingRecipe = nullptr;
	CraftingElapsedTime = 0.0f;
}

void UCraftingManager::BeginPlay()
{
	Super::BeginPlay();
}

void UCraftingManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// クラフト中の場合、進捗を更新
	if (bIsCrafting && CurrentCraftingRecipe)
	{
		CraftingElapsedTime += DeltaTime;

		// クラフト完了チェック
		if (CraftingElapsedTime >= CurrentCraftingRecipe->CraftingTime)
		{
			CompleteCrafting();
		}
	}
}

void UCraftingManager::RegisterRecipe(UCraftingRecipe* Recipe)
{
	if (Recipe && Recipe->RecipeID != NAME_None)
	{
		Recipes.Add(Recipe->RecipeID, Recipe);
	}
}

void UCraftingManager::UnregisterRecipe(FName RecipeID)
{
	Recipes.Remove(RecipeID);
}

UCraftingRecipe* UCraftingManager::GetRecipe(FName RecipeID) const
{
	const UCraftingRecipe* const* RecipePtr = Recipes.Find(RecipeID);
	return RecipePtr ? const_cast<UCraftingRecipe*>(*RecipePtr) : nullptr;
}

TArray<UCraftingRecipe*> UCraftingManager::GetAllRecipes() const
{
	TArray<UCraftingRecipe*> AllRecipes;
	Recipes.GenerateValueArray(AllRecipes);
	return AllRecipes;
}

TArray<UCraftingRecipe*> UCraftingManager::GetUnlockedRecipes() const
{
	TArray<UCraftingRecipe*> UnlockedRecipes;

	for (const auto& Pair : Recipes)
	{
		if (Pair.Value && Pair.Value->bIsUnlocked)
		{
			UnlockedRecipes.Add(Pair.Value);
		}
	}

	return UnlockedRecipes;
}

void UCraftingManager::AddMaterial(FName MaterialID, int32 Amount)
{
	if (MaterialID == NAME_None || Amount <= 0)
	{
		return;
	}

	int32* CurrentAmount = Materials.Find(MaterialID);
	if (CurrentAmount)
	{
		*CurrentAmount += Amount;
	}
	else
	{
		Materials.Add(MaterialID, Amount);
	}
}

int32 UCraftingManager::RemoveMaterial(FName MaterialID, int32 Amount)
{
	if (MaterialID == NAME_None || Amount <= 0)
	{
		return 0;
	}

	int32* CurrentAmount = Materials.Find(MaterialID);
	if (!CurrentAmount)
	{
		return 0;
	}

	int32 RemovedAmount = FMath::Min(*CurrentAmount, Amount);
	*CurrentAmount -= RemovedAmount;

	// 数量が0になった場合は削除
	if (*CurrentAmount <= 0)
	{
		Materials.Remove(MaterialID);
	}

	return RemovedAmount;
}

int32 UCraftingManager::GetMaterialAmount(FName MaterialID) const
{
	const int32* Amount = Materials.Find(MaterialID);
	return Amount ? *Amount : 0;
}

TMap<FName, int32> UCraftingManager::GetAllMaterials() const
{
	return Materials;
}

bool UCraftingManager::StartCrafting(FName RecipeID)
{
	// すでにクラフト中の場合は失敗
	if (bIsCrafting)
	{
		return false;
	}

	// レシピを取得
	UCraftingRecipe* Recipe = GetRecipe(RecipeID);
	if (!Recipe)
	{
		return false;
	}

	// レシピがアンロックされているかチェック
	if (!Recipe->bIsUnlocked)
	{
		return false;
	}

	// 素材が十分にあるかチェック
	if (!Recipe->CanCraft(Materials))
	{
		OnCraftingFailed.Broadcast(RecipeID);
		return false;
	}

	// 素材を消費
	UsedMaterials.Empty();
	for (const FCraftingMaterial& Material : Recipe->RequiredMaterials)
	{
		int32 RemovedAmount = RemoveMaterial(Material.MaterialID, Material.RequiredAmount);
		UsedMaterials.Add(Material.MaterialID, RemovedAmount);
	}

	// クラフト開始
	bIsCrafting = true;
	CurrentCraftingRecipe = Recipe;
	CraftingElapsedTime = 0.0f;

	return true;
}

bool UCraftingManager::CanCraftRecipe(FName RecipeID) const
{
	UCraftingRecipe* Recipe = GetRecipe(RecipeID);
	if (!Recipe || !Recipe->bIsUnlocked)
	{
		return false;
	}

	return Recipe->CanCraft(Materials);
}

float UCraftingManager::GetCraftingProgress() const
{
	if (!bIsCrafting || !CurrentCraftingRecipe)
	{
		return 0.0f;
	}

	if (CurrentCraftingRecipe->CraftingTime <= 0.0f)
	{
		return 1.0f;
	}

	return FMath::Clamp(CraftingElapsedTime / CurrentCraftingRecipe->CraftingTime, 0.0f, 1.0f);
}

void UCraftingManager::CancelCrafting(bool bRefundMaterials)
{
	if (!bIsCrafting)
	{
		return;
	}

	// 素材を返却
	if (bRefundMaterials)
	{
		for (const auto& Pair : UsedMaterials)
		{
			AddMaterial(Pair.Key, Pair.Value);
		}
	}

	// クラフト状態をリセット
	bIsCrafting = false;
	CurrentCraftingRecipe = nullptr;
	CraftingElapsedTime = 0.0f;
	UsedMaterials.Empty();
}

void UCraftingManager::CompleteCrafting()
{
	if (!CurrentCraftingRecipe)
	{
		return;
	}

	// アイテムを生成（インベントリに追加）
	AddMaterial(CurrentCraftingRecipe->Result.ItemID, CurrentCraftingRecipe->Result.Amount);

	// イベントを発行
	OnCraftingCompleted.Broadcast(
		CurrentCraftingRecipe->Result.ItemID,
		CurrentCraftingRecipe->Result.Amount
	);

	// クラフト状態をリセット
	bIsCrafting = false;
	CurrentCraftingRecipe = nullptr;
	CraftingElapsedTime = 0.0f;
	UsedMaterials.Empty();
}
