// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGComboSystem.h"
#include "AGGameplayAbility.h"

UAGComboSystem::UAGComboSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	CurrentComboCount = 0;
	bComboActive = false;
	ComboTimeRemaining = 0.0f;
}

void UAGComboSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UAGComboSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bComboActive)
	{
		ComboTimeRemaining -= DeltaTime;
		if (ComboTimeRemaining <= 0.0f)
		{
			ResetCombo();
		}
	}
}

void UAGComboSystem::RegisterComboChain(const FAGComboChain& ComboChain)
{
	if (ComboChain.ComboTag.IsValid() && ComboChain.AbilityChain.Num() > 0)
	{
		ComboChains.Add(ComboChain.ComboTag, ComboChain);
	}
}

bool UAGComboSystem::StartCombo(FGameplayTag ComboTag)
{
	if (!ComboTag.IsValid())
	{
		return false;
	}

	const FAGComboChain* Chain = ComboChains.Find(ComboTag);
	if (!Chain || Chain->AbilityChain.Num() == 0)
	{
		return false;
	}

	CurrentComboTag = ComboTag;
	CurrentComboCount = 0;
	bComboActive = true;
	ComboTimeRemaining = Chain->ComboWindowDuration;

	SetComponentTickEnabled(true);

	OnComboProgressed.Broadcast(CurrentComboTag, CurrentComboCount);

	return true;
}

bool UAGComboSystem::ProgressCombo()
{
	if (!bComboActive)
	{
		return false;
	}

	const FAGComboChain* Chain = ComboChains.Find(CurrentComboTag);
	if (!Chain)
	{
		ResetCombo();
		return false;
	}

	// Check if we can progress
	if (CurrentComboCount >= Chain->AbilityChain.Num())
	{
		ResetCombo();
		return false;
	}

	CurrentComboCount++;
	ResetComboTimer();

	OnComboProgressed.Broadcast(CurrentComboTag, CurrentComboCount);

	// Check if combo is complete
	if (CurrentComboCount >= Chain->AbilityChain.Num())
	{
		ResetCombo();
	}

	return true;
}

void UAGComboSystem::ResetCombo()
{
	if (bComboActive)
	{
		OnComboEnded.Broadcast();
	}

	CurrentComboTag = FGameplayTag();
	CurrentComboCount = 0;
	bComboActive = false;
	ComboTimeRemaining = 0.0f;

	SetComponentTickEnabled(false);
}

TSubclassOf<UAGGameplayAbility> UAGComboSystem::GetNextAbilityInCombo() const
{
	if (!bComboActive)
	{
		return nullptr;
	}

	const FAGComboChain* Chain = ComboChains.Find(CurrentComboTag);
	if (!Chain)
	{
		return nullptr;
	}

	if (CurrentComboCount < Chain->AbilityChain.Num())
	{
		return Chain->AbilityChain[CurrentComboCount];
	}

	return nullptr;
}

void UAGComboSystem::ResetComboTimer()
{
	const FAGComboChain* Chain = ComboChains.Find(CurrentComboTag);
	if (Chain)
	{
		ComboTimeRemaining = Chain->ComboWindowDuration;
	}
}
