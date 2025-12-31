#include "ScreenTransitionManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UScreenTransitionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentScreen = nullptr;
	bIsTransitioning = false;
	CurrentTransitionEffect = nullptr;
	ScreenStack.Empty();
	MaxStackDepth = 10;
	LastError = EScreenTransitionError::None;
	LastErrorMessage = TEXT("");
	bHasPendingContext = false;
}

void UScreenTransitionManager::Deinitialize()
{
	ClearScreenStack();

	if (CurrentScreen)
	{
		DeactivateScreen(CurrentScreen);
		CurrentScreen = nullptr;
	}

	if (CurrentTransitionEffect)
	{
		CurrentTransitionEffect->ConditionalBeginDestroy();
		CurrentTransitionEffect = nullptr;
	}

	Super::Deinitialize();
}

void UScreenTransitionManager::TransitionToScreen(TSubclassOf<UScreenBase> ScreenClass, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	if (!ScreenClass || bIsTransitioning)
	{
		return;
	}

	UScreenBase* NewScreen = CreateScreen(ScreenClass);
	if (!NewScreen)
	{
		return;
	}

	UScreenBase* OldScreen = CurrentScreen;

	ClearScreenStack();

	PerformTransition(OldScreen, NewScreen, bUseTransition, TransitionEffectClass);
}

void UScreenTransitionManager::PushScreen(TSubclassOf<UScreenBase> ScreenClass, bool bAsModal, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	if (!ScreenClass)
	{
		LogError(EScreenTransitionError::InvalidScreenClass, TEXT("PushScreen: Screen class is null"));
		return;
	}

	if (bIsTransitioning)
	{
		UE_LOG(LogTemp, Warning, TEXT("PushScreen: Already transitioning, ignoring request"));
		return;
	}

	if (!ValidateStackDepth())
	{
		return;
	}

	UScreenBase* NewScreen = CreateScreen(ScreenClass);
	if (!NewScreen)
	{
		return;
	}

	if (CurrentScreen)
	{
		ScreenStack.Add(FScreenStackEntry(CurrentScreen, false));
	}

	UScreenBase* OldScreen = CurrentScreen;

	PerformTransition(OldScreen, NewScreen, bUseTransition, TransitionEffectClass);
}

bool UScreenTransitionManager::PopScreen(bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	if (ScreenStack.Num() == 0 || bIsTransitioning)
	{
		return false;
	}

	FScreenStackEntry LastEntry = ScreenStack.Last();
	ScreenStack.RemoveAt(ScreenStack.Num() - 1);

	UScreenBase* OldScreen = CurrentScreen;
	UScreenBase* NewScreen = LastEntry.Screen;

	PerformTransition(OldScreen, NewScreen, bUseTransition, TransitionEffectClass);

	return true;
}

void UScreenTransitionManager::ClearScreenStack()
{
	for (const FScreenStackEntry& Entry : ScreenStack)
	{
		if (Entry.Screen)
		{
			DeactivateScreen(Entry.Screen);
		}
	}
	ScreenStack.Empty();
}

void UScreenTransitionManager::SetDefaultTransitionEffect(TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	DefaultTransitionEffectClass = TransitionEffectClass;
}

void UScreenTransitionManager::PerformTransition(UScreenBase* FromScreen, UScreenBase* ToScreen, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	if (!ToScreen)
	{
		return;
	}

	bIsTransitioning = true;
	OnTransitionStarted.Broadcast(ToScreen);

	if (FromScreen)
	{
		FromScreen->OnExit();
	}

	if (bUseTransition)
	{
		TSubclassOf<UTransitionEffect> EffectClass = TransitionEffectClass ? TransitionEffectClass : DefaultTransitionEffectClass;

		if (EffectClass)
		{
			CurrentTransitionEffect = NewObject<UTransitionEffect>(this, EffectClass);
			if (CurrentTransitionEffect)
			{
				CurrentTransitionEffect->OnTransitionComplete.AddDynamic(this, &UScreenTransitionManager::OnTransitionEffectComplete);
				CurrentTransitionEffect->StartTransition(FromScreen, ToScreen);

				CurrentScreen = ToScreen;
				OnScreenChanged.Broadcast(FromScreen, ToScreen);

				if (FromScreen && FromScreen != ToScreen)
				{
					DeactivateScreen(FromScreen);
				}

				return;
			}
		}
	}

	if (FromScreen && FromScreen != ToScreen)
	{
		DeactivateScreen(FromScreen);
	}

	CurrentScreen = ToScreen;
	OnScreenChanged.Broadcast(FromScreen, ToScreen);
	ActivateScreen(ToScreen);

	bIsTransitioning = false;
	OnTransitionCompleted.Broadcast(ToScreen);
}

