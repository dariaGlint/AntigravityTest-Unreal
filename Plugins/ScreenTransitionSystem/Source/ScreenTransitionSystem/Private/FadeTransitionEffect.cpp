#include "FadeTransitionEffect.h"
#include "ScreenBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

UFadeTransitionEffect::UFadeTransitionEffect()
{
	TransitionType = EScreenTransitionType::Fade;
	Duration = 0.3f;
	EasingType = EFadeEasingType::Linear;
	IntermediateColor = FLinearColor::Black;
	bUseIntermediateColor = false;
}

void UFadeTransitionEffect::StartTransition_Implementation(UScreenBase* FromScreen, UScreenBase* ToScreen)
{
	Super::StartTransition_Implementation(FromScreen, ToScreen);

	if (ToScreen)
	{
		ToScreen->SetRenderOpacity(0.0f);
	}
}

void UFadeTransitionEffect::TickTransition_Implementation(float Alpha)
{
	Super::TickTransition_Implementation(Alpha);

	float EasedAlpha = ApplyEasing(Alpha);

	if (bUseIntermediateColor)
	{
		if (Alpha < 0.5f)
		{
			float FadeOutAlpha = EasedAlpha * 2.0f;
			if (FromScreenRef)
			{
				FromScreenRef->SetRenderOpacity(1.0f - FadeOutAlpha);
			}
		}
		else
		{
			if (FromScreenRef)
			{
				FromScreenRef->SetRenderOpacity(0.0f);
			}

			float FadeInAlpha = (EasedAlpha - 0.5f) * 2.0f;
			if (ToScreenRef)
			{
				ToScreenRef->SetRenderOpacity(FadeInAlpha);
			}
		}
	}
	else
	{
		if (FromScreenRef)
		{
			FromScreenRef->SetRenderOpacity(1.0f - EasedAlpha);
		}

		if (ToScreenRef)
		{
			ToScreenRef->SetRenderOpacity(EasedAlpha);
		}
	}
}

float UFadeTransitionEffect::ApplyEasing(float Alpha) const
{
	switch (EasingType)
	{
		case EFadeEasingType::EaseIn:
			return Alpha * Alpha;

		case EFadeEasingType::EaseOut:
			return 1.0f - (1.0f - Alpha) * (1.0f - Alpha);

		case EFadeEasingType::EaseInOut:
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

		case EFadeEasingType::Linear:
		default:
			return Alpha;
	}
}
