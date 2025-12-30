// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AGComboSystem.generated.h"

class UAGGameplayAbility;

/**
 * Delegate for combo events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComboProgressed, FGameplayTag, ComboTag, int32, ComboCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboEnded);

/**
 * Combo chain definition
 */
USTRUCT(BlueprintType)
struct FAGComboChain
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	FGameplayTag ComboTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<TSubclassOf<UAGGameplayAbility>> AbilityChain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float ComboWindowDuration;

	FAGComboChain()
		: ComboWindowDuration(1.0f)
	{
	}
};

/**
 * Component for managing ability combos
 */
UCLASS(ClassGroup = AbilitySystem, meta = (BlueprintSpawnableComponent))
class AGABILITYSYSTEM_API UAGComboSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UAGComboSystem();

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnComboProgressed OnComboProgressed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnComboEnded OnComboEnded;

	// Register a combo chain
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void RegisterComboChain(const FAGComboChain& ComboChain);

	// Start a combo
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool StartCombo(FGameplayTag ComboTag);

	// Progress combo to next ability
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool ProgressCombo();

	// Reset current combo
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();

	// Get current combo count
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combo")
	int32 GetCurrentComboCount() const { return CurrentComboCount; }

	// Get current combo tag
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combo")
	FGameplayTag GetCurrentComboTag() const { return CurrentComboTag; }

	// Check if combo is active
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combo")
	bool IsComboActive() const { return bComboActive; }

	// Get next ability in combo
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combo")
	TSubclassOf<UAGGameplayAbility> GetNextAbilityInCombo() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Registered combo chains
	UPROPERTY()
	TMap<FGameplayTag, FAGComboChain> ComboChains;

	// Current combo state
	UPROPERTY()
	FGameplayTag CurrentComboTag;

	UPROPERTY()
	int32 CurrentComboCount;

	UPROPERTY()
	bool bComboActive;

	UPROPERTY()
	float ComboTimeRemaining;

	// Reset combo timer
	void ResetComboTimer();
};
