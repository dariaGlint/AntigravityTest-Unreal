#include "ResourceDropTable.h"

void UResourceDropTable::RollDrops(TArray<TPair<UItemDefinition*, int32>>& OutDrops, float SkillModifier) const
{
	OutDrops.Empty();

	if (bDropAllSuccessful)
	{
		// Roll for each item independently
		for (const FResourceDropEntry& Entry : Drops)
		{
			if (!Entry.ItemDefinition)
			{
				continue;
			}

			// Check if this item should drop
			const float RandomValue = FMath::FRand();
			if (RandomValue <= Entry.DropChance)
			{
				// Calculate quantity with skill modifier
				int32 BaseQuantity = FMath::RandRange(Entry.MinQuantity, Entry.MaxQuantity);
				int32 FinalQuantity = FMath::Max(1, FMath::RoundToInt(BaseQuantity * SkillModifier));

				OutDrops.Add(TPair<UItemDefinition*, int32>(Entry.ItemDefinition, FinalQuantity));
			}
		}
	}
	else
	{
		// Build weighted list of items that passed their drop check
		TArray<const FResourceDropEntry*> ValidDrops;
		for (const FResourceDropEntry& Entry : Drops)
		{
			if (!Entry.ItemDefinition)
			{
				continue;
			}

			const float RandomValue = FMath::FRand();
			if (RandomValue <= Entry.DropChance)
			{
				ValidDrops.Add(&Entry);
			}
		}

		// Pick one random item from valid drops
		if (ValidDrops.Num() > 0)
		{
			const FResourceDropEntry* SelectedEntry = ValidDrops[FMath::RandRange(0, ValidDrops.Num() - 1)];

			// Calculate quantity with skill modifier
			int32 BaseQuantity = FMath::RandRange(SelectedEntry->MinQuantity, SelectedEntry->MaxQuantity);
			int32 FinalQuantity = FMath::Max(1, FMath::RoundToInt(BaseQuantity * SkillModifier));

			OutDrops.Add(TPair<UItemDefinition*, int32>(SelectedEntry->ItemDefinition, FinalQuantity));
		}
	}
}
