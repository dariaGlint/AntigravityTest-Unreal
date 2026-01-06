// Copyright Epic Games, Inc. All Rights Reserved.

#include "AchievementManager.h"
#include "Kismet/GameplayStatics.h"
#include "AchievementSaveGame.h"

UAchievementManager::UAchievementManager()
{
}

void UAchievementManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("AchievementManager initialized"));
}

void UAchievementManager::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("AchievementManager deinitialized"));
}

void UAchievementManager::RegisterAchievement(const FAchievementDefinition& Definition)
{
	if (!Definition.AchievementId.IsNone())
	{
		AchievementDefinitions.Add(Definition.AchievementId, Definition);

		// 進捗データが存在しない場合は初期化
		if (!ProgressData.Contains(Definition.AchievementId))
		{
			FAchievementProgress NewProgress;
			NewProgress.AchievementId = Definition.AchievementId;
			ProgressData.Add(Definition.AchievementId, NewProgress);
		}

		UE_LOG(LogTemp, Log, TEXT("Achievement registered: %s"), *Definition.AchievementId.ToString());
	}
}

void UAchievementManager::LoadAchievementsFromDataTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("DataTable is null"));
		return;
	}

	TArray<FAchievementDefinition*> Rows;
	DataTable->GetAllRows<FAchievementDefinition>(TEXT("LoadAchievementsFromDataTable"), Rows);

	for (FAchievementDefinition* Row : Rows)
	{
		if (Row)
		{
			RegisterAchievement(*Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded %d achievements from DataTable"), Rows.Num());
}

void UAchievementManager::UpdateProgress(FName AchievementId, int32 Progress)
{
	if (!AchievementDefinitions.Contains(AchievementId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Achievement not found: %s"), *AchievementId.ToString());
		return;
	}

	FAchievementProgress* ProgressPtr = ProgressData.Find(AchievementId);
	if (!ProgressPtr)
	{
		return;
	}

	// 既に解除済みの場合は更新しない
	if (ProgressPtr->bUnlocked)
	{
		return;
	}

	// 進捗を更新
	ProgressPtr->CurrentProgress += Progress;

	const FAchievementDefinition& Definition = AchievementDefinitions[AchievementId];

	// 進捗パーセンテージを計算
	ProgressPtr->ProgressPercentage = CalculateProgressPercentage(AchievementId);

	// 進捗更新イベントを発火
	OnAchievementProgressUpdated.Broadcast(AchievementId, ProgressPtr->ProgressPercentage);

	// 達成条件を満たしたか確認
	if (ProgressPtr->CurrentProgress >= Definition.RequiredProgress)
	{
		ProcessUnlock(AchievementId);
	}

	UE_LOG(LogTemp, Log, TEXT("Achievement progress updated: %s (%d/%d)"),
		*AchievementId.ToString(),
		ProgressPtr->CurrentProgress,
		Definition.RequiredProgress);
}

void UAchievementManager::SetProgress(FName AchievementId, int32 Progress)
{
	if (!AchievementDefinitions.Contains(AchievementId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Achievement not found: %s"), *AchievementId.ToString());
		return;
	}

	FAchievementProgress* ProgressPtr = ProgressData.Find(AchievementId);
	if (!ProgressPtr)
	{
		return;
	}

	// 既に解除済みの場合は更新しない
	if (ProgressPtr->bUnlocked)
	{
		return;
	}

	// 進捗を設定
	ProgressPtr->CurrentProgress = Progress;

	const FAchievementDefinition& Definition = AchievementDefinitions[AchievementId];

	// 進捗パーセンテージを計算
	ProgressPtr->ProgressPercentage = CalculateProgressPercentage(AchievementId);

	// 進捗更新イベントを発火
	OnAchievementProgressUpdated.Broadcast(AchievementId, ProgressPtr->ProgressPercentage);

	// 達成条件を満たしたか確認
	if (ProgressPtr->CurrentProgress >= Definition.RequiredProgress)
	{
		ProcessUnlock(AchievementId);
	}
}

void UAchievementManager::UnlockAchievement(FName AchievementId)
{
	if (!AchievementDefinitions.Contains(AchievementId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Achievement not found: %s"), *AchievementId.ToString());
		return;
	}

	FAchievementProgress* ProgressPtr = ProgressData.Find(AchievementId);
	if (!ProgressPtr || ProgressPtr->bUnlocked)
	{
		return;
	}

	ProcessUnlock(AchievementId);
}

FAchievementProgress UAchievementManager::GetProgress(FName AchievementId) const
{
	if (const FAchievementProgress* ProgressPtr = ProgressData.Find(AchievementId))
	{
		return *ProgressPtr;
	}

	return FAchievementProgress();
}

bool UAchievementManager::IsAchievementUnlocked(FName AchievementId) const
{
	if (const FAchievementProgress* ProgressPtr = ProgressData.Find(AchievementId))
	{
		return ProgressPtr->bUnlocked;
	}

	return false;
}

TArray<FAchievementDefinition> UAchievementManager::GetAllAchievementDefinitions() const
{
	TArray<FAchievementDefinition> Definitions;
	AchievementDefinitions.GenerateValueArray(Definitions);
	return Definitions;
}

TArray<FAchievementDefinition> UAchievementManager::GetAchievementsByCategory(EAchievementCategory Category) const
{
	TArray<FAchievementDefinition> Definitions;

	for (const auto& Pair : AchievementDefinitions)
	{
		if (Pair.Value.Category == Category)
		{
			Definitions.Add(Pair.Value);
		}
	}

	return Definitions;
}

FAchievementStatistics UAchievementManager::GetStatistics() const
{
	FAchievementStatistics Stats;

	Stats.TotalAchievements = AchievementDefinitions.Num();

	for (const auto& Pair : ProgressData)
	{
		const FName& AchievementId = Pair.Key;
		const FAchievementProgress& Progress = Pair.Value;

		if (Progress.bUnlocked)
		{
			Stats.UnlockedAchievements++;

			// ポイント加算
			if (const FAchievementDefinition* Definition = AchievementDefinitions.Find(AchievementId))
			{
				Stats.TotalPoints += Definition->RewardPoints;

				// カテゴリ別カウント
				int32& CategoryCount = Stats.CategoryUnlockCounts.FindOrAdd(Definition->Category);
				CategoryCount++;
			}
		}
	}

	// 完了率計算
	if (Stats.TotalAchievements > 0)
	{
		Stats.CompletionRate = static_cast<float>(Stats.UnlockedAchievements) / static_cast<float>(Stats.TotalAchievements);
	}

	return Stats;
}

void UAchievementManager::ResetAllProgress()
{
	for (auto& Pair : ProgressData)
	{
		Pair.Value.CurrentProgress = 0;
		Pair.Value.bUnlocked = false;
		Pair.Value.UnlockTime = FDateTime(0);
		Pair.Value.ProgressPercentage = 0.0f;
	}

	UE_LOG(LogTemp, Log, TEXT("All achievement progress reset"));
}

bool UAchievementManager::SaveProgress(const FString& SlotName)
{
	UAchievementSaveGame* SaveGameInstance = Cast<UAchievementSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UAchievementSaveGame::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create save game object"));
		return false;
	}

	// 進捗データをセーブオブジェクトにコピー
	SaveGameInstance->ProgressData = ProgressData;

	// セーブ実行
	bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Achievement progress saved to slot: %s"), *SlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save achievement progress"));
	}

	return bSuccess;
}

bool UAchievementManager::LoadProgress(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save game does not exist: %s"), *SlotName);
		return false;
	}

	UAchievementSaveGame* LoadedGame = Cast<UAchievementSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (!LoadedGame)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
		return false;
	}

	// 進捗データをロード
	ProgressData = LoadedGame->ProgressData;

	UE_LOG(LogTemp, Log, TEXT("Achievement progress loaded from slot: %s"), *SlotName);
	return true;
}

