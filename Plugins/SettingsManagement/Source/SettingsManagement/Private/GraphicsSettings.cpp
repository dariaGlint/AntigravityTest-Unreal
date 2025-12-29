#include "GraphicsSettings.h"
#include "GameFramework/GameUserSettings.h"

UGraphicsSettings::UGraphicsSettings()
{
	ResetToDefault();
}

void UGraphicsSettings::ResetToDefault()
{
	QualityPreset = EGraphicsQualityPreset::High;
	ResolutionScale = 1.0f;
	ShadowQuality = 3;
	AntiAliasingQuality = 3;
	TextureQuality = 3;
	EffectsQuality = 3;
	PostProcessQuality = 3;
	ViewDistanceQuality = 3;
	bVSync = true;
	FrameRateLimit = 60;
	bFullscreen = true;
}

void UGraphicsSettings::ApplyPreset(EGraphicsQualityPreset Preset)
{
	QualityPreset = Preset;

	switch (Preset)
	{
	case EGraphicsQualityPreset::Low:
		ResolutionScale = 0.7f;
		ShadowQuality = 0;
		AntiAliasingQuality = 0;
		TextureQuality = 0;
		EffectsQuality = 0;
		PostProcessQuality = 0;
		ViewDistanceQuality = 0;
		break;

	case EGraphicsQualityPreset::Medium:
		ResolutionScale = 0.85f;
		ShadowQuality = 1;
		AntiAliasingQuality = 1;
		TextureQuality = 1;
		EffectsQuality = 1;
		PostProcessQuality = 1;
		ViewDistanceQuality = 1;
		break;

	case EGraphicsQualityPreset::High:
		ResolutionScale = 1.0f;
		ShadowQuality = 2;
		AntiAliasingQuality = 2;
		TextureQuality = 2;
		EffectsQuality = 2;
		PostProcessQuality = 2;
		ViewDistanceQuality = 2;
		break;

	case EGraphicsQualityPreset::Ultra:
		ResolutionScale = 1.0f;
		ShadowQuality = 3;
		AntiAliasingQuality = 3;
		TextureQuality = 3;
		EffectsQuality = 3;
		PostProcessQuality = 3;
		ViewDistanceQuality = 3;
		break;

	case EGraphicsQualityPreset::Custom:
		// カスタム設定は変更しない
		break;
	}
}

void UGraphicsSettings::ApplySettings()
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	// 品質設定を適用
	UserSettings->SetShadowQuality(ShadowQuality);
	UserSettings->SetAntiAliasingQuality(AntiAliasingQuality);
	UserSettings->SetTextureQuality(TextureQuality);
	UserSettings->SetVisualEffectQuality(EffectsQuality);
	UserSettings->SetPostProcessingQuality(PostProcessQuality);
	UserSettings->SetViewDistanceQuality(ViewDistanceQuality);

	// 解像度スケールを適用
	UserSettings->SetResolutionScaleNormalized(ResolutionScale);

	// VSync設定
	UserSettings->SetVSyncEnabled(bVSync);

	// フレームレート制限
	if (FrameRateLimit > 0)
	{
		UserSettings->SetFrameRateLimit(static_cast<float>(FrameRateLimit));
	}
	else
	{
		UserSettings->SetFrameRateLimit(0.0f);
	}

	// フルスクリーン設定
	UserSettings->SetFullscreenMode(bFullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed);

	// 設定を適用して保存
	UserSettings->ApplySettings(false);
}

UGraphicsSettings* UGraphicsSettings::CreateCopy() const
{
	UGraphicsSettings* Copy = NewObject<UGraphicsSettings>();
	Copy->QualityPreset = QualityPreset;
	Copy->ResolutionScale = ResolutionScale;
	Copy->ShadowQuality = ShadowQuality;
	Copy->AntiAliasingQuality = AntiAliasingQuality;
	Copy->TextureQuality = TextureQuality;
	Copy->EffectsQuality = EffectsQuality;
	Copy->PostProcessQuality = PostProcessQuality;
	Copy->ViewDistanceQuality = ViewDistanceQuality;
	Copy->bVSync = bVSync;
	Copy->FrameRateLimit = FrameRateLimit;
	Copy->bFullscreen = bFullscreen;
	return Copy;
}
