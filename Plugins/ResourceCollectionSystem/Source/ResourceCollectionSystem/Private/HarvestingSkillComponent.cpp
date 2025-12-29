#include "HarvestingSkillComponent.h"

UHarvestingSkillComponent::UHarvestingSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UHarvestingSkillComponent::GetSkill(FGameplayTag SkillTag, FHarvestingSkill& OutSkill) const
{
	for (const FHarvestingSkill& Skill : Skills)
	{
		if (Skill.SkillTag == SkillTag)
		{
			OutSkill = Skill;
			return true;
		}
	}
	return false;
}

float UHarvestingSkillComponent::GetSkillModifier(FGameplayTag SkillTag) const
{
	FHarvestingSkill Skill;
	if (GetSkill(SkillTag, Skill))
	{
		return Skill.GetSkillModifier();
	}
	return 1.0f;
}

bool UHarvestingSkillComponent::AddSkillExperience(FGameplayTag SkillTag, int32 ExperienceAmount)
{
	for (FHarvestingSkill& Skill : Skills)
	{
		if (Skill.SkillTag == SkillTag)
		{
			int32 OldLevel = Skill.Level;
			Skill.Experience += ExperienceAmount;

			// Broadcast experience gained
			OnSkillExperienceGained.Broadcast(SkillTag, ExperienceAmount, Skill.Experience);

			// Check for level ups (may level up multiple times)
			bool bLeveledUp = false;
			while (Skill.CheckLevelUp())
			{
				bLeveledUp = true;
			}

			// Broadcast level change if leveled up
			if (bLeveledUp)
			{
				OnSkillLevelChanged.Broadcast(SkillTag, Skill.Level);
			}

			return true;
		}
	}
	return false;
}

bool UHarvestingSkillComponent::AddSkill(FGameplayTag SkillTag, int32 StartingLevel)
{
	// Check if skill already exists
	for (const FHarvestingSkill& Skill : Skills)
	{
		if (Skill.SkillTag == SkillTag)
		{
			return false; // Skill already exists
		}
	}

	// Create new skill
	FHarvestingSkill NewSkill;
	NewSkill.SkillTag = SkillTag;
	NewSkill.Level = FMath::Max(1, StartingLevel);
	NewSkill.Experience = 0;
	NewSkill.ExperienceToNextLevel = FMath::RoundToInt(100.0f * FMath::Pow(1.15f, NewSkill.Level - 1));

	Skills.Add(NewSkill);
	return true;
}
