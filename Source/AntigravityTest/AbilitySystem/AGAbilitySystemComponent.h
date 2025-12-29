// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AGAbilitySystemComponent.generated.h"

/**
 * Custom Ability System Component for AntigravityTest
 * Manages abilities, attributes, and gameplay effects
 */
UCLASS()
class ANTIGRAVITYTEST_API UAGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UAGAbilitySystemComponent();

	// Called when the component is initialized
	virtual void InitializeComponent() override;

	// Grants a gameplay ability to this component
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<UGameplayAbility> Ability, int32 Level = 1);

	// Grants multiple gameplay abilities at once
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities, int32 Level = 1);

	// Removes a previously granted ability
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RemoveAbility(FGameplayAbilitySpecHandle Handle);

	// Activates an ability by class
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);

	// Gets the remaining cooldown time for an ability
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetAbilityCooldownTimeRemaining(TSubclassOf<UGameplayAbility> AbilityClass) const;

	// Checks if an ability is currently on cooldown
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsAbilityOnCooldown(TSubclassOf<UGameplayAbility> AbilityClass) const;

	// Receives input for abilities
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AbilityInputPressed(int32 InputID);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AbilityInputReleased(int32 InputID);

protected:
	// List of abilities granted on initialization
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	// List of effects granted on initialization
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
};
