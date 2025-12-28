#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ScreenBase.h"
#include "TransitionEffect.h"
#include "ScreenTransitionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenChanged, UScreenBase*, OldScreen, UScreenBase*, NewScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionStarted, UScreenBase*, TargetScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionCompleted, UScreenBase*, CurrentScreen);

USTRUCT(BlueprintType)
struct FScreenStackEntry
{
	GENERATED_BODY()

	UPROPERTY()
	UScreenBase* Screen;

	UPROPERTY()
	bool bIsModal;

	FScreenStackEntry()
		: Screen(nullptr)
		, bIsModal(false)
	{}

	FScreenStackEntry(UScreenBase* InScreen, bool bInIsModal)
		: Screen(InScreen)
		, bIsModal(bInIsModal)
	{}
};

UCLASS()
class SCREENTRANSITIONSYSTEM_API UScreenTransitionManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void TransitionToScreen(TSubclassOf<UScreenBase> ScreenClass, bool bUseTransition = true, TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void PushScreen(TSubclassOf<UScreenBase> ScreenClass, bool bAsModal = false, bool bUseTransition = true, TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	bool PopScreen(bool bUseTransition = true, TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void ClearScreenStack();

	UFUNCTION(BlueprintPure, Category = "Screen Transition")
	UScreenBase* GetCurrentScreen() const { return CurrentScreen; }

	UFUNCTION(BlueprintPure, Category = "Screen Transition")
	int32 GetScreenStackDepth() const { return ScreenStack.Num(); }

	UFUNCTION(BlueprintPure, Category = "Screen Transition")
	bool IsTransitioning() const { return bIsTransitioning; }

	UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
	FOnScreenChanged OnScreenChanged;

	UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
	FOnTransitionStarted OnTransitionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
	FOnTransitionCompleted OnTransitionCompleted;

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void SetDefaultTransitionEffect(TSubclassOf<UTransitionEffect> TransitionEffectClass);

protected:
	UPROPERTY()
	UScreenBase* CurrentScreen;

	UPROPERTY()
	TArray<FScreenStackEntry> ScreenStack;

	UPROPERTY()
	bool bIsTransitioning;

	UPROPERTY()
	TSubclassOf<UTransitionEffect> DefaultTransitionEffectClass;

	UPROPERTY()
	UTransitionEffect* CurrentTransitionEffect;

private:
	void PerformTransition(UScreenBase* FromScreen, UScreenBase* ToScreen, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass);
	void OnTransitionEffectComplete();
	void ActivateScreen(UScreenBase* Screen);
	void DeactivateScreen(UScreenBase* Screen);
	UScreenBase* CreateScreen(TSubclassOf<UScreenBase> ScreenClass);
};
