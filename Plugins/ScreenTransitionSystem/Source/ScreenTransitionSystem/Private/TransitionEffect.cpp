#include "TransitionEffect.h"
#include "ScreenBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

UTransitionEffect::UTransitionEffect()
{
	TransitionType = ETransitionType::Fade;
	Duration = 0.5f;
	SlideDirection = ESlideDirection::Left;
	CurrentTime = 0.0f;
	bIsPlaying = false;
	FromScreenRef = nullptr;
	ToScreenRef = nullptr;
}

void UTransitionEffect::StartTransition_Implementation(UScreenBase* FromScreen, UScreenBase* ToScreen)
{
	FromScreenRef = FromScreen;
	ToScreenRef = ToScreen;
	CurrentTime = 0.0f;
	bIsPlaying = true;

	if (ToScreen)
	{
		ToScreen->AddToViewport(ToScreen->ZOrder);
	}

	if (Duration <= 0.0f)
	{
		CompleteTransition();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		CompleteTransition();
		return;
	}

	World->GetTimerManager().SetTimer(
		TransitionTimerHandle,
		this,
		&UTransitionEffect::OnTransitionTick,
		0.016f,
		true
	);
}

void UTransitionEffect::OnTransitionTick()
{
	if (!bIsPlaying)
	{
		return;
	}

	CurrentTime += 0.016f;
	float Alpha = FMath::Clamp(CurrentTime / Duration, 0.0f, 1.0f);

	TickTransition(Alpha);

	if (Alpha >= 1.0f)
	{
		CompleteTransition();
	}
}

void UTransitionEffect::UpdateTransition(float DeltaTime)
{
	if (!bIsPlaying)
	{
		return;
	}

	CurrentTime += DeltaTime;
	float Alpha = FMath::Clamp(CurrentTime / Duration, 0.0f, 1.0f);

	TickTransition(Alpha);

	if (Alpha >= 1.0f)
	{
		CompleteTransition();
	}
}

void UTransitionEffect::TickTransition_Implementation(float Alpha)
{
	float EasedAlpha = UKismetMathLibrary::Ease(0.0f, 1.0f, Alpha, EEasingFunc::EaseInOut);

	switch (TransitionType)
	{
	case ETransitionType::Fade:
	{
		if (FromScreenRef)
		{
			FromScreenRef->SetRenderOpacity(1.0f - EasedAlpha);
		}
		if (ToScreenRef)
		{
			ToScreenRef->SetRenderOpacity(EasedAlpha);
		}
		break;
	}
	case ETransitionType::Slide:
	{
		if (ToScreenRef && ToScreenRef->Slot)
		{
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ToScreenRef->Slot);
			if (CanvasSlot)
			{
				FVector2D ScreenSize = CanvasSlot->GetSize();
				FVector2D Offset = FVector2D::ZeroVector;

				switch (SlideDirection)
				{
				case ESlideDirection::Left:
					Offset.X = ScreenSize.X * (1.0f - EasedAlpha);
					break;
				case ESlideDirection::Right:
					Offset.X = -ScreenSize.X * (1.0f - EasedAlpha);
					break;
				case ESlideDirection::Up:
					Offset.Y = ScreenSize.Y * (1.0f - EasedAlpha);
					break;
				case ESlideDirection::Down:
					Offset.Y = -ScreenSize.Y * (1.0f - EasedAlpha);
					break;
				}

				CanvasSlot->SetPosition(Offset);
			}
		}
		break;
	}
	case ETransitionType::Custom:
		break;
	}
}

void UTransitionEffect::CompleteTransition()
{
	bIsPlaying = false;

	UWorld* World = GetWorld();
	if (World && TransitionTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(TransitionTimerHandle);
	}

	if (FromScreenRef)
	{
		FromScreenRef->SetRenderOpacity(1.0f);
	}

	if (ToScreenRef)
	{
		ToScreenRef->SetRenderOpacity(1.0f);
		if (ToScreenRef->Slot)
		{
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ToScreenRef->Slot);
			if (CanvasSlot)
			{
				CanvasSlot->SetPosition(FVector2D::ZeroVector);
			}
		}
	}

	OnTransitionComplete.Broadcast();
}
