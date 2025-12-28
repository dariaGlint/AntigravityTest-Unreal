#include "ScreenBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UScreenBase::UScreenBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	ZOrder = 0;
	bIsActive = false;
	bPreviousMouseCursorState = false;
	bPreviousClickEventsState = false;
}

void UScreenBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetZOrder(ZOrder);
}

void UScreenBase::NativeDestruct()
{
	if (bIsActive)
	{
		OnExit();
	}

	Super::NativeDestruct();
}

void UScreenBase::OnEnter_Implementation()
{
	if (bIsActive)
	{
		return;
	}

	bIsActive = true;

	AddToViewport(ZOrder);

	ApplyInputSettings();

	OnScreenEnterDelegate.Broadcast();
}

void UScreenBase::OnExit_Implementation()
{
	if (!bIsActive)
	{
		return;
	}

	bIsActive = false;

	RestoreInputSettings();

	OnScreenExitDelegate.Broadcast();
}

void UScreenBase::OnPause_Implementation()
{
	OnScreenPauseDelegate.Broadcast();
}

void UScreenBase::OnResume_Implementation()
{
	OnScreenResumeDelegate.Broadcast();
}

void UScreenBase::ApplyInputSettings()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	bPreviousMouseCursorState = PC->bShowMouseCursor;
	bPreviousClickEventsState = PC->bEnableClickEvents;

	PC->bShowMouseCursor = bShowMouseCursor;
	PC->bEnableClickEvents = bEnableClickEvents;
}

void UScreenBase::RestoreInputSettings()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	PC->bShowMouseCursor = bPreviousMouseCursorState;
	PC->bEnableClickEvents = bPreviousClickEventsState;
}
