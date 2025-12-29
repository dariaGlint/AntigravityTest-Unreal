// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AchievementData.h"
#include "AchievementManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAchievementUnlocked, FName, AchievementId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAchievementProgressUpdated, FName, AchievementId, float, ProgressPercentage);

/**
 * アチーブメント管理システム
 * ゲーム全体のアチーブメントを管理するGameInstanceSubsystem
 */
UCLASS()
class ACHIEVEMENTSYSTEM_API UAchievementManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UAchievementManager();

	// Subsystem初期化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * アチーブメント定義を登録
	 * @param Definition アチーブメント定義データ
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void RegisterAchievement(const FAchievementDefinition& Definition);

	/**
	 * データテーブルからアチーブメント定義をロード
	 * @param DataTable アチーブメント定義のデータテーブル
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void LoadAchievementsFromDataTable(UDataTable* DataTable);

	/**
	 * アチーブメントの進捗を更新
	 * @param AchievementId アチーブメントID
	 * @param Progress 追加する進捗値
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void UpdateProgress(FName AchievementId, int32 Progress = 1);

	/**
	 * アチーブメントの進捗を設定
	 * @param AchievementId アチーブメントID
	 * @param Progress 設定する進捗値
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void SetProgress(FName AchievementId, int32 Progress);

	/**
	 * アチーブメントを強制的に解除
	 * @param AchievementId アチーブメントID
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void UnlockAchievement(FName AchievementId);

	/**
	 * アチーブメントの進捗を取得
	 * @param AchievementId アチーブメントID
	 * @return 進捗データ
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	FAchievementProgress GetProgress(FName AchievementId) const;

	/**
	 * アチーブメントが解除済みかチェック
	 * @param AchievementId アチーブメントID
	 * @return 解除済みならtrue
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	bool IsAchievementUnlocked(FName AchievementId) const;

	/**
	 * すべてのアチーブメント定義を取得
	 * @return アチーブメント定義の配列
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	TArray<FAchievementDefinition> GetAllAchievementDefinitions() const;

	/**
	 * カテゴリ別のアチーブメント定義を取得
	 * @param Category カテゴリ
	 * @return アチーブメント定義の配列
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	TArray<FAchievementDefinition> GetAchievementsByCategory(EAchievementCategory Category) const;

	/**
	 * 統計データを取得
	 * @return 統計データ
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	FAchievementStatistics GetStatistics() const;

	/**
	 * すべての進捗データをリセット
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void ResetAllProgress();

	/**
	 * 進捗データをセーブ
	 * @param SlotName セーブスロット名
	 * @return セーブに成功したらtrue
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	bool SaveProgress(const FString& SlotName = TEXT("AchievementSave"));

	/**
	 * 進捗データをロード
	 * @param SlotName セーブスロット名
	 * @return ロードに成功したらtrue
	 */
	UFUNCTION(BlueprintCallable, Category = "Achievement")
	bool LoadProgress(const FString& SlotName = TEXT("AchievementSave"));

	// イベントデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Achievement")
	FOnAchievementUnlocked OnAchievementUnlocked;

	UPROPERTY(BlueprintAssignable, Category = "Achievement")
	FOnAchievementProgressUpdated OnAchievementProgressUpdated;

private:
	// アチーブメント定義マップ（ID -> Definition）
	UPROPERTY()
	TMap<FName, FAchievementDefinition> AchievementDefinitions;

	// 進捗データマップ（ID -> Progress）
	UPROPERTY()
	TMap<FName, FAchievementProgress> ProgressData;

	// 統計データ更新
	void UpdateStatistics();

	// アチーブメント解除処理
	void ProcessUnlock(FName AchievementId);

	// 進捗パーセンテージを計算
	float CalculateProgressPercentage(FName AchievementId) const;
};
