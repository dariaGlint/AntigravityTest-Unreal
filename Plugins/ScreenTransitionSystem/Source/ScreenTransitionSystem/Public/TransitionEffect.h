#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TransitionEffect.generated.h"

class UScreenBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransitionComplete);

UENUM(BlueprintType)
enum class EScreenTransitionType : uint8
{
	Fade UMETA(DisplayName = "Fade"),
	Slide UMETA(DisplayName = "Slide"),
	Custom UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class EScreenTransitionSlideDirection : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down")
};

UCLASS(Blueprintable, BlueprintType)
class SCREENTRANSITIONSYSTEM_API UTransitionEffect : public UObject
{
	GENERATED_BODY()

public:
	UTransitionEffect();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Transition")
	void StartTransition(UScreenBase* FromScreen, UScreenBase* ToScreen);
	virtual void StartTransition_Implementation(UScreenBase* FromScreen, UScreenBase* ToScreen);

	UFUNCTION(BlueprintCallable, Category = "Transition")
	void CompleteTransition();

	UPROPERTY(BlueprintAssignable, Category = "Transition")
	FOnTransitionComplete OnTransitionComplete;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transition")
	EScreenTransitionType TransitionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transition")
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transition", meta = (EditCondition = "TransitionType == EScreenTransitionType::Slide"))
	EScreenTransitionSlideDirection SlideDirection;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Transition")
	UScreenBase* FromScreenRef;

	UPROPERTY(BlueprintReadOnly, Category = "Transition")
	UScreenBase* ToScreenRef;

	UPROPERTY(BlueprintReadOnly, Category = "Transition")
	float CurrentTime;

	UPROPERTY(BlueprintReadOnly, Category = "Transition")
	bool bIsPlaying;

	UFUNCTION(BlueprintCallable, Category = "Transition")
	void UpdateTransition(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Transition")
	void TickTransition(float Alpha);
	virtual void TickTransition_Implementation(float Alpha);

private:
	FTimerHandle TransitionTimerHandle;
	void OnTransitionTick();
};
