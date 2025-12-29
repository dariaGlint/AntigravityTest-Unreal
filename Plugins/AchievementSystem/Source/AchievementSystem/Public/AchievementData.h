// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "AchievementData.generated.h"

/**
 * アチーブメントの種類
 */
UENUM(BlueprintType)
enum class EAchievementType : uint8
{
	// 単純な達成型（一度達成すれば解除）
	Simple UMETA(DisplayName = "Simple"),

	// 進捗型（進捗を蓄積して達成）
	Progressive UMETA(DisplayName = "Progressive"),

	// カウント型（複数回の達成が必要）
	Count UMETA(DisplayName = "Count"),

	// 隠し（達成条件が隠されている）
	Hidden UMETA(DisplayName = "Hidden")
};

/**
 * アチーブメントのカテゴリ
 */
UENUM(BlueprintType)
enum class EAchievementCategory : uint8
{
	General UMETA(DisplayName = "General"),
	Combat UMETA(DisplayName = "Combat"),
	Exploration UMETA(DisplayName = "Exploration"),
	Collection UMETA(DisplayName = "Collection"),
	Social UMETA(DisplayName = "Social"),
	Secret UMETA(DisplayName = "Secret")
};

/**
 * アチーブメントの定義データ
 */
USTRUCT(BlueprintType)
struct FAchievementDefinition : public FTableRowBase
{
	GENERATED_BODY()

	// アチーブメントID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FName AchievementId;

	// 表示名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FText DisplayName;

	// 説明
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FText Description;

	// アチーブメントの種類
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	EAchievementType Type;

	// カテゴリ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	EAchievementCategory Category;

	// 達成に必要な進捗値
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int32 RequiredProgress;

	// 隠しアチーブメントかどうか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	bool bIsHidden;

	// アイコンのパス
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FSoftObjectPath IconPath;

	// 報酬ポイント
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int32 RewardPoints;

	FAchievementDefinition()
		: Type(EAchievementType::Simple)
		, Category(EAchievementCategory::General)
		, RequiredProgress(1)
		, bIsHidden(false)
		, RewardPoints(0)
	{
	}
};

/**
 * アチーブメントの進捗データ
 */
USTRUCT(BlueprintType)
struct FAchievementProgress
{
	GENERATED_BODY()

	// アチーブメントID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FName AchievementId;

	// 現在の進捗値
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int32 CurrentProgress;

	// 解除済みかどうか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	bool bUnlocked;

	// 解除日時
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FDateTime UnlockTime;

	// 進捗パーセンテージ（0.0 - 1.0）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	float ProgressPercentage;

	FAchievementProgress()
		: CurrentProgress(0)
		, bUnlocked(false)
		, UnlockTime(0)
		, ProgressPercentage(0.0f)
	{
	}
};

/**
 * アチーブメント統計データ
 */
USTRUCT(BlueprintType)
struct FAchievementStatistics
{
	GENERATED_BODY()

	// 総アチーブメント数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int32 TotalAchievements;

	// 解除済みアチーブメント数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int32 UnlockedAchievements;

	// 総獲得ポイント
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int32 TotalPoints;

	// 完了率（0.0 - 1.0）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	float CompletionRate;

	// カテゴリ別の解除数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	TMap<EAchievementCategory, int32> CategoryUnlockCounts;

	FAchievementStatistics()
		: TotalAchievements(0)
		, UnlockedAchievements(0)
		, TotalPoints(0)
		, CompletionRate(0.0f)
	{
	}
};
