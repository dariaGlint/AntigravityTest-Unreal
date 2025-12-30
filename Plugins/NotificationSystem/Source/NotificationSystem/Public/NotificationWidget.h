// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationData.h"
#include "NotificationWidget.generated.h"

/**
 * 通知ウィジェットの基底クラス
 * このクラスを継承してBlueprint/C++で独自の通知UIを作成できます
 */
UCLASS(Blueprintable, BlueprintType)
class NOTIFICATIONSYSTEM_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 通知データを設定（内部使用） */
	void SetNotificationData(const FNotificationData& InData);

	/** 通知データを取得 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	const FNotificationData& GetNotificationData() const { return NotificationData; }

	/** フェードイン開始 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	virtual void StartFadeIn();

	/** フェードアウト開始 */
	UFUNCTION(BlueprintCallable, Category = "Notification")
	virtual void StartFadeOut();

	/** 通知が表示されたときに呼ばれるイベント */
	UFUNCTION(BlueprintImplementableEvent, Category = "Notification")
	void OnNotificationShown();

	/** 通知が非表示になったときに呼ばれるイベント */
	UFUNCTION(BlueprintImplementableEvent, Category = "Notification")
	void OnNotificationHidden();

	/** フェードイン中に呼ばれるイベント */
	UFUNCTION(BlueprintImplementableEvent, Category = "Notification")
	void OnFadeIn(float Alpha);

	/** フェードアウト中に呼ばれるイベント */
	UFUNCTION(BlueprintImplementableEvent, Category = "Notification")
	void OnFadeOut(float Alpha);

	/** 通知データが更新されたときに呼ばれるイベント */
	UFUNCTION(BlueprintImplementableEvent, Category = "Notification")
	void OnDataUpdated(const FNotificationData& Data);

	/** フェードイン完了の通知 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeInComplete, UNotificationWidget*, Widget);
	UPROPERTY(BlueprintAssignable, Category = "Notification")
	FOnFadeInComplete OnFadeInComplete;

	/** フェードアウト完了の通知 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeOutComplete, UNotificationWidget*, Widget);
	UPROPERTY(BlueprintAssignable, Category = "Notification")
	FOnFadeOutComplete OnFadeOutComplete;

protected:
	/** 通知データ */
	UPROPERTY(BlueprintReadOnly, Category = "Notification")
	FNotificationData NotificationData;

	/** フェード進行状態 */
	UPROPERTY(BlueprintReadOnly, Category = "Notification")
	float CurrentFadeAlpha = 0.0f;

	/** フェード中かどうか */
	UPROPERTY(BlueprintReadOnly, Category = "Notification")
	bool bIsFading = false;

	/** フェードイン中かどうか */
	UPROPERTY(BlueprintReadOnly, Category = "Notification")
	bool bIsFadingIn = false;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float FadeElapsedTime = 0.0f;
	float CurrentFadeDuration = 0.0f;
};
