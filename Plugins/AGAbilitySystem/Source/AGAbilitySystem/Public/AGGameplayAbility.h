// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "AGGameplayAbility.generated.h"

/**
 * Resource type for ability costs
 */
UENUM(BlueprintType)
enum class EAGResourceType : uint8
{
	None UMETA(DisplayName = "None"),
	Mana UMETA(DisplayName = "Mana/MP"),
	Stamina UMETA(DisplayName = "Stamina"),
	Health UMETA(DisplayName = "Health")
};

/**
 * Base class for gameplay abilities with cooldown, resource cost, and combo support
 */
UCLASS(Blueprintable, BlueprintType)
class AGABILITYSYSTEM_API UAGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAGGameplayAbility();

	// Cooldown management
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	float CooldownDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;

	// Resource cost
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	EAGResourceType ResourceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	float ResourceCost;

	// Combo system support
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	FGameplayTag ComboTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<FGameplayTag> ComboNextAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	float ComboWindowDuration;

	// Ability overrides
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", meta = (DisplayName = "On Ability Activated"))
	void BP_OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", meta = (DisplayName = "On Ability Ended"))
	void BP_OnAbilityEnded();

	// Check if ability is on cooldown
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	bool IsOnCooldown() const;

	// Get remaining cooldown time
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	float GetCooldownTimeRemaining() const;

protected:
	// Apply cooldown
	void ApplyCooldown();

	// Check and consume resources
	bool CheckAndConsumeResources();

	// Check if we have enough resources
	bool HasEnoughResources() const;
};
