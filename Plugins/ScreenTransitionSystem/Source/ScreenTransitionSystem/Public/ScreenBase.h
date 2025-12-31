#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreenBase.generated.h"

// Forward declaration
struct FScreenTransitionContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenExit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenPause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenResume);

UCLASS(Abstract, Blueprintable)
class SCREENTRANSITIONSYSTEM_API UScreenBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UScreenBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
	void OnEnter();
	virtual void OnEnter_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
	void OnEnterWithContext(const FScreenTransitionContext& Context);
	virtual void OnEnterWithContext_Implementation(const FScreenTransitionContext& Context);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
	void OnExit();
	virtual void OnExit_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
	void OnPause();
	virtual void OnPause_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
	void OnResume();
	virtual void OnResume_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
	bool OnValidate();
	virtual bool OnValidate_Implementation();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Screen")
	FString GetScreenName() const;
	virtual FString GetScreenName_Implementation() const;

	UPROPERTY(BlueprintAssignable, Category = "Screen")
	FOnScreenEnter OnScreenEnterDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Screen")
	FOnScreenExit OnScreenExitDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Screen")
	FOnScreenPause OnScreenPauseDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Screen")
	FOnScreenResume OnScreenResumeDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Screen")
	bool bShowMouseCursor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Screen")
	bool bEnableClickEvents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Screen")
	int32 ZOrder;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Screen")
	bool bIsActive;

private:
	void ApplyInputSettings();
	void RestoreInputSettings();

	bool bPreviousMouseCursorState;
	bool bPreviousClickEventsState;
};
