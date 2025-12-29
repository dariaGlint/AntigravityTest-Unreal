// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGGameplayAbility.h"
#include "AGAbilitySystemComponent.h"
#include "AGAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UAGGameplayAbility::UAGGameplayAbility()
{
	CooldownDuration = 0.0f;
	ResourceType = EAGResourceType::None;
	ResourceCost = 0.0f;
	ComboWindowDuration = 1.0f;

	// Set default replication policy
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UAGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Check cooldown
	if (IsOnCooldown())
	{
		return false;
	}

	// Check resources
	if (!HasEnoughResources())
	{
		return false;
	}

	return true;
}

void UAGGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Consume resources
	CheckAndConsumeResources();

	// Trigger Blueprint event
	BP_OnAbilityActivated();
}

void UAGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Apply cooldown when ability ends
	if (!bWasCancelled && CooldownDuration > 0.0f)
	{
		ApplyCooldown();
	}

	// Trigger Blueprint event
	BP_OnAbilityEnded();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UAGGameplayAbility::IsOnCooldown() const
{
	if (CooldownTag.IsValid())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			return ASC->HasMatchingGameplayTag(CooldownTag);
		}
	}
	return false;
}

float UAGGameplayAbility::GetCooldownTimeRemaining() const
{
	if (!CooldownTag.IsValid())
	{
		return 0.0f;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayTagContainer CooldownTags;
		CooldownTags.AddTag(CooldownTag);

		FGameplayEffectQuery Query;
		Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAnyTags(CooldownTags);

		TArray<float> Durations = ASC->GetActiveEffectsTimeRemaining(Query);
		if (Durations.Num() > 0)
		{
			Durations.Sort();
			return Durations[Durations.Num() - 1];
		}
	}

	return 0.0f;
}

void UAGGameplayAbility::ApplyCooldown()
{
	if (CooldownDuration <= 0.0f || !CooldownTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	// Create a dynamic gameplay effect for cooldown
	UGameplayEffect* CooldownGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Cooldown")));
	CooldownGE->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	CooldownGE->DurationMagnitude = FScalableFloat(CooldownDuration);

	FInheritedTagContainer TagContainer;
	TagContainer.Added.AddTag(CooldownTag);
	CooldownGE->InheritableOwnedTagsContainer = TagContainer;

	ASC->ApplyGameplayEffectToSelf(CooldownGE, 1.0f, ASC->MakeEffectContext());
}

bool UAGGameplayAbility::CheckAndConsumeResources()
{
	if (ResourceType == EAGResourceType::None || ResourceCost <= 0.0f)
	{
		return true;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	const UAGAttributeSet* AttributeSet = ASC->GetSet<UAGAttributeSet>();
	if (!AttributeSet)
	{
		return false;
	}

	// Create gameplay effect to consume resource
	UGameplayEffect* CostGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AbilityCost")));
	CostGE->DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FScalableFloat(-ResourceCost);

	switch (ResourceType)
	{
	case EAGResourceType::Mana:
		ModifierInfo.Attribute = UAGAttributeSet::GetManaAttribute();
		break;
	case EAGResourceType::Stamina:
		ModifierInfo.Attribute = UAGAttributeSet::GetStaminaAttribute();
		break;
	case EAGResourceType::Health:
		ModifierInfo.Attribute = UAGAttributeSet::GetHealthAttribute();
		break;
	default:
		return true;
	}

	CostGE->Modifiers.Add(ModifierInfo);
	ASC->ApplyGameplayEffectToSelf(CostGE, 1.0f, ASC->MakeEffectContext());

	return true;
}

bool UAGGameplayAbility::HasEnoughResources() const
{
	if (ResourceType == EAGResourceType::None || ResourceCost <= 0.0f)
	{
		return true;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	const UAGAttributeSet* AttributeSet = ASC->GetSet<UAGAttributeSet>();
	if (!AttributeSet)
	{
		return false;
	}

	float CurrentValue = 0.0f;
	switch (ResourceType)
	{
	case EAGResourceType::Mana:
		CurrentValue = AttributeSet->GetMana();
		break;
	case EAGResourceType::Stamina:
		CurrentValue = AttributeSet->GetStamina();
		break;
	case EAGResourceType::Health:
		CurrentValue = AttributeSet->GetHealth();
		break;
	default:
		return true;
	}

	return CurrentValue >= ResourceCost;
}
