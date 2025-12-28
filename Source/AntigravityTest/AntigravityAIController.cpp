// Fill out your copyright notice in the Description page of Project Settings.

#include "AntigravityAIController.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

AAntigravityAIController::AAntigravityAIController()
{
}

void AAntigravityAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Start a repeating timer that calls PerformJump every 3.0 seconds
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TimerHandle_Jump, this, &AAntigravityAIController::PerformJump, 3.0f, true);
	}
}

void AAntigravityAIController::OnUnPossess()
{
	Super::OnUnPossess();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_Jump);
	}
}

void AAntigravityAIController::PerformJump()
{
	// Check if we are controlling a character and make it jump
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		ControlledCharacter->Jump();
	}
}
