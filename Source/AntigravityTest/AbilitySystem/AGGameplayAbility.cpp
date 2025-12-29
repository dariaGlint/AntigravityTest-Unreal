// Fill out your copyright notice in the Description page of Project Settings.

#include "AGGameplayAbility.h"
#include "AGAbilitySystemComponent.h"
#include "AGAttributeSet.h"
#include "AbilitySystemComponent.h"

UAGGameplayAbility::UAGGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Default values
	ResourceType = EAbilityResourceType::None;
	ResourceCost = 0.f;
	ComboIndex = 0;
	NextComboAbility = nullptr;
	ComboWindowDuration = 1.0f;
}

bool UAGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Check if we have enough resources
	if (!HasEnoughResources(ActorInfo))
	{
		return false;
	}

	return true;
}

void UAGGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		// Apply resource cost
		ApplyResourceCost(ActorInfo);

		// Apply cooldown
		ApplyCooldown(Handle, ActorInfo, ActivationInfo);

		// Call blueprint event
		OnAbilityActivated();
	}
}

void UAGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Call blueprint event
	OnAbilityEnded(bWasCancelled);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAGGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownDuration.GetValue() > 0.f && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		// If we have a cooldown effect class, use it
		if (CooldownEffect)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(ActorInfo->AvatarActor.Get());

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CooldownEffect, GetAbilityLevel(), EffectContext);
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		// Otherwise, apply cooldown tags manually
		else if (!CooldownTags.IsEmpty())
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(ActorInfo->AvatarActor.Get());

			// Create a temporary cooldown effect
			FGameplayEffectSpec* CooldownSpec = new FGameplayEffectSpec();
			CooldownSpec->Duration = CooldownDuration.GetValue();
			CooldownSpec->Period = 0.f;
			CooldownSpec->DynamicGrantedTags.AppendTags(CooldownTags);
		}
	}
}

bool UAGGameplayAbility::HasEnoughResources(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (ResourceType == EAbilityResourceType::None || ResourceCost <= 0.f)
	{
		return true;
	}

	UAGAttributeSet* AttributeSet = GetAttributeSet(ActorInfo);
	if (!AttributeSet)
	{
		return false;
	}

	switch (ResourceType)
	{
	case EAbilityResourceType::Health:
		return AttributeSet->GetHealth() >= ResourceCost;
	case EAbilityResourceType::Mana:
		return AttributeSet->GetMana() >= ResourceCost;
	case EAbilityResourceType::Stamina:
		return AttributeSet->GetStamina() >= ResourceCost;
	default:
		return true;
	}
}

void UAGGameplayAbility::ApplyResourceCost(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (ResourceType == EAbilityResourceType::None || ResourceCost <= 0.f)
	{
		return;
	}

	UAGAttributeSet* AttributeSet = GetAttributeSet(ActorInfo);
	if (!AttributeSet)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	switch (ResourceType)
	{
	case EAbilityResourceType::Health:
		ASC->ApplyModToAttribute(AttributeSet->GetHealthAttribute(), EGameplayModOp::Additive, -ResourceCost);
		break;
	case EAbilityResourceType::Mana:
		ASC->ApplyModToAttribute(AttributeSet->GetManaAttribute(), EGameplayModOp::Additive, -ResourceCost);
		break;
	case EAbilityResourceType::Stamina:
		ASC->ApplyModToAttribute(AttributeSet->GetStaminaAttribute(), EGameplayModOp::Additive, -ResourceCost);
		break;
	default:
		break;
	}
}

bool UAGGameplayAbility::CanComboInto(TSubclassOf<UAGGameplayAbility> TargetAbility) const
{
	return NextComboAbility && NextComboAbility == TargetAbility;
}

UAGAttributeSet* UAGGameplayAbility::GetAttributeSet(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return nullptr;
	}

	return const_cast<UAGAttributeSet*>(ActorInfo->AbilitySystemComponent->GetSet<UAGAttributeSet>());
}
