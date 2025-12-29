// Copyright Epic Games, Inc. All Rights Reserved.

#include "AchievementSystem.h"

#define LOCTEXT_NAMESPACE "FAchievementSystemModule"

void FAchievementSystemModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("AchievementSystem module started"));
}

void FAchievementSystemModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("AchievementSystem module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAchievementSystemModule, AchievementSystem)
