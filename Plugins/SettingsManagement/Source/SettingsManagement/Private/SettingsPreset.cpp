#include "SettingsPreset.h"

USettingsPreset::USettingsPreset()
{
	PresetName = TEXT("New Preset");
	Description = TEXT("");
	AudioSettings = nullptr;
	GraphicsSettings = nullptr;
	ControlSettings = nullptr;
}

void USettingsPreset::CreateFromCurrentSettings(UAudioSettings* InAudioSettings, UGraphicsSettings* InGraphicsSettings, UControlSettings* InControlSettings)
{
	if (InAudioSettings)
	{
		AudioSettings = InAudioSettings->CreateCopy();
	}

	if (InGraphicsSettings)
	{
		GraphicsSettings = InGraphicsSettings->CreateCopy();
	}

	if (InControlSettings)
	{
		ControlSettings = InControlSettings->CreateCopy();
	}
}

void USettingsPreset::ApplyPreset()
{
	if (AudioSettings)
	{
		AudioSettings->ApplySettings();
	}

	if (GraphicsSettings)
	{
		GraphicsSettings->ApplySettings();
	}

	if (ControlSettings)
	{
		ControlSettings->ApplySettings();
	}
}

USettingsPreset* USettingsPreset::CreateCopy() const
{
	USettingsPreset* Copy = NewObject<USettingsPreset>();
	Copy->PresetName = PresetName;
	Copy->Description = Description;

	if (AudioSettings)
	{
		Copy->AudioSettings = AudioSettings->CreateCopy();
	}

	if (GraphicsSettings)
	{
		Copy->GraphicsSettings = GraphicsSettings->CreateCopy();
	}

	if (ControlSettings)
	{
		Copy->ControlSettings = ControlSettings->CreateCopy();
	}

	return Copy;
}
