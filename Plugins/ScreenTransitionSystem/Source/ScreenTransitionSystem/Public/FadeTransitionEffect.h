#pragma once

#include "CoreMinimal.h"
#include "TransitionEffect.h"
#include "FadeTransitionEffect.generated.h"

UENUM(BlueprintType)
enum class EFadeEasingType : uint8
{
	Linear UMETA(DisplayName = "Linear"),
	EaseIn UMETA(DisplayName = "Ease In"),
	EaseOut UMETA(DisplayName = "Ease Out"),
	EaseInOut UMETA(DisplayName = "Ease In Out")
};

/**
 * Fade transition effect with configurable easing curves
 * Smoothly transitions between screens using opacity fade
 */
UCLASS(Blueprintable, BlueprintType)
class SCREENTRANSITIONSYSTEM_API UFadeTransitionEffect : public UTransitionEffect
{
	GENERATED_BODY()

public:
	UFadeTransitionEffect();

	virtual void StartTransition_Implementation(UScreenBase* FromScreen, UScreenBase* ToScreen) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fade Transition")
	EFadeEasingType EasingType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fade Transition")
	FLinearColor IntermediateColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fade Transition")
	bool bUseIntermediateColor;

protected:
	virtual void TickTransition_Implementation(float Alpha) override;

private:
	float ApplyEasing(float Alpha) const;
};
