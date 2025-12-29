// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AGGameplayAbility.generated.h"

/**
 * Resource type for ability costs
 */
UENUM(BlueprintType)
enum class EAbilityResourceType : uint8
{
	None,
	Health,
	Mana,
	Stamina
};

/**
 * Base Gameplay Ability class with enhanced features
 * Supports cooldowns, resource costs, and combo systems
 */
UCLASS()
class ANTIGRAVITYTEST_API UAGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAGGameplayAbility();

	// Ability activation
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Cooldown
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cooldown")
	FScalableFloat CooldownDuration;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;

	// Apply cooldown to this ability
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;

	// Resource Cost
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cost")
	EAbilityResourceType ResourceType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cost")
	float ResourceCost;

	// Check if the actor has enough resources
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool HasEnoughResources(const FGameplayAbilityActorInfo* ActorInfo) const;

	// Apply resource cost
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ApplyResourceCost(const FGameplayAbilityActorInfo* ActorInfo);

	// Combo System
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
	int32 ComboIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
	TSubclassOf<UAGGameplayAbility> NextComboAbility;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
	float ComboWindowDuration;

	// Checks if this ability can combo into another
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool CanComboInto(TSubclassOf<UAGGameplayAbility> TargetAbility) const;

protected:
	// Override these in child classes
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityEnded(bool bWasCancelled);

	// Internal cooldown effect
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffect;

	// Helper to get attribute set
	class UAGAttributeSet* GetAttributeSet(const FGameplayAbilityActorInfo* ActorInfo) const;
};
