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
	if (!ScreenClass || bIsTransitioning)
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
		Screen->OnEnter();
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
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return nullptr;
	}

	UScreenBase* NewScreen = CreateWidget<UScreenBase>(PC, ScreenClass);
	return NewScreen;
}
