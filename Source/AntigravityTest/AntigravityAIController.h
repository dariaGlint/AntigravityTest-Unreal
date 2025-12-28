// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AntigravityAIController.generated.h"

/**
 * AI Controller that makes the character jump periodically.
 */
UCLASS()
class AAntigravityAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAntigravityAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	UFUNCTION()
	void PerformJump();
private:
	FTimerHandle TimerHandle_Jump;
};
