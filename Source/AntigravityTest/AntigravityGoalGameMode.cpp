// Fill out your copyright notice in the Description page of Project Settings.


#include "AntigravityGoalGameMode.h"

void AAntigravityGoalGameMode::CompleteStage()
{
	// ログ出力
	UE_LOG(LogTemp, Log, TEXT("Stage Cleared!"));

	// Blueprintイベントの発火
	OnStageCleared();
}
