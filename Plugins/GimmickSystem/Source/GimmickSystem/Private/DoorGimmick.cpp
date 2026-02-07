#include "DoorGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

ADoorGimmick::ADoorGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create door mesh component
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	// Default settings
	CurrentState = EDoorState::Closed;
	InitialState = EDoorState::Closed;
	bToggleOnInteract = true;
	bAutoClose = false;
	AutoCloseDelay = 3.0f;

	InteractionPrompt = FText::FromString(TEXT("Open Door"));
}

void ADoorGimmick::BeginPlay()
{
	Super::BeginPlay();

	// Set initial state
	SetDoorState(InitialState);
}

bool ADoorGimmick::CanInteract_Implementation(AActor* Interactor) const
{
	// Cannot interact while animating
	if (CurrentState == EDoorState::Opening || CurrentState == EDoorState::Closing)
	{
		return false;
	}

	return Super::CanInteract_Implementation(Interactor);
}

void ADoorGimmick::OnInteract_Implementation(AActor* Interactor)
{
	if (bToggleOnInteract)
	{
		// Toggle between open and closed
		if (IsOpen())
		{
			CloseDoor(Interactor);
		}
		else if (IsClosed())
		{
			OpenDoor(Interactor);
		}
	}
	else
	{
		// Always try to open
		if (!IsOpen())
		{
			OpenDoor(Interactor);
		}
	}
}

void ADoorGimmick::OpenDoor(AActor* Opener)
{
	if (IsLocked())
	{
		OnInteractionFailed.Broadcast(Opener);
		return;
	}

	if (CurrentState == EDoorState::Opening || CurrentState == EDoorState::Open)
	{
		return;
	}

	SetDoorState(EDoorState::Opening);
	PlayOpenAnimation();

	// If no animation implementation, complete immediately
	if (!GetClass()->GetDefaultObject()->FindFunction(FName("PlayOpenAnimation")))
	{
		OnOpenAnimationComplete();
	}
}

void ADoorGimmick::CloseDoor(AActor* Closer)
{
	if (CurrentState == EDoorState::Closing || CurrentState == EDoorState::Closed)
	{
		return;
	}

	// Clear auto-close timer if it exists
	if (AutoCloseTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoCloseTimerHandle);
	}

	SetDoorState(EDoorState::Closing);
	PlayCloseAnimation();

	// If no animation implementation, complete immediately
	if (!GetClass()->GetDefaultObject()->FindFunction(FName("PlayCloseAnimation")))
	{
		OnCloseAnimationComplete();
	}
}

void ADoorGimmick::LockDoor()
{
	if (!IsLocked())
	{
		SetDoorState(EDoorState::Locked);
		PlayLockAnimation();
		OnDoorLocked.Broadcast();
	}
}

void ADoorGimmick::UnlockDoor()
{
	if (IsLocked())
	{
		SetDoorState(EDoorState::Closed);
		PlayUnlockAnimation();
		OnDoorUnlocked.Broadcast();
	}
}

void ADoorGimmick::SetDoorState(EDoorState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		OnStateChanged.Broadcast();

		// Update interaction prompt based on state
		switch (CurrentState)
		{
		case EDoorState::Open:
			InteractionPrompt = FText::FromString(TEXT("Close Door"));
			break;
		case EDoorState::Closed:
			InteractionPrompt = FText::FromString(TEXT("Open Door"));
			break;
		case EDoorState::Locked:
			InteractionPrompt = FText::FromString(TEXT("Locked"));
			break;
		default:
			break;
		}
	}
}

void ADoorGimmick::OnOpenAnimationComplete()
{
	SetDoorState(EDoorState::Open);
	OnDoorOpened.Broadcast(nullptr);

	if (bAutoClose)
	{
		StartAutoCloseTimer();
	}
}

void ADoorGimmick::OnCloseAnimationComplete()
{
	SetDoorState(EDoorState::Closed);
	OnDoorClosed.Broadcast(nullptr);
}

void ADoorGimmick::StartAutoCloseTimer()
{
	if (bAutoClose && AutoCloseDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoCloseTimerHandle,
			[this]() { CloseDoor(nullptr); },
			AutoCloseDelay,
			false
		);
	}
}
