// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AGAbilitySystemComponent.generated.h"

class UAGGameplayAbility;

/**
 * Custom Ability System Component with cooldown checking and input binding support
 */
UCLASS(ClassGroup = AbilitySystem, meta = (BlueprintSpawnableComponent))
class AGABILITYSYSTEM_API UAGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UAGAbilitySystemComponent();

	// Grant an ability and return its handle
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<UAGGameplayAbility> AbilityClass, int32 Level = 1, int32 InputID = -1);

	// Remove an ability by handle
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RemoveAbility(FGameplayAbilitySpecHandle AbilityHandle);

	// Check if an ability is on cooldown
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	bool IsAbilityOnCooldown(TSubclassOf<UAGGameplayAbility> AbilityClass) const;

	// Get remaining cooldown time for an ability
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	float GetAbilityCooldownTimeRemaining(TSubclassOf<UAGGameplayAbility> AbilityClass) const;

	// Try to activate ability by class
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbilityByClass(TSubclassOf<UAGGameplayAbility> AbilityClass);

	// Input binding
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BindAbilityActivationToInputComponent(UInputComponent* InputComponent);

protected:
	// Input handlers
	void AbilityInputTagPressed(int32 InputID);
	void AbilityInputTagReleased(int32 InputID);

	// Granted abilities for tracking
	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	TArray<FGameplayAbilitySpecHandle> GrantedAbilities;
};
