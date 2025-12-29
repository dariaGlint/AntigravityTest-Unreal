// Fill out your copyright notice in the Description page of Project Settings.

#include "AGBuffDebuffEffect.h"
#include "AGAbilitySystemComponent.h"
#include "GameFramework/Actor.h"

UAGBuffDebuffEffect::UAGBuffDebuffEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	EffectType = EBuffDebuffType::Neutral;
	bCanStack = false;
	MaxStacks = 1;
}

UAGBuffDebuffManager::UAGBuffDebuffManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	AbilitySystemComponent = nullptr;
}

void UAGBuffDebuffManager::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent = GetAbilitySystemComponent();
	if (AbilitySystemComponent)
	{
		// Register delegates for effect application and removal
		AbilitySystemComponent->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAGBuffDebuffManager::OnEffectApplied);
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UAGBuffDebuffManager::OnEffectRemoved);
	}
}

FActiveGameplayEffectHandle UAGBuffDebuffManager::ApplyBuffDebuff(TSubclassOf<UAGBuffDebuffEffect> EffectClass, float Level)
{
	if (!EffectClass || !AbilitySystemComponent)
	{
		return FActiveGameplayEffectHandle();
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(GetOwner());

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, EffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		// Track the effect
		const UAGBuffDebuffEffect* EffectCDO = EffectClass->GetDefaultObject<UAGBuffDebuffEffect>();
		if (EffectCDO)
		{
			if (EffectCDO->EffectType == EBuffDebuffType::Buff)
			{
				ActiveBuffs.AddUnique(ActiveHandle);
			}
			else if (EffectCDO->EffectType == EBuffDebuffType::Debuff)
			{
				ActiveDebuffs.AddUnique(ActiveHandle);
			}
		}

		return ActiveHandle;
	}

	return FActiveGameplayEffectHandle();
}

bool UAGBuffDebuffManager::RemoveBuffDebuff(FActiveGameplayEffectHandle Handle)
{
	if (!AbilitySystemComponent || !Handle.IsValid())
	{
		return false;
	}

	bool bSuccess = AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
	if (bSuccess)
	{
		ActiveBuffs.Remove(Handle);
		ActiveDebuffs.Remove(Handle);
	}

	return bSuccess;
}

TArray<FActiveGameplayEffectHandle> UAGBuffDebuffManager::GetActiveBuffs() const
{
	return ActiveBuffs;
}

TArray<FActiveGameplayEffectHandle> UAGBuffDebuffManager::GetActiveDebuffs() const
{
	return ActiveDebuffs;
}

bool UAGBuffDebuffManager::HasBuffDebuff(TSubclassOf<UAGBuffDebuffEffect> EffectClass) const
{
	if (!AbilitySystemComponent || !EffectClass)
	{
		return false;
	}

	FGameplayEffectQuery Query;
	Query.EffectDefinition = EffectClass;

	TArray<FActiveGameplayEffectHandle> ActiveEffects = AbilitySystemComponent->GetActiveEffects(Query);
	return ActiveEffects.Num() > 0;
}

float UAGBuffDebuffManager::GetBuffDebuffRemainingTime(TSubclassOf<UAGBuffDebuffEffect> EffectClass) const
{
	if (!AbilitySystemComponent || !EffectClass)
	{
		return 0.f;
	}

	FGameplayEffectQuery Query;
	Query.EffectDefinition = EffectClass;

	TArray<float> Durations = AbilitySystemComponent->GetActiveEffectsTimeRemaining(Query);
	if (Durations.Num() > 0)
	{
		return Durations[0];
	}

	return 0.f;
}

UAGAbilitySystemComponent* UAGBuffDebuffManager::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent;
	}

	AActor* Owner = GetOwner();
	if (Owner)
	{
		AbilitySystemComponent = Cast<UAGAbilitySystemComponent>(Owner->FindComponentByClass(UAGAbilitySystemComponent::StaticClass()));
	}

	return AbilitySystemComponent;
}

void UAGBuffDebuffManager::OnEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	if (!Spec.Def)
	{
		return;
	}

	const UAGBuffDebuffEffect* BuffDebuffEffect = Cast<UAGBuffDebuffEffect>(Spec.Def);
	if (BuffDebuffEffect)
	{
		if (BuffDebuffEffect->EffectType == EBuffDebuffType::Buff)
		{
			OnBuffApplied.Broadcast(ASC, Spec, Handle);
		}
		else if (BuffDebuffEffect->EffectType == EBuffDebuffType::Debuff)
		{
			OnDebuffApplied.Broadcast(ASC, Spec, Handle);
		}
	}
}

void UAGBuffDebuffManager::OnEffectRemoved(const FActiveGameplayEffect& RemovedEffect)
{
	if (!RemovedEffect.Spec.Def)
	{
		return;
	}

	const UAGBuffDebuffEffect* BuffDebuffEffect = Cast<UAGBuffDebuffEffect>(RemovedEffect.Spec.Def);
	if (BuffDebuffEffect)
	{
		FActiveGameplayEffectHandle Handle = RemovedEffect.Handle;

		if (BuffDebuffEffect->EffectType == EBuffDebuffType::Buff)
		{
			ActiveBuffs.Remove(Handle);
			OnBuffRemoved.Broadcast(RemovedEffect);
		}
		else if (BuffDebuffEffect->EffectType == EBuffDebuffType::Debuff)
		{
			ActiveDebuffs.Remove(Handle);
			OnDebuffRemoved.Broadcast(RemovedEffect);
		}
	}
}
