#include "AudioSettings.h"
#include "Kismet/GameplayStatics.h"

UAudioSettings::UAudioSettings()
{
	ResetToDefault();
}

void UAudioSettings::ResetToDefault()
{
	MasterVolume = 1.0f;
	MusicVolume = 0.8f;
	SFXVolume = 0.8f;
	VoiceVolume = 1.0f;
	AmbientVolume = 0.6f;
	bMuted = false;
}

void UAudioSettings::ApplySettings()
{
	// ここで実際のオーディオシステムに設定を適用
	// Apply settings to the actual audio system
	// Note: 実装はプロジェクトのオーディオシステムに依存します
	// Implementation depends on the project's audio system
}

UAudioSettings* UAudioSettings::CreateCopy() const
{
	UAudioSettings* Copy = NewObject<UAudioSettings>();
	Copy->MasterVolume = MasterVolume;
	Copy->MusicVolume = MusicVolume;
	Copy->SFXVolume = SFXVolume;
	Copy->VoiceVolume = VoiceVolume;
	Copy->AmbientVolume = AmbientVolume;
	Copy->bMuted = bMuted;
	return Copy;
}
