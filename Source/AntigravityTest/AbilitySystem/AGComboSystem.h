// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AGComboSystem.generated.h"

class UAGGameplayAbility;

/**
 * Combo chain data
 */
USTRUCT(BlueprintType)
struct FComboChain
{
	GENERATED_BODY()

	// Abilities in this combo chain
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UAGGameplayAbility>> Abilities;

	// Time window to continue the combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ComboWindow;

	// Whether this combo can loop back to the start
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanLoop;

	FComboChain()
		: ComboWindow(1.0f)
		, bCanLoop(false)
	{
	}
};

/**
 * Combo System Component
 * Manages ability combos and chains
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ANTIGRAVITYTEST_API UAGComboSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UAGComboSystem();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Register a combo chain
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void RegisterComboChain(const FComboChain& Chain);

	// Start a combo with the given ability
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool StartCombo(TSubclassOf<UAGGameplayAbility> StartAbility);

	// Continue the combo with the next ability
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool ContinueCombo(TSubclassOf<UAGGameplayAbility> NextAbility);

	// Reset the current combo
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();

	// Get the current combo index
	UFUNCTION(BlueprintPure, Category = "Combo")
	int32 GetCurrentComboIndex() const { return CurrentComboIndex; }

	// Get the current combo count
	UFUNCTION(BlueprintPure, Category = "Combo")
	int32 GetComboCount() const { return ComboCount; }

	// Check if we're currently in a combo
	UFUNCTION(BlueprintPure, Category = "Combo")
	bool IsInCombo() const { return bIsInCombo; }

	// Get the next ability in the current combo chain
	UFUNCTION(BlueprintPure, Category = "Combo")
	TSubclassOf<UAGGameplayAbility> GetNextComboAbility() const;

	// Get the time remaining in the combo window
	UFUNCTION(BlueprintPure, Category = "Combo")
	float GetComboWindowTimeRemaining() const;

	// Event called when combo count increases
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboCountChanged, int32, NewComboCount);
	UPROPERTY(BlueprintAssignable, Category = "Combo")
	FOnComboCountChanged OnComboCountChanged;

	// Event called when combo is reset
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboReset);
	UPROPERTY(BlueprintAssignable, Category = "Combo")
	FOnComboReset OnComboReset;

protected:
	// Registered combo chains
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<FComboChain> ComboChains;

	// Current combo chain being executed
	UPROPERTY()
	int32 CurrentComboChainIndex;

	// Current position in the combo chain
	UPROPERTY()
	int32 CurrentComboIndex;

	// Total combo count (resets when combo breaks)
	UPROPERTY()
	int32 ComboCount;

	// Whether we're currently in a combo
	UPROPERTY()
	bool bIsInCombo;

	// Time remaining in the combo window
	UPROPERTY()
	float ComboWindowTimer;

	// Find combo chain starting with the given ability
	int32 FindComboChainIndex(TSubclassOf<UAGGameplayAbility> StartAbility) const;

	// Check if ability is next in current combo
	bool IsNextInCombo(TSubclassOf<UAGGameplayAbility> Ability) const;

	// Increment combo count
	void IncrementComboCount();
};
