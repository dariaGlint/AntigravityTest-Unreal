#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioSettings.h"
#include "GraphicsSettings.h"
#include "ControlSettings.h"
#include "SettingsPreset.generated.h"

/**
 * 設定プリセット
 * Settings preset
 */
UCLASS(BlueprintType)
class SETTINGSMANAGEMENT_API USettingsPreset : public UObject
{
	GENERATED_BODY()

public:
	USettingsPreset();

	// プリセット名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Preset")
	FString PresetName;

	// プリセットの説明
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Preset")
	FString Description;

	// オーディオ設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Preset")
	UAudioSettings* AudioSettings;

	// グラフィック設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Preset")
	UGraphicsSettings* GraphicsSettings;

	// コントロール設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Preset")
	UControlSettings* ControlSettings;

	/**
	 * プリセットを現在の設定から作成
	 * Create preset from current settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Preset")
	void CreateFromCurrentSettings(UAudioSettings* InAudioSettings, UGraphicsSettings* InGraphicsSettings, UControlSettings* InControlSettings);

	/**
	 * プリセットを適用
	 * Apply preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Preset")
	void ApplyPreset();

	/**
	 * プリセットのコピーを作成
	 * Create a copy of preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Preset")
	USettingsPreset* CreateCopy() const;
};
