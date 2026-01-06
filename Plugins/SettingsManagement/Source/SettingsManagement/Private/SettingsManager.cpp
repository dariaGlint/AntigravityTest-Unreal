#include "SettingsManager.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SettingsSaveGame.h"

USettingsManager::USettingsManager()
{
	CurrentAudioSettings = nullptr;
	CurrentGraphicsSettings = nullptr;
	CurrentControlSettings = nullptr;
}

void USettingsManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeSettings();

	// 保存された設定を読み込み、失敗したらデフォルト値を使用
	if (!LoadSettings())
	{
		ResetAllToDefault();
		ApplyAllSettings();
	}
}

void USettingsManager::Deinitialize()
{
	Super::Deinitialize();
}

void USettingsManager::InitializeSettings()
{
	CurrentAudioSettings = NewObject<UAudioSettings>(this);
	CurrentGraphicsSettings = NewObject<UGraphicsSettings>(this);
	CurrentControlSettings = NewObject<UControlSettings>(this);
}

bool USettingsManager::SaveSettings()
{
	USettingsSaveGame* SaveGameInstance = Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
	if (!SaveGameInstance)
	{
		return false;
	}

	// 設定をセーブゲームオブジェクトにコピー
	if (CurrentAudioSettings)
	{
		SaveGameInstance->AudioSettings = CurrentAudioSettings->CreateCopy();
	}

	if (CurrentGraphicsSettings)
	{
		SaveGameInstance->GraphicsSettings = CurrentGraphicsSettings->CreateCopy();
	}

	if (CurrentControlSettings)
	{
		SaveGameInstance->ControlSettings = CurrentControlSettings->CreateCopy();
	}

	// ファイルに保存
	return UGameplayStatics::SaveGameToSlot(SaveGameInstance, GetSettingsSaveSlotName(), 0);
}

bool USettingsManager::LoadSettings()
{
	USettingsSaveGame* LoadedGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(GetSettingsSaveSlotName(), 0));
	if (!LoadedGame)
	{
		return false;
	}

	// ロードした設定を現在の設定にコピー
	if (LoadedGame->AudioSettings && CurrentAudioSettings)
	{
		CurrentAudioSettings = LoadedGame->AudioSettings->CreateCopy();
	}

	if (LoadedGame->GraphicsSettings && CurrentGraphicsSettings)
	{
		CurrentGraphicsSettings = LoadedGame->GraphicsSettings->CreateCopy();
	}

	if (LoadedGame->ControlSettings && CurrentControlSettings)
	{
		CurrentControlSettings = LoadedGame->ControlSettings->CreateCopy();
	}

	ApplyAllSettings();
	return true;
}

void USettingsManager::ResetAllToDefault()
{
	if (CurrentAudioSettings)
	{
		CurrentAudioSettings->ResetToDefault();
	}

	if (CurrentGraphicsSettings)
	{
		CurrentGraphicsSettings->ResetToDefault();
	}

	if (CurrentControlSettings)
	{
		CurrentControlSettings->ResetToDefault();
	}
}

void USettingsManager::ApplyAllSettings()
{
	if (CurrentAudioSettings)
	{
		CurrentAudioSettings->ApplySettings();
	}

	if (CurrentGraphicsSettings)
	{
		CurrentGraphicsSettings->ApplySettings();
	}

	if (CurrentControlSettings)
	{
		CurrentControlSettings->ApplySettings();
	}
}

USettingsPreset* USettingsManager::CreatePreset(const FString& PresetName, const FString& Description)
{
	USettingsPreset* NewPreset = NewObject<USettingsPreset>(this);
	NewPreset->PresetName = PresetName;
	NewPreset->Description = Description;
	NewPreset->CreateFromCurrentSettings(CurrentAudioSettings, CurrentGraphicsSettings, CurrentControlSettings);

	SavedPresets.Add(NewPreset);
	return NewPreset;
}

bool USettingsManager::SavePreset(USettingsPreset* Preset)
{
	if (!Preset)
	{
		return false;
	}

	USettingsSaveGame* SaveGameInstance = Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
	if (!SaveGameInstance)
	{
		return false;
	}

	// プリセットをセーブゲームオブジェクトにコピー
	SaveGameInstance->AudioSettings = Preset->AudioSettings;
	SaveGameInstance->GraphicsSettings = Preset->GraphicsSettings;
	SaveGameInstance->ControlSettings = Preset->ControlSettings;

	// ファイルに保存
	return UGameplayStatics::SaveGameToSlot(SaveGameInstance, GetPresetSaveSlotName(Preset->PresetName), 0);
}

bool USettingsManager::LoadPreset(const FString& PresetName)
{
	USettingsSaveGame* LoadedGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(GetPresetSaveSlotName(PresetName), 0));
	if (!LoadedGame)
	{
		return false;
	}

	USettingsPreset* LoadedPreset = NewObject<USettingsPreset>(this);
	LoadedPreset->PresetName = PresetName;
	LoadedPreset->AudioSettings = LoadedGame->AudioSettings;
	LoadedPreset->GraphicsSettings = LoadedGame->GraphicsSettings;
	LoadedPreset->ControlSettings = LoadedGame->ControlSettings;

	ApplyPreset(LoadedPreset);
	SavedPresets.Add(LoadedPreset);

	return true;
}

bool USettingsManager::DeletePreset(const FString& PresetName)
{
	// プリセットリストから削除
	for (int32 i = 0; i < SavedPresets.Num(); i++)
	{
		if (SavedPresets[i] && SavedPresets[i]->PresetName == PresetName)
		{
			SavedPresets.RemoveAt(i);
			break;
		}
	}

	// セーブファイルを削除
	return UGameplayStatics::DeleteGameInSlot(GetPresetSaveSlotName(PresetName), 0);
}

void USettingsManager::ApplyPreset(USettingsPreset* Preset)
{
	if (!Preset)
	{
		return;
	}

	if (Preset->AudioSettings)
	{
		CurrentAudioSettings = Preset->AudioSettings->CreateCopy();
	}

	if (Preset->GraphicsSettings)
	{
		CurrentGraphicsSettings = Preset->GraphicsSettings->CreateCopy();
	}

	if (Preset->ControlSettings)
	{
		CurrentControlSettings = Preset->ControlSettings->CreateCopy();
	}

	ApplyAllSettings();
}

FString USettingsManager::GetSettingsSaveSlotName() const
{
	return TEXT("GameSettings");
}

FString USettingsManager::GetPresetSaveSlotName(const FString& PresetName) const
{
	return FString::Printf(TEXT("SettingsPreset_%s"), *PresetName);
}