void UAchievementManager::UpdateStatistics()
{
	// 統計データは GetStatistics() で動的に計算されるため、
	// ここでは特に処理を行わない
}

void UAchievementManager::ProcessUnlock(FName AchievementId)
{
	FAchievementProgress* ProgressPtr = ProgressData.Find(AchievementId);
	if (!ProgressPtr || ProgressPtr->bUnlocked)
	{
		return;
	}

	// 解除処理
	ProgressPtr->bUnlocked = true;
	ProgressPtr->UnlockTime = FDateTime::Now();
	ProgressPtr->ProgressPercentage = 1.0f;

	// 解除イベントを発火
	OnAchievementUnlocked.Broadcast(AchievementId);

	UE_LOG(LogTemp, Log, TEXT("Achievement unlocked: %s"), *AchievementId.ToString());
}

float UAchievementManager::CalculateProgressPercentage(FName AchievementId) const
{
	const FAchievementDefinition* Definition = AchievementDefinitions.Find(AchievementId);
	const FAchievementProgress* Progress = ProgressData.Find(AchievementId);

	if (!Definition || !Progress || Definition->RequiredProgress <= 0)
	{
		return 0.0f;
	}

	float Percentage = static_cast<float>(Progress->CurrentProgress) / static_cast<float>(Definition->RequiredProgress);
	return FMath::Clamp(Percentage, 0.0f, 1.0f);
}
