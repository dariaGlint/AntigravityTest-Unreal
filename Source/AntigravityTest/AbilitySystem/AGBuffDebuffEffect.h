// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AGBuffDebuffEffect.generated.h"

/**
 * Types of buff/debuff effects
 */
UENUM(BlueprintType)
enum class EBuffDebuffType : uint8
{
	Buff,
	Debuff,
	Neutral
};

/**
 * Base class for creating buff and debuff effects
 * These are configured through Blueprints or C++
 */
UCLASS(Blueprintable)
class ANTIGRAVITYTEST_API UAGBuffDebuffEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UAGBuffDebuffEffect();

	// Type of effect (Buff, Debuff, or Neutral)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Buff/Debuff")
	EBuffDebuffType EffectType;

	// Display name for UI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Buff/Debuff")
	FText DisplayName;

	// Description for UI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Buff/Debuff")
	FText Description;

	// Icon for UI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Buff/Debuff")
	UTexture2D* Icon;

	// Whether this effect can stack
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Buff/Debuff")
	bool bCanStack;

	// Maximum number of stacks
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Buff/Debuff", meta = (EditCondition = "bCanStack"))
	int32 MaxStacks;
};

/**
 * Component for tracking active buffs and debuffs
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ANTIGRAVITYTEST_API UAGBuffDebuffManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAGBuffDebuffManager();

	virtual void BeginPlay() override;

	// Apply a buff or debuff
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	FActiveGameplayEffectHandle ApplyBuffDebuff(TSubclassOf<UAGBuffDebuffEffect> EffectClass, float Level = 1.0f);

	// Remove a buff or debuff by handle
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	bool RemoveBuffDebuff(FActiveGameplayEffectHandle Handle);

	// Get all active buffs
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	TArray<FActiveGameplayEffectHandle> GetActiveBuffs() const;

	// Get all active debuffs
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	TArray<FActiveGameplayEffectHandle> GetActiveDebuffs() const;

	// Check if a specific buff/debuff is active
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	bool HasBuffDebuff(TSubclassOf<UAGBuffDebuffEffect> EffectClass) const;

	// Get remaining duration of a buff/debuff
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	float GetBuffDebuffRemainingTime(TSubclassOf<UAGBuffDebuffEffect> EffectClass) const;

	// Event called when a buff is applied
	UPROPERTY(BlueprintAssignable, Category = "Buff/Debuff")
	FOnGameplayEffectAppliedDelegate OnBuffApplied;

	// Event called when a debuff is applied
	UPROPERTY(BlueprintAssignable, Category = "Buff/Debuff")
	FOnGameplayEffectAppliedDelegate OnDebuffApplied;

	// Event called when a buff is removed
	UPROPERTY(BlueprintAssignable, Category = "Buff/Debuff")
	FOnGameplayEffectRemoved OnBuffRemoved;

	// Event called when a debuff is removed
	UPROPERTY(BlueprintAssignable, Category = "Buff/Debuff")
	FOnGameplayEffectRemoved OnDebuffRemoved;

protected:
	// Reference to the ability system component
	UPROPERTY()
	class UAGAbilitySystemComponent* AbilitySystemComponent;

	// Helper to get ASC
	class UAGAbilitySystemComponent* GetAbilitySystemComponent();

private:
	// Cached buff/debuff handles
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> ActiveBuffs;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> ActiveDebuffs;

	// Callbacks for effect application/removal
	void OnEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	void OnEffectRemoved(const FActiveGameplayEffect& RemovedEffect);
};
