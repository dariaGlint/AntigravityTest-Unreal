#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AudioSettings.h"
#include "GraphicsSettings.h"
#include "ControlSettings.h"
#include "SettingsSaveGame.generated.h"

/**
 * 設定データを保存するためのセーブゲームクラス
 * Save game class for storing settings data
 */
UCLASS()
class SETTINGSMANAGEMENT_API USettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USettingsSaveGame();

	// オーディオ設定
	UPROPERTY(VisibleAnywhere, Category = "Settings Save Game")
	UAudioSettings* AudioSettings;

	// グラフィック設定
	UPROPERTY(VisibleAnywhere, Category = "Settings Save Game")
	UGraphicsSettings* GraphicsSettings;

	// コントロール設定
	UPROPERTY(VisibleAnywhere, Category = "Settings Save Game")
	UControlSettings* ControlSettings;

	// セーブバージョン
	UPROPERTY(VisibleAnywhere, Category = "Settings Save Game")
	int32 SaveVersion;
};
