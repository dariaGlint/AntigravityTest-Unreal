#include "SlideTransitionEffect.h"
#include "ScreenBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

USlideTransitionEffect::USlideTransitionEffect()
{
	TransitionType = EScreenTransitionType::Slide;
	Duration = 0.4f;
	SlideDirection = EScreenTransitionSlideDirection::Left;
	EasingType = ESlideEasingType::EaseInOut;
	bUseParallax = false;
	ParallaxStrength = 0.5f;
	InitialFromPosition = FVector2D::ZeroVector;
	InitialToPosition = FVector2D::ZeroVector;
	ViewportSize = FVector2D::ZeroVector;
}

void USlideTransitionEffect::StartTransition_Implementation(UScreenBase* FromScreen, UScreenBase* ToScreen)
{
	Super::StartTransition_Implementation(FromScreen, ToScreen);

	ViewportSize = UWidgetLayoutLibrary::GetViewportSize(ToScreen);

	if (FromScreen)
	{
		UCanvasPanelSlot* FromSlot = Cast<UCanvasPanelSlot>(FromScreen->Slot);
		if (FromSlot)
		{
			InitialFromPosition = FromSlot->GetPosition();
		}
	}

	if (ToScreen)
	{
		UCanvasPanelSlot* ToSlot = Cast<UCanvasPanelSlot>(ToScreen->Slot);
		if (ToSlot)
		{
			InitialToPosition = ToSlot->GetPosition();

			FVector2D StartOffset = GetSlideOffset(SlideDirection, 0.0f);
			ToSlot->SetPosition(StartOffset);
		}
	}
}

void USlideTransitionEffect::TickTransition_Implementation(float Alpha)
{
	Super::TickTransition_Implementation(Alpha);

	float EasedAlpha = ApplyEasing(Alpha);

	FVector2D ToOffset = GetSlideOffset(SlideDirection, EasedAlpha);

	if (ToScreenRef)
	{
		UCanvasPanelSlot* ToSlot = Cast<UCanvasPanelSlot>(ToScreenRef->Slot);
		if (ToSlot)
		{
			ToSlot->SetPosition(FMath::Lerp(GetSlideOffset(SlideDirection, 0.0f), FVector2D::ZeroVector, EasedAlpha));
		}
	}

	if (FromScreenRef && bUseParallax)
	{
		UCanvasPanelSlot* FromSlot = Cast<UCanvasPanelSlot>(FromScreenRef->Slot);
		if (FromSlot)
		{
			FVector2D ParallaxOffset = ToOffset * ParallaxStrength;
			FromSlot->SetPosition(InitialFromPosition + ParallaxOffset);
		}
	}
	else if (FromScreenRef)
	{
		UCanvasPanelSlot* FromSlot = Cast<UCanvasPanelSlot>(FromScreenRef->Slot);
		if (FromSlot)
		{
			FVector2D OppositeOffset = -ToOffset;
			switch (SlideDirection)
			{
				case EScreenTransitionSlideDirection::Left:
					OppositeOffset = FVector2D(ViewportSize.X * EasedAlpha, 0);
					break;
				case EScreenTransitionSlideDirection::Right:
					OppositeOffset = FVector2D(-ViewportSize.X * EasedAlpha, 0);
					break;
				case EScreenTransitionSlideDirection::Up:
					OppositeOffset = FVector2D(0, ViewportSize.Y * EasedAlpha);
					break;
				case EScreenTransitionSlideDirection::Down:
					OppositeOffset = FVector2D(0, -ViewportSize.Y * EasedAlpha);
					break;
			}
			FromSlot->SetPosition(InitialFromPosition + OppositeOffset);
		}
	}
}

float USlideTransitionEffect::ApplyEasing(float Alpha) const
{
	switch (EasingType)
	{
		case ESlideEasingType::EaseIn:
			return Alpha * Alpha;

		case ESlideEasingType::EaseOut:
			return 1.0f - (1.0f - Alpha) * (1.0f - Alpha);

		case ESlideEasingType::EaseInOut:
		{
			if (Alpha < 0.5f)
			{
				return 2.0f * Alpha * Alpha;
			}
			else
			{
				float T = 2.0f * Alpha - 2.0f;
				return 1.0f - (T * T) / 2.0f;
			}
		}

		case ESlideEasingType::Linear:
		default:
			return Alpha;
	}
}

FVector2D USlideTransitionEffect::GetSlideOffset(EScreenTransitionSlideDirection Direction, float Alpha) const
{
	FVector2D Offset = FVector2D::ZeroVector;

	switch (Direction)
	{
		case EScreenTransitionSlideDirection::Left:
			Offset = FVector2D(-ViewportSize.X * (1.0f - Alpha), 0);
			break;
		case EScreenTransitionSlideDirection::Right:
			Offset = FVector2D(ViewportSize.X * (1.0f - Alpha), 0);
			break;
		case EScreenTransitionSlideDirection::Up:
			Offset = FVector2D(0, -ViewportSize.Y * (1.0f - Alpha));
			break;
		case EScreenTransitionSlideDirection::Down:
			Offset = FVector2D(0, ViewportSize.Y * (1.0f - Alpha));
			break;
	}

	return Offset;
}
