#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ScreenBase.h"
#include "TransitionEffect.h"
#include "ScreenTransitionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenChanged, UScreenBase*, OldScreen, UScreenBase*, NewScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionStarted, UScreenBase*, TargetScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionCompleted, UScreenBase*, CurrentScreen);

UENUM(BlueprintType)
enum class EScreenTransitionError : uint8
{
	None UMETA(DisplayName = "None"),
	InvalidScreenClass UMETA(DisplayName = "Invalid Screen Class"),
	StackOverflow UMETA(DisplayName = "Stack Overflow"),
	StackUnderflow UMETA(DisplayName = "Stack Underflow"),
	TransitionTimeout UMETA(DisplayName = "Transition Timeout"),
	ContextValidationFailed UMETA(DisplayName = "Context Validation Failed"),
	ScreenCreationFailed UMETA(DisplayName = "Screen Creation Failed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenTransitionError, EScreenTransitionError, ErrorType, const FString&, ErrorMessage);

USTRUCT(BlueprintType)
struct SCREENTRANSITIONSYSTEM_API FScreenTransitionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, FString> StringData;

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, int32> IntData;

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, float> FloatData;

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, bool> BoolData;
};

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
	void TransitionToScreenWithContext(TSubclassOf<UScreenBase> ScreenClass, const FScreenTransitionContext& Context, bool bUseTransition = true, TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void PushScreen(TSubclassOf<UScreenBase> ScreenClass, bool bAsModal = false, bool bUseTransition = true, TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void PushScreenWithContext(TSubclassOf<UScreenBase> ScreenClass, const FScreenTransitionContext& Context, bool bAsModal = false, bool bUseTransition = true, TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr);

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

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void SetMaxStackDepth(int32 NewMaxDepth);

	UFUNCTION(BlueprintPure, Category = "Screen Transition")
	int32 GetMaxStackDepth() const { return MaxStackDepth; }

	UFUNCTION(BlueprintPure, Category = "Screen Transition")
	EScreenTransitionError GetLastError() const { return LastError; }

	UFUNCTION(BlueprintPure, Category = "Screen Transition")
	FString GetLastErrorMessage() const { return LastErrorMessage; }

	UFUNCTION(BlueprintCallable, Category = "Screen Transition")
	void ClearLastError();

	UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
	FOnScreenTransitionError OnTransitionError;

	// Console command callbacks
	static void DumpStackToLog();
	static void SetMaxDepthCommand(const TArray<FString>& Args, UWorld* World);

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

	UPROPERTY()
	int32 MaxStackDepth;

	UPROPERTY()
	EScreenTransitionError LastError;

	UPROPERTY()
	FString LastErrorMessage;

	UPROPERTY()
	FScreenTransitionContext PendingContext;

	UPROPERTY()
	bool bHasPendingContext;

private:
	void PerformTransition(UScreenBase* FromScreen, UScreenBase* ToScreen, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass);
	void OnTransitionEffectComplete();
	void ActivateScreen(UScreenBase* Screen);
	void DeactivateScreen(UScreenBase* Screen);
	UScreenBase* CreateScreen(TSubclassOf<UScreenBase> ScreenClass);
	void LogError(EScreenTransitionError ErrorType, const FString& ErrorMessage);
	bool ValidateStackDepth();
};
