// Copyright Epic Games, Inc. All Rights Reserved.

#include "CraftingRecipe.h"

bool UCraftingRecipe::CanCraft(const TMap<FName, int32>& AvailableMaterials) const
{
	// 必要な素材がすべて揃っているかチェック
	for (const FCraftingMaterial& Material : RequiredMaterials)
	{
		const int32* AvailableAmount = AvailableMaterials.Find(Material.MaterialID);

		// 素材が存在しないか、数量が不足している場合
		if (!AvailableAmount || *AvailableAmount < Material.RequiredAmount)
		{
			return false;
		}
	}

	return true;
}

void UCraftingRecipe::GetMissingMaterials(const TMap<FName, int32>& AvailableMaterials, TMap<FName, int32>& OutMissingMaterials) const
{
	OutMissingMaterials.Empty();

	// 各必要素材をチェック
	for (const FCraftingMaterial& Material : RequiredMaterials)
	{
		const int32* AvailableAmount = AvailableMaterials.Find(Material.MaterialID);
		int32 CurrentAmount = AvailableAmount ? *AvailableAmount : 0;

		// 不足している場合はリストに追加
		if (CurrentAmount < Material.RequiredAmount)
		{
			OutMissingMaterials.Add(Material.MaterialID, Material.RequiredAmount - CurrentAmount);
		}
	}
}
