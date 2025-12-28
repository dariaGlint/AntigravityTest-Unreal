// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AntigravityTestGameMode.h"
#include "AntigravityGoalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ANTIGRAVITYTEST_API AAntigravityGoalGameMode : public AAntigravityTestGameMode
{
	GENERATED_BODY()
	
public:
	// ステージクリア処理
	UFUNCTION(BlueprintCallable, Category = "Game")
	void CompleteStage();

protected:
	// Blueprint側でオーバーライド可能なイベント
	UFUNCTION(BlueprintImplementableEvent, Category = "Game")
	void OnStageCleared();
};
