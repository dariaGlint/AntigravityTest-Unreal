// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NotificationData.h"
#include "NotificationWidget.h"
#include "NotificationManager.generated.h"

/**
 * ゲーム内通知を管理するGameInstanceSubsystem
 * キューイング、優先度管理、フェード制御を提供します
 */
UCLASS()
class NOTIFICATIONSYSTEM_API UNotificationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Subsystemの初期化 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * 通知を表示キューに追加
	 * @param NotificationData 表示する通知データ
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void ShowNotification(const FNotificationData& NotificationData);

	/**
	 * 簡易的な通知表示（タイトルとメッセージのみ）
	 * @param Title 通知のタイトル
	 * @param Message 通知のメッセージ
	 * @param Type 通知タイプ
	 * @param Priority 通知優先度
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void ShowSimpleNotification(
		const FText& Title,
		const FText& Message,
		ENotificationType Type = ENotificationType::Info,
		ENotificationPriority Priority = ENotificationPriority::Normal
	);

	/**
	 * すべての通知をクリア
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void ClearAllNotifications();

	/**
	 * 現在表示中の通知を取得
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	UNotificationWidget* GetCurrentNotification() const { return CurrentNotificationWidget; }

	/**
	 * キュー内の通知数を取得
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	int32 GetQueuedNotificationCount() const { return NotificationQueue.Num(); }

	/**
	 * 通知ウィジェットクラスを設定
	 * @param WidgetClass 使用する通知ウィジェットクラス
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void SetNotificationWidgetClass(TSubclassOf<UNotificationWidget> WidgetClass);

	/**
	 * 同時に表示する最大通知数を設定（現在は1のみサポート）
	 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void SetMaxSimultaneousNotifications(int32 MaxCount) { MaxSimultaneousNotifications = FMath::Max(1, MaxCount); }

	/** 通知が表示されたときのイベント */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationShown, const FNotificationData&, NotificationData);
	UPROPERTY(BlueprintAssignable, Category = "Notification")
	FOnNotificationShown OnNotificationShown;

	/** 通知が非表示になったときのイベント */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationHidden, const FNotificationData&, NotificationData);
	UPROPERTY(BlueprintAssignable, Category = "Notification")
	FOnNotificationHidden OnNotificationHidden;

protected:
	/** デフォルトの通知ウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Notification")
	TSubclassOf<UNotificationWidget> DefaultNotificationWidgetClass;

	/** 現在表示中の通知ウィジェット */
	UPROPERTY()
	UNotificationWidget* CurrentNotificationWidget = nullptr;

	/** 通知キュー（優先度順） */
	UPROPERTY()
	TArray<FNotificationData> NotificationQueue;

	/** 同時表示可能な最大通知数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Notification")
	int32 MaxSimultaneousNotifications = 1;

	/** 現在表示中の通知データ */
	UPROPERTY()
	FNotificationData CurrentNotificationData;

	/** 次の通知を表示 */
	void ShowNextNotification();

	/** フェードイン完了時の処理 */
	UFUNCTION()
	void OnFadeInCompleted(UNotificationWidget* Widget);

	/** フェードアウト完了時の処理 */
	UFUNCTION()
	void OnFadeOutCompleted(UNotificationWidget* Widget);

	/** タイマーハンドル：表示時間管理用 */
	FTimerHandle DisplayTimerHandle;

	/** 表示時間終了時の処理 */
	void OnDisplayTimeElapsed();

	/** キューをソート（優先度順） */
	void SortNotificationQueue();
};
