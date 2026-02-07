// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NotificationData.generated.h"

/**
 * 通知タイプの列挙型
 */
UENUM(BlueprintType)
enum class ENotificationType : uint8
{
	Info UMETA(DisplayName = "情報"),
	Success UMETA(DisplayName = "成功"),
	Warning UMETA(DisplayName = "警告"),
	Error UMETA(DisplayName = "エラー"),
	Achievement UMETA(DisplayName = "実績"),
	Custom UMETA(DisplayName = "カスタム")
};

/**
 * 通知優先度の列挙型
 */
UENUM(BlueprintType)
enum class ENotificationPriority : uint8
{
	Low UMETA(DisplayName = "低"),
	Normal UMETA(DisplayName = "通常"),
	High UMETA(DisplayName = "高"),
	Critical UMETA(DisplayName = "緊急")
};

/**
 * 通知データ構造体
 */
USTRUCT(BlueprintType)
struct NOTIFICATIONSYSTEM_API FNotificationData
{
	GENERATED_BODY()

public:
	/** 通知のタイトル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	FText Title;

	/** 通知の本文 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	FText Message;

	/** 通知タイプ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	ENotificationType Type = ENotificationType::Info;

	/** 通知優先度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	ENotificationPriority Priority = ENotificationPriority::Normal;

	/** 表示時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	float DisplayDuration = 3.0f;

	/** フェードイン時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	float FadeInDuration = 0.3f;

	/** フェードアウト時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	float FadeOutDuration = 0.3f;

	/** アイコンテクスチャ（オプション） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	UTexture2D* IconTexture = nullptr;

	/** カスタムデータ（Blueprint/C++で拡張可能） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	TMap<FString, FString> CustomData;

	/** 作成時刻（内部使用） */
	UPROPERTY()
	float CreationTime = 0.0f;

	/** コンストラクタ */
	FNotificationData()
		: Type(ENotificationType::Info)
		, Priority(ENotificationPriority::Normal)
		, DisplayDuration(3.0f)
		, FadeInDuration(0.3f)
		, FadeOutDuration(0.3f)
		, IconTexture(nullptr)
		, CreationTime(0.0f)
	{}

	/** 優先度による比較演算子（キューのソート用） */
	bool operator<(const FNotificationData& Other) const
	{
		if (Priority != Other.Priority)
		{
			return Priority < Other.Priority;
		}
		return CreationTime > Other.CreationTime; // 同じ優先度の場合は古い順
	}
};
