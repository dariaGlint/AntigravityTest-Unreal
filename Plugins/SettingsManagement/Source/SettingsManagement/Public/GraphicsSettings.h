#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphicsSettings.generated.h"

/**
 * グラフィック品質プリセット
 * Graphics quality preset
 */
UENUM(BlueprintType)
enum class EGraphicsQualityPreset : uint8
{
	Low UMETA(DisplayName = "Low"),
	Medium UMETA(DisplayName = "Medium"),
	High UMETA(DisplayName = "High"),
	Ultra UMETA(DisplayName = "Ultra"),
	Custom UMETA(DisplayName = "Custom")
};

/**
 * グラフィック設定を管理するクラス
 * Graphics settings management class
 */
UCLASS(BlueprintType)
class SETTINGSMANAGEMENT_API UGraphicsSettings : public UObject
{
	GENERATED_BODY()

public:
	UGraphicsSettings();

	// 品質プリセット
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
	EGraphicsQualityPreset QualityPreset;

	// 解像度スケール (0.5 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float ResolutionScale;

	// 影の品質 (0-3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "3"))
	int32 ShadowQuality;

	// アンチエイリアス品質 (0-3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "3"))
	int32 AntiAliasingQuality;

	// テクスチャ品質 (0-3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "3"))
	int32 TextureQuality;

	// エフェクト品質 (0-3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "3"))
	int32 EffectsQuality;

	// ポストプロセス品質 (0-3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "3"))
	int32 PostProcessQuality;

	// 視野距離品質 (0-3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "3"))
	int32 ViewDistanceQuality;

	// 垂直同期
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
	bool bVSync;

	// フレームレート制限 (0 = 無制限)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings", meta = (ClampMin = "0", ClampMax = "300"))
	int32 FrameRateLimit;

	// フルスクリーンモード
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
	bool bFullscreen;

	/**
	 * デフォルト値に復元
	 * Reset to default values
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics Settings")
	void ResetToDefault();

	/**
	 * プリセットを適用
	 * Apply preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics Settings")
	void ApplyPreset(EGraphicsQualityPreset Preset);

	/**
	 * 設定を適用
	 * Apply settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics Settings")
	void ApplySettings();

	/**
	 * 設定のコピーを作成
	 * Create a copy of settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics Settings")
	UGraphicsSettings* CreateCopy() const;
};
