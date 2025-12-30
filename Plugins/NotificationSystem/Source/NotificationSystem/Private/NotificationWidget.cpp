// Copyright Epic Games, Inc. All Rights Reserved.

#include "NotificationWidget.h"

void UNotificationWidget::SetNotificationData(const FNotificationData& InData)
{
	NotificationData = InData;
	OnDataUpdated(NotificationData);
}

void UNotificationWidget::StartFadeIn()
{
	bIsFading = true;
	bIsFadingIn = true;
	FadeElapsedTime = 0.0f;
	CurrentFadeAlpha = 0.0f;
	CurrentFadeDuration = NotificationData.FadeInDuration;
	OnNotificationShown();
}

void UNotificationWidget::StartFadeOut()
{
	bIsFading = true;
	bIsFadingIn = false;
	FadeElapsedTime = 0.0f;
	CurrentFadeAlpha = 1.0f;
	CurrentFadeDuration = NotificationData.FadeOutDuration;
}

void UNotificationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsFading)
	{
		FadeElapsedTime += InDeltaTime;

		if (CurrentFadeDuration > 0.0f)
		{
			float Progress = FMath::Clamp(FadeElapsedTime / CurrentFadeDuration, 0.0f, 1.0f);

			if (bIsFadingIn)
			{
				CurrentFadeAlpha = Progress;
				OnFadeIn(CurrentFadeAlpha);

				if (Progress >= 1.0f)
				{
					bIsFading = false;
					OnFadeInComplete.Broadcast(this);
				}
			}
			else
			{
				CurrentFadeAlpha = 1.0f - Progress;
				OnFadeOut(CurrentFadeAlpha);

				if (Progress >= 1.0f)
				{
					bIsFading = false;
					OnNotificationHidden();
					OnFadeOutComplete.Broadcast(this);
				}
			}
		}
		else
		{
			// フェード時間が0の場合は即座に完了
			CurrentFadeAlpha = bIsFadingIn ? 1.0f : 0.0f;
			bIsFading = false;

			if (bIsFadingIn)
			{
				OnFadeIn(CurrentFadeAlpha);
				OnFadeInComplete.Broadcast(this);
			}
			else
			{
				OnFadeOut(CurrentFadeAlpha);
				OnNotificationHidden();
				OnFadeOutComplete.Broadcast(this);
			}
		}
	}
}
