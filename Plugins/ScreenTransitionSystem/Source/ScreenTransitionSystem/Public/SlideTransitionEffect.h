#pragma once

#include "CoreMinimal.h"
#include "TransitionEffect.h"
#include "SlideTransitionEffect.generated.h"

UENUM(BlueprintType)
enum class ESlideEasingType : uint8
{
	Linear UMETA(DisplayName = "Linear"),
	EaseIn UMETA(DisplayName = "Ease In"),
	EaseOut UMETA(DisplayName = "Ease Out"),
	EaseInOut UMETA(DisplayName = "Ease In Out")
};

/**
 * Slide transition effect with directional movement and easing curves
 * Smoothly slides screens in the specified direction
 */
UCLASS(Blueprintable, BlueprintType)
class SCREENTRANSITIONSYSTEM_API USlideTransitionEffect : public UTransitionEffect
{
	GENERATED_BODY()

public:
	USlideTransitionEffect();

	virtual void StartTransition_Implementation(UScreenBase* FromScreen, UScreenBase* ToScreen) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slide Transition")
	ESlideEasingType EasingType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slide Transition")
	bool bUseParallax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slide Transition", meta = (EditCondition = "bUseParallax"))
	float ParallaxStrength;

protected:
	virtual void TickTransition_Implementation(float Alpha) override;

private:
	float ApplyEasing(float Alpha) const;
	FVector2D GetSlideOffset(EScreenTransitionSlideDirection Direction, float Alpha) const;
	FVector2D InitialFromPosition;
	FVector2D InitialToPosition;
	FVector2D ViewportSize;
};
