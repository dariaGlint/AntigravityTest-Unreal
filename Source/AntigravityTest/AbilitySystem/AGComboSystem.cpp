// Fill out your copyright notice in the Description page of Project Settings.

#include "AGComboSystem.h"
#include "AGGameplayAbility.h"
#include "AGAbilitySystemComponent.h"

UAGComboSystem::UAGComboSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	CurrentComboChainIndex = INDEX_NONE;
	CurrentComboIndex = 0;
	ComboCount = 0;
	bIsInCombo = false;
	ComboWindowTimer = 0.f;
}

void UAGComboSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UAGComboSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update combo window timer
	if (bIsInCombo && ComboWindowTimer > 0.f)
	{
		ComboWindowTimer -= DeltaTime;
		if (ComboWindowTimer <= 0.f)
		{
			ResetCombo();
		}
	}
}

void UAGComboSystem::RegisterComboChain(const FComboChain& Chain)
{
	ComboChains.Add(Chain);
}

bool UAGComboSystem::StartCombo(TSubclassOf<UAGGameplayAbility> StartAbility)
{
	if (!StartAbility)
	{
		return false;
	}

	// Find a combo chain that starts with this ability
	int32 ChainIndex = FindComboChainIndex(StartAbility);
	if (ChainIndex == INDEX_NONE)
	{
		// Not a combo starter, reset
		ResetCombo();
		return false;
	}

	// Start the combo
	CurrentComboChainIndex = ChainIndex;
	CurrentComboIndex = 0;
	bIsInCombo = true;
	IncrementComboCount();

	// Set combo window timer
	if (ComboChains.IsValidIndex(CurrentComboChainIndex))
	{
		ComboWindowTimer = ComboChains[CurrentComboChainIndex].ComboWindow;
	}

	return true;
}

bool UAGComboSystem::ContinueCombo(TSubclassOf<UAGGameplayAbility> NextAbility)
{
	if (!bIsInCombo || !NextAbility)
	{
		return false;
	}

	// Check if this ability is next in the combo
	if (!IsNextInCombo(NextAbility))
	{
		ResetCombo();
		return false;
	}

	// Continue the combo
	CurrentComboIndex++;
	IncrementComboCount();

	// Check if we've reached the end of the combo chain
	if (ComboChains.IsValidIndex(CurrentComboChainIndex))
	{
		const FComboChain& CurrentChain = ComboChains[CurrentComboChainIndex];

		if (CurrentComboIndex >= CurrentChain.Abilities.Num())
		{
			if (CurrentChain.bCanLoop)
			{
				CurrentComboIndex = 0;
			}
			else
			{
				ResetCombo();
				return true;
			}
		}

		// Reset combo window timer
		ComboWindowTimer = CurrentChain.ComboWindow;
	}

	return true;
}

void UAGComboSystem::ResetCombo()
{
	CurrentComboChainIndex = INDEX_NONE;
	CurrentComboIndex = 0;
	ComboCount = 0;
	bIsInCombo = false;
	ComboWindowTimer = 0.f;

	OnComboReset.Broadcast();
}

TSubclassOf<UAGGameplayAbility> UAGComboSystem::GetNextComboAbility() const
{
	if (!bIsInCombo || !ComboChains.IsValidIndex(CurrentComboChainIndex))
	{
		return nullptr;
	}

	const FComboChain& CurrentChain = ComboChains[CurrentComboChainIndex];
	int32 NextIndex = CurrentComboIndex + 1;

	if (NextIndex >= CurrentChain.Abilities.Num())
	{
		if (CurrentChain.bCanLoop)
		{
			NextIndex = 0;
		}
		else
		{
			return nullptr;
		}
	}

	if (CurrentChain.Abilities.IsValidIndex(NextIndex))
	{
		return CurrentChain.Abilities[NextIndex];
	}

	return nullptr;
}

float UAGComboSystem::GetComboWindowTimeRemaining() const
{
	return FMath::Max(0.f, ComboWindowTimer);
}

int32 UAGComboSystem::FindComboChainIndex(TSubclassOf<UAGGameplayAbility> StartAbility) const
{
	for (int32 i = 0; i < ComboChains.Num(); i++)
	{
		if (ComboChains[i].Abilities.Num() > 0 && ComboChains[i].Abilities[0] == StartAbility)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

bool UAGComboSystem::IsNextInCombo(TSubclassOf<UAGGameplayAbility> Ability) const
{
	if (!ComboChains.IsValidIndex(CurrentComboChainIndex))
	{
		return false;
	}

	const FComboChain& CurrentChain = ComboChains[CurrentComboChainIndex];
	int32 NextIndex = CurrentComboIndex + 1;

	if (NextIndex >= CurrentChain.Abilities.Num())
	{
		if (CurrentChain.bCanLoop)
		{
			NextIndex = 0;
		}
		else
		{
			return false;
		}
	}

	if (CurrentChain.Abilities.IsValidIndex(NextIndex))
	{
		return CurrentChain.Abilities[NextIndex] == Ability;
	}

	return false;
}

void UAGComboSystem::IncrementComboCount()
{
	ComboCount++;
	OnComboCountChanged.Broadcast(ComboCount);
}
