// Copyright Epic Games, Inc. All Rights Reserved.

#include "NotificationManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UNotificationManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UNotificationManager::Deinitialize()
{
	ClearAllNotifications();
	Super::Deinitialize();
}

void UNotificationManager::ShowNotification(const FNotificationData& NotificationData)
{
	FNotificationData DataWithTime = NotificationData;
	DataWithTime.CreationTime = GetWorld()->GetTimeSeconds();

	// キューに追加
	NotificationQueue.Add(DataWithTime);

	// 優先度順にソート
	SortNotificationQueue();

	// 現在表示中でなければ次の通知を表示
	if (!CurrentNotificationWidget)
	{
		ShowNextNotification();
	}
}

void UNotificationManager::ShowSimpleNotification(
	const FText& Title,
	const FText& Message,
	ENotificationType Type,
	ENotificationPriority Priority)
{
	FNotificationData Data;
	Data.Title = Title;
	Data.Message = Message;
	Data.Type = Type;
	Data.Priority = Priority;

	ShowNotification(Data);
}

void UNotificationManager::ClearAllNotifications()
{
	// タイマーをクリア
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DisplayTimerHandle);
	}

	// 現在の通知をクリア
	if (CurrentNotificationWidget)
	{
		CurrentNotificationWidget->RemoveFromParent();
		CurrentNotificationWidget = nullptr;
	}

	// キューをクリア
	NotificationQueue.Empty();
}

void UNotificationManager::SetNotificationWidgetClass(TSubclassOf<UNotificationWidget> WidgetClass)
{
	DefaultNotificationWidgetClass = WidgetClass;
}

void UNotificationManager::ShowNextNotification()
{
	if (NotificationQueue.Num() == 0)
	{
		return;
	}

	// ウィジェットクラスが設定されていない場合は処理できない
	if (!DefaultNotificationWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotificationManager: DefaultNotificationWidgetClass is not set!"));
		return;
	}

	// キューの先頭から通知を取得
	CurrentNotificationData = NotificationQueue[0];
	NotificationQueue.RemoveAt(0);

	// ウィジェットを作成
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotificationManager: PlayerController not found!"));
		return;
	}

	CurrentNotificationWidget = CreateWidget<UNotificationWidget>(PlayerController, DefaultNotificationWidgetClass);
	if (!CurrentNotificationWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("NotificationManager: Failed to create notification widget!"));
		return;
	}

	// 通知データを設定
	CurrentNotificationWidget->SetNotificationData(CurrentNotificationData);

	// イベントをバインド
	CurrentNotificationWidget->OnFadeInComplete.AddDynamic(this, &UNotificationManager::OnFadeInCompleted);
	CurrentNotificationWidget->OnFadeOutComplete.AddDynamic(this, &UNotificationManager::OnFadeOutCompleted);

	// ビューポートに追加
	CurrentNotificationWidget->AddToViewport(999); // 高いZ-Orderで表示

	// フェードイン開始
	CurrentNotificationWidget->StartFadeIn();

	// イベント通知
	OnNotificationShown.Broadcast(CurrentNotificationData);
}

void UNotificationManager::OnFadeInCompleted(UNotificationWidget* Widget)
{
	// 表示時間のタイマーを開始
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DisplayTimerHandle,
			this,
			&UNotificationManager::OnDisplayTimeElapsed,
			CurrentNotificationData.DisplayDuration,
			false
		);
	}
}

void UNotificationManager::OnFadeOutCompleted(UNotificationWidget* Widget)
{
	// ウィジェットを削除
	if (CurrentNotificationWidget)
	{
		CurrentNotificationWidget->RemoveFromParent();
		CurrentNotificationWidget = nullptr;
	}

	// イベント通知
	OnNotificationHidden.Broadcast(CurrentNotificationData);

	// 次の通知を表示
	ShowNextNotification();
}

void UNotificationManager::OnDisplayTimeElapsed()
{
	// フェードアウト開始
	if (CurrentNotificationWidget)
	{
		CurrentNotificationWidget->StartFadeOut();
	}
}

void UNotificationManager::SortNotificationQueue()
{
	// 優先度順にソート（降順）
	NotificationQueue.Sort([](const FNotificationData& A, const FNotificationData& B)
	{
		if (A.Priority != B.Priority)
		{
			return A.Priority > B.Priority; // 優先度が高い順
		}
		return A.CreationTime < B.CreationTime; // 同じ優先度の場合は古い順
	});
}
