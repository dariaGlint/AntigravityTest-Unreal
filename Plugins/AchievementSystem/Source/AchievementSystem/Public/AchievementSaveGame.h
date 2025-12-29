// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AchievementData.h"
#include "AchievementSaveGame.generated.h"

/**
 * アチーブメントの進捗データを保存するためのSaveGameクラス
 */
UCLASS()
class ACHIEVEMENTSYSTEM_API UAchievementSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UAchievementSaveGame();

	// 進捗データマップ（ID -> Progress）
	UPROPERTY(VisibleAnywhere, Category = "Achievement")
	TMap<FName, FAchievementProgress> ProgressData;
};
