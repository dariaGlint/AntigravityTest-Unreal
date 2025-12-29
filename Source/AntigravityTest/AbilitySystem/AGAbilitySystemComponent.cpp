// Fill out your copyright notice in the Description page of Project Settings.

#include "AGAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbility.h"

UAGAbilitySystemComponent::UAGAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Enable replication
	SetIsReplicatedByDefault(true);
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
}

void UAGAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Grant default abilities
	if (GetOwnerRole() == ROLE_Authority)
	{
		for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
		{
			if (Ability)
			{
				GrantAbility(Ability);
			}
		}

		// Apply default effects
		for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
		{
			if (Effect)
			{
				FGameplayEffectContextHandle EffectContext = MakeEffectContext();
				EffectContext.AddSourceObject(GetOwner());

				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Effect, 1.0f, EffectContext);
				if (SpecHandle.IsValid())
				{
					ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}
}

FGameplayAbilitySpecHandle UAGAbilitySystemComponent::GrantAbility(TSubclassOf<UGameplayAbility> Ability, int32 Level)
{
	if (!Ability)
	{
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec AbilitySpec(Ability, Level, INDEX_NONE, GetOwner());
	return GiveAbility(AbilitySpec);
}

void UAGAbilitySystemComponent::GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities, int32 Level)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
	{
		GrantAbility(Ability, Level);
	}
}

void UAGAbilitySystemComponent::RemoveAbility(FGameplayAbilitySpecHandle Handle)
{
	if (Handle.IsValid())
	{
		ClearAbility(Handle);
	}
}

bool UAGAbilitySystemComponent::ActivateAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return false;
	}

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass);
	if (Spec)
	{
		return TryActivateAbility(Spec->Handle);
	}

	return false;
}

float UAGAbilitySystemComponent::GetAbilityCooldownTimeRemaining(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	if (!AbilityClass)
	{
		return 0.f;
	}

	const UGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UGameplayAbility>();
	if (!AbilityCDO)
	{
		return 0.f;
	}

	FGameplayTagContainer CooldownTags;
	AbilityCDO->GetCooldownTags(CooldownTags);

	if (CooldownTags.IsEmpty())
	{
		return 0.f;
	}

	float TimeRemaining = 0.f;
	float Duration = 0.f;

	FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	TArray<TPair<float, float>> DurationAndTimeRemaining = GetActiveEffectsTimeRemainingAndDuration(Query);

	if (DurationAndTimeRemaining.Num() > 0)
	{
		int32 BestIdx = 0;
		float LongestTime = DurationAndTimeRemaining[0].Key;
		for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
		{
			if (DurationAndTimeRemaining[Idx].Key > LongestTime)
			{
				LongestTime = DurationAndTimeRemaining[Idx].Key;
				BestIdx = Idx;
			}
		}

		TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
		Duration = DurationAndTimeRemaining[BestIdx].Value;
	}

	return TimeRemaining;
}

bool UAGAbilitySystemComponent::IsAbilityOnCooldown(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	return GetAbilityCooldownTimeRemaining(AbilityClass) > 0.f;
}

void UAGAbilitySystemComponent::AbilityInputPressed(int32 InputID)
{
	if (InputID < 0)
	{
		return;
	}

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					AbilitySpecInputPressed(Spec);
				}
				else
				{
					TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UAGAbilitySystemComponent::AbilityInputReleased(int32 InputID)
{
	if (InputID < 0)
	{
		return;
	}

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID && Spec.Ability)
		{
			Spec.InputPressed = false;
			if (Spec.IsActive())
			{
				AbilitySpecInputReleased(Spec);
			}
		}
	}
}
