#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioSettings.generated.h"

/**
 * オーディオ設定を管理するクラス
 * Audio settings management class
 */
UCLASS(BlueprintType)
class SETTINGSMANAGEMENT_API UAudioSettings : public UObject
{
	GENERATED_BODY()

public:
	UAudioSettings();

	// マスターボリューム (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float MasterVolume;

	// BGMボリューム (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float MusicVolume;

	// SEボリューム (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float SFXVolume;

	// ボイスボリューム (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float VoiceVolume;

	// 環境音ボリューム (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float AmbientVolume;

	// ミュート設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	bool bMuted;

	/**
	 * デフォルト値に復元
	 * Reset to default values
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio Settings")
	void ResetToDefault();

	/**
	 * 設定を適用
	 * Apply settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio Settings")
	void ApplySettings();

	/**
	 * 設定のコピーを作成
	 * Create a copy of settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio Settings")
	UAudioSettings* CreateCopy() const;
};
