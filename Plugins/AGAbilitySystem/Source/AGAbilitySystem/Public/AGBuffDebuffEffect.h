// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "AGBuffDebuffEffect.generated.h"

class UAbilitySystemComponent;

/**
 * Type of buff/debuff effect
 */
UENUM(BlueprintType)
enum class EAGEffectType : uint8
{
	Buff UMETA(DisplayName = "Buff"),
	Debuff UMETA(DisplayName = "Debuff")
};

/**
 * Delegate for buff/debuff events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffDebuffApplied, FGameplayTag, EffectTag, int32, StackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffDebuffRemoved, FGameplayTag, EffectTag);

/**
 * Buff/Debuff effect data
 */
USTRUCT(BlueprintType)
struct FAGBuffDebuffData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EAGEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int32 MaxStacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> GameplayEffect;

	FAGBuffDebuffData()
		: EffectType(EAGEffectType::Buff)
		, Duration(10.0f)
		, MaxStacks(1)
		, GameplayEffect(nullptr)
	{
	}
};

/**
 * Component for managing buffs and debuffs
 */
UCLASS(ClassGroup = AbilitySystem, meta = (BlueprintSpawnableComponent))
class AGABILITYSYSTEM_API UAGBuffDebuffManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAGBuffDebuffManager();

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuffDebuffApplied OnBuffDebuffApplied;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuffDebuffRemoved OnBuffDebuffRemoved;

	// Apply a buff or debuff
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	bool ApplyEffect(const FAGBuffDebuffData& EffectData, AActor* Instigator = nullptr);

	// Remove a buff or debuff
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	bool RemoveEffect(FGameplayTag EffectTag);

	// Remove all effects
	UFUNCTION(BlueprintCallable, Category = "Buff/Debuff")
	void RemoveAllEffects();

	// Check if effect is active
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Buff/Debuff")
	bool HasEffect(FGameplayTag EffectTag) const;

	// Get stack count for an effect
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Buff/Debuff")
	int32 GetEffectStackCount(FGameplayTag EffectTag) const;

	// Get time remaining for an effect
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Buff/Debuff")
	float GetEffectTimeRemaining(FGameplayTag EffectTag) const;

	// Get all active effects
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Buff/Debuff")
	TArray<FGameplayTag> GetActiveEffects() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	// Active effects tracking
	UPROPERTY()
	TMap<FGameplayTag, FActiveGameplayEffectHandle> ActiveEffects;

	UPROPERTY()
	TMap<FGameplayTag, int32> EffectStacks;
};
