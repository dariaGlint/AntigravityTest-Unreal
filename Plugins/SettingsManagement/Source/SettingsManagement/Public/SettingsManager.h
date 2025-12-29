#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSettings.h"
#include "GraphicsSettings.h"
#include "ControlSettings.h"
#include "SettingsPreset.h"
#include "SettingsManager.generated.h"

/**
 * 設定管理サブシステム
 * Settings management subsystem
 */
UCLASS()
class SETTINGSMANAGEMENT_API USettingsManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USettingsManager();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 現在のオーディオ設定
	UPROPERTY(BlueprintReadOnly, Category = "Settings Manager")
	UAudioSettings* CurrentAudioSettings;

	// 現在のグラフィック設定
	UPROPERTY(BlueprintReadOnly, Category = "Settings Manager")
	UGraphicsSettings* CurrentGraphicsSettings;

	// 現在のコントロール設定
	UPROPERTY(BlueprintReadOnly, Category = "Settings Manager")
	UControlSettings* CurrentControlSettings;

	// 保存されたプリセット
	UPROPERTY(BlueprintReadOnly, Category = "Settings Manager")
	TArray<USettingsPreset*> SavedPresets;

	/**
	 * オーディオ設定を取得
	 * Get audio settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	UAudioSettings* GetAudioSettings() const { return CurrentAudioSettings; }

	/**
	 * グラフィック設定を取得
	 * Get graphics settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	UGraphicsSettings* GetGraphicsSettings() const { return CurrentGraphicsSettings; }

	/**
	 * コントロール設定を取得
	 * Get control settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	UControlSettings* GetControlSettings() const { return CurrentControlSettings; }

	/**
	 * すべての設定を保存
	 * Save all settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	bool SaveSettings();

	/**
	 * すべての設定を読み込み
	 * Load all settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	bool LoadSettings();

	/**
	 * すべての設定をデフォルトに復元
	 * Reset all settings to default
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	void ResetAllToDefault();

	/**
	 * すべての設定を適用
	 * Apply all settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	void ApplyAllSettings();

	/**
	 * プリセットを作成
	 * Create preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	USettingsPreset* CreatePreset(const FString& PresetName, const FString& Description);

	/**
	 * プリセットを保存
	 * Save preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	bool SavePreset(USettingsPreset* Preset);

	/**
	 * プリセットを読み込み
	 * Load preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	bool LoadPreset(const FString& PresetName);

	/**
	 * プリセットを削除
	 * Delete preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	bool DeletePreset(const FString& PresetName);

	/**
	 * すべてのプリセットを取得
	 * Get all presets
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	TArray<USettingsPreset*> GetAllPresets() const { return SavedPresets; }

	/**
	 * プリセットを適用
	 * Apply preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings Manager")
	void ApplyPreset(USettingsPreset* Preset);

private:
	FString GetSettingsSaveSlotName() const;
	FString GetPresetSaveSlotName(const FString& PresetName) const;

	void InitializeSettings();
};
