#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "HarvestingSkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillLevelChanged, FGameplayTag, SkillTag, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillExperienceGained, FGameplayTag, SkillTag, int32, ExperienceGained, int32, TotalExperience);

/**
 * Represents a single harvesting skill (e.g., Mining, Woodcutting, Herbalism)
 */
USTRUCT(BlueprintType)
struct FHarvestingSkill
{
	GENERATED_BODY()

	// Gameplay tag identifying this skill (e.g., "Skill.Mining", "Skill.Woodcutting")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FGameplayTag SkillTag;

	// Current level (1-based)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "1"))
	int32 Level = 1;

	// Current experience points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "0"))
	int32 Experience = 0;

	// Experience required for next level (recalculated on level up)
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	int32 ExperienceToNextLevel = 100;

	// Bonus multiplier to resource quantities per level (additive)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "0.0"))
	float BonusPerLevel = 0.05f; // 5% per level

	/**
	 * Calculate the total skill modifier for resource quantities
	 */
	float GetSkillModifier() const
	{
		return 1.0f + (Level - 1) * BonusPerLevel;
	}

	/**
	 * Check if this skill should level up and perform level up if needed
	 * @return True if leveled up
	 */
	bool CheckLevelUp()
	{
		if (Experience >= ExperienceToNextLevel)
		{
			Level++;
			Experience -= ExperienceToNextLevel;

			// Simple exponential curve for level requirements
			ExperienceToNextLevel = FMath::RoundToInt(100.0f * FMath::Pow(1.15f, Level - 1));

			return true;
		}
		return false;
	}
};

/**
 * Component for managing harvesting skills on a character
 * Tracks experience, levels, and skill-based bonuses
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RESOURCECOLLECTIONSYSTEM_API UHarvestingSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHarvestingSkillComponent();

	// List of harvesting skills this character has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harvesting Skills")
	TArray<FHarvestingSkill> Skills;

	// Base experience gained per harvest action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harvesting Skills", meta = (ClampMin = "0"))
	int32 BaseExperiencePerHarvest = 10;

	/**
	 * Get a skill by its tag
	 * @param SkillTag - Tag identifying the skill
	 * @param OutSkill - The found skill
	 * @return True if skill was found
	 */
	UFUNCTION(BlueprintCallable, Category = "Harvesting Skills")
	bool GetSkill(FGameplayTag SkillTag, FHarvestingSkill& OutSkill) const;

	/**
	 * Get the skill modifier for a specific skill
	 * @param SkillTag - Tag identifying the skill
	 * @return Multiplier for resource quantities (1.0 if skill not found)
	 */
	UFUNCTION(BlueprintPure, Category = "Harvesting Skills")
	float GetSkillModifier(FGameplayTag SkillTag) const;

	/**
	 * Add experience to a skill
	 * @param SkillTag - Tag identifying the skill
	 * @param ExperienceAmount - Amount of experience to add
	 * @return True if skill was found and experience was added
	 */
	UFUNCTION(BlueprintCallable, Category = "Harvesting Skills")
	bool AddSkillExperience(FGameplayTag SkillTag, int32 ExperienceAmount);

	/**
	 * Add a new skill to the character
	 * @param SkillTag - Tag identifying the skill
	 * @param StartingLevel - Initial level for the skill
	 * @return True if skill was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Harvesting Skills")
	bool AddSkill(FGameplayTag SkillTag, int32 StartingLevel = 1);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Harvesting Skills|Events")
	FOnSkillLevelChanged OnSkillLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Harvesting Skills|Events")
	FOnSkillExperienceGained OnSkillExperienceGained;
};
