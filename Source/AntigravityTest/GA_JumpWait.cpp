// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_JumpWait.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_JumpWait::UGA_JumpWait()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_JumpWait::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		// Wait for 1 second
		UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, 1.0f);
		Task->OnFinish.AddDynamic(this, &UGA_JumpWait::OnDelayFinished);
		Task->ReadyForActivation();
	}
}

void UGA_JumpWait::OnDelayFinished()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->Jump();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_JumpWait::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		Character->StopJumping();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
