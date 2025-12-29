// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGAbilitySystemComponent.h"
#include "AGGameplayAbility.h"
#include "GameplayTagContainer.h"

UAGAbilitySystemComponent::UAGAbilitySystemComponent()
{
}

FGameplayAbilitySpecHandle UAGAbilitySystemComponent::GrantAbility(TSubclassOf<UAGGameplayAbility> AbilityClass, int32 Level, int32 InputID)
{
	if (!AbilityClass)
	{
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec AbilitySpec(AbilityClass, Level, InputID, this);
	FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

	if (Handle.IsValid())
	{
		GrantedAbilities.Add(Handle);
	}

	return Handle;
}

void UAGAbilitySystemComponent::RemoveAbility(FGameplayAbilitySpecHandle AbilityHandle)
{
	if (AbilityHandle.IsValid())
	{
		ClearAbility(AbilityHandle);
		GrantedAbilities.Remove(AbilityHandle);
	}
}

bool UAGAbilitySystemComponent::IsAbilityOnCooldown(TSubclassOf<UAGGameplayAbility> AbilityClass) const
{
	if (!AbilityClass)
	{
		return false;
	}

	const UAGGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UAGGameplayAbility>();
	if (AbilityCDO && AbilityCDO->CooldownTag.IsValid())
	{
		return HasMatchingGameplayTag(AbilityCDO->CooldownTag);
	}

	return false;
}

float UAGAbilitySystemComponent::GetAbilityCooldownTimeRemaining(TSubclassOf<UAGGameplayAbility> AbilityClass) const
{
	if (!AbilityClass)
	{
		return 0.0f;
	}

	const UAGGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UAGGameplayAbility>();
	if (AbilityCDO && AbilityCDO->CooldownTag.IsValid())
	{
		FGameplayTagContainer CooldownTags;
		CooldownTags.AddTag(AbilityCDO->CooldownTag);

		FGameplayEffectQuery Query;
		Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAnyTags(CooldownTags);

		TArray<float> Durations = GetActiveEffectsTimeRemaining(Query);
		if (Durations.Num() > 0)
		{
			Durations.Sort();
			return Durations[Durations.Num() - 1];
		}
	}

	return 0.0f;
}

bool UAGAbilitySystemComponent::TryActivateAbilityByClass(TSubclassOf<UAGGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return false;
	}

	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilities)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (Spec && Spec->Ability && Spec->Ability->GetClass() == AbilityClass)
		{
			return TryActivateAbility(Handle);
		}
	}

	return false;
}

void UAGAbilitySystemComponent::BindAbilityActivationToInputComponent(UInputComponent* InputComponent)
{
	if (!InputComponent)
	{
		return;
	}

	// This is a simplified binding system
	// In a real project, you might want to use Enhanced Input System or similar
}

void UAGAbilitySystemComponent::AbilityInputTagPressed(int32 InputID)
{
	if (InputID < 0)
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.InputID == InputID)
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UAGAbilitySystemComponent::AbilityInputTagReleased(int32 InputID)
{
	if (InputID < 0)
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.InputID == InputID && Spec.IsActive())
		{
			CancelAbilityHandle(Spec.Handle);
		}
	}
}
