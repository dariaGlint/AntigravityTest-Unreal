// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGBuffDebuffEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Actor.h"

UAGBuffDebuffManager::UAGBuffDebuffManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAGBuffDebuffManager::BeginPlay()
{
	Super::BeginPlay();

	// Get ability system component from owner
	AActor* Owner = GetOwner();
	if (Owner)
	{
		AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	}
}

bool UAGBuffDebuffManager::ApplyEffect(const FAGBuffDebuffData& EffectData, AActor* Instigator)
{
	if (!AbilitySystemComponent || !EffectData.GameplayEffect)
	{
		return false;
	}

	// Check if we already have this effect
	if (ActiveEffects.Contains(EffectData.EffectTag))
	{
		// Check stack limit
		int32 CurrentStacks = EffectStacks.FindRef(EffectData.EffectTag);
		if (CurrentStacks >= EffectData.MaxStacks)
		{
			return false;
		}

		// Increment stack count
		EffectStacks[EffectData.EffectTag] = CurrentStacks + 1;
		OnBuffDebuffApplied.Broadcast(EffectData.EffectTag, EffectStacks[EffectData.EffectTag]);
		return true;
	}

	// Create effect context
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	if (Instigator)
	{
		EffectContext.AddInstigator(Instigator, Instigator);
	}

	// Create effect spec
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		EffectData.GameplayEffect,
		1.0f,
		EffectContext
	);

	if (SpecHandle.IsValid())
	{
		// Apply the effect
		FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		if (ActiveHandle.IsValid())
		{
			ActiveEffects.Add(EffectData.EffectTag, ActiveHandle);
			EffectStacks.Add(EffectData.EffectTag, 1);
			OnBuffDebuffApplied.Broadcast(EffectData.EffectTag, 1);
			return true;
		}
	}

	return false;
}

bool UAGBuffDebuffManager::RemoveEffect(FGameplayTag EffectTag)
{
	if (!AbilitySystemComponent)
	{
		return false;
	}

	FActiveGameplayEffectHandle* Handle = ActiveEffects.Find(EffectTag);
	if (Handle && Handle->IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(*Handle);
		ActiveEffects.Remove(EffectTag);
		EffectStacks.Remove(EffectTag);
		OnBuffDebuffRemoved.Broadcast(EffectTag);
		return true;
	}

	return false;
}

void UAGBuffDebuffManager::RemoveAllEffects()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	TArray<FGameplayTag> EffectTags;
	ActiveEffects.GetKeys(EffectTags);

	for (const FGameplayTag& Tag : EffectTags)
	{
		RemoveEffect(Tag);
	}
}

bool UAGBuffDebuffManager::HasEffect(FGameplayTag EffectTag) const
{
	return ActiveEffects.Contains(EffectTag);
}

int32 UAGBuffDebuffManager::GetEffectStackCount(FGameplayTag EffectTag) const
{
	return EffectStacks.FindRef(EffectTag);
}

float UAGBuffDebuffManager::GetEffectTimeRemaining(FGameplayTag EffectTag) const
{
	if (!AbilitySystemComponent)
	{
		return 0.0f;
	}

	const FActiveGameplayEffectHandle* Handle = ActiveEffects.Find(EffectTag);
	if (Handle && Handle->IsValid())
	{
		return AbilitySystemComponent->GetGameplayEffectDuration(*Handle);
	}

	return 0.0f;
}

TArray<FGameplayTag> UAGBuffDebuffManager::GetActiveEffects() const
{
	TArray<FGameplayTag> EffectTags;
	ActiveEffects.GetKeys(EffectTags);
	return EffectTags;
}