void UScreenTransitionManager::OnTransitionEffectComplete()
{
	if (CurrentTransitionEffect)
	{
		CurrentTransitionEffect->OnTransitionComplete.RemoveDynamic(this, &UScreenTransitionManager::OnTransitionEffectComplete);
		CurrentTransitionEffect = nullptr;
	}

	if (CurrentScreen)
	{
		ActivateScreen(CurrentScreen);
	}

	bIsTransitioning = false;
	OnTransitionCompleted.Broadcast(CurrentScreen);
}

void UScreenTransitionManager::ActivateScreen(UScreenBase* Screen)
{
	if (Screen)
	{
		if (bHasPendingContext)
		{
			Screen->OnEnterWithContext(PendingContext);
			bHasPendingContext = false;
			PendingContext = FScreenTransitionContext();
		}
		else
		{
			Screen->OnEnter();
		}
	}
}

void UScreenTransitionManager::DeactivateScreen(UScreenBase* Screen)
{
	if (Screen)
	{
		Screen->RemoveFromParent();
		Screen->ConditionalBeginDestroy();
	}
}

UScreenBase* UScreenTransitionManager::CreateScreen(TSubclassOf<UScreenBase> ScreenClass)
{
	if (!ScreenClass)
	{
		LogError(EScreenTransitionError::InvalidScreenClass, TEXT("Screen class is null"));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		LogError(EScreenTransitionError::ScreenCreationFailed, TEXT("World is null"));
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		LogError(EScreenTransitionError::ScreenCreationFailed, TEXT("PlayerController is null"));
		return nullptr;
	}

	UScreenBase* NewScreen = CreateWidget<UScreenBase>(PC, ScreenClass);
	if (!NewScreen)
	{
		LogError(EScreenTransitionError::ScreenCreationFailed, FString::Printf(TEXT("Failed to create widget of class %s"), *ScreenClass->GetName()));
		return nullptr;
	}

	return NewScreen;
}

void UScreenTransitionManager::TransitionToScreenWithContext(TSubclassOf<UScreenBase> ScreenClass, const FScreenTransitionContext& Context, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	PendingContext = Context;
	bHasPendingContext = true;
	TransitionToScreen(ScreenClass, bUseTransition, TransitionEffectClass);
}

void UScreenTransitionManager::PushScreenWithContext(TSubclassOf<UScreenBase> ScreenClass, const FScreenTransitionContext& Context, bool bAsModal, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass)
{
	PendingContext = Context;
	bHasPendingContext = true;
	PushScreen(ScreenClass, bAsModal, bUseTransition, TransitionEffectClass);
}

void UScreenTransitionManager::SetMaxStackDepth(int32 NewMaxDepth)
{
	MaxStackDepth = FMath::Clamp(NewMaxDepth, 1, 100);
	UE_LOG(LogTemp, Log, TEXT("ScreenTransitionManager: MaxStackDepth set to %d"), MaxStackDepth);
}

void UScreenTransitionManager::ClearLastError()
{
	LastError = EScreenTransitionError::None;
	LastErrorMessage = TEXT("");
}

void UScreenTransitionManager::LogError(EScreenTransitionError ErrorType, const FString& ErrorMessage)
{
	LastError = ErrorType;
	LastErrorMessage = ErrorMessage;

	UE_LOG(LogTemp, Error, TEXT("ScreenTransitionManager Error: %s"), *ErrorMessage);

	OnTransitionError.Broadcast(ErrorType, ErrorMessage);
}

bool UScreenTransitionManager::ValidateStackDepth()
{
	if (ScreenStack.Num() >= MaxStackDepth)
	{
		LogError(EScreenTransitionError::StackOverflow, FString::Printf(TEXT("Stack depth exceeded maximum of %d"), MaxStackDepth));
		return false;
	}
	return true;
}
