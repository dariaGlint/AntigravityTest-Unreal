// Copyright Epic Games, Inc. All Rights Reserved.

#include "SnapshotInputHandler.h"
#include "StateSnapshotSubsystem.h"
#include "SnapshotManagerWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

USnapshotInputHandler::USnapshotInputHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USnapshotInputHandler::BeginPlay()
{
	Super::BeginPlay();

#if !WITH_DEBUG_STATE_SNAPSHOT
	return;
#endif

	// Get snapshot subsystem
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance)
	{
		SnapshotSubsystem = GameInstance->GetSubsystem<UStateSnapshotSubsystem>();
	}

	// Setup input bindings
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	// Add input mapping context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		if (SnapshotMappingContext)
		{
			Subsystem->AddMappingContext(SnapshotMappingContext, 100); // High priority
		}
	}

	// Bind input actions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		if (QuickSaveAction)
		{
			EnhancedInputComponent->BindAction(QuickSaveAction, ETriggerEvent::Triggered, this, &USnapshotInputHandler::OnQuickSave);
		}

		if (QuickLoadAction)
		{
			EnhancedInputComponent->BindAction(QuickLoadAction, ETriggerEvent::Triggered, this, &USnapshotInputHandler::OnQuickLoad);
		}

		if (ToggleManagerAction)
		{
			EnhancedInputComponent->BindAction(ToggleManagerAction, ETriggerEvent::Triggered, this, &USnapshotInputHandler::OnToggleManager);
		}
	}

	// Create snapshot manager widget
	if (SnapshotManagerWidgetClass)
	{
		ManagerWidget = CreateWidget<USnapshotManagerWidget>(PC, SnapshotManagerWidgetClass);
		if (ManagerWidget)
		{
			ManagerWidget->AddToViewport(100);
			ManagerWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void USnapshotInputHandler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_DEBUG_STATE_SNAPSHOT
	// Remove input mapping context
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn)
	{
		APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
		if (PC)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				if (SnapshotMappingContext)
				{
					Subsystem->RemoveMappingContext(SnapshotMappingContext);
				}
			}
		}
	}

	// Remove manager widget
	if (ManagerWidget)
	{
		ManagerWidget->RemoveFromParent();
		ManagerWidget = nullptr;
	}
#endif

	Super::EndPlay(EndPlayReason);
}

void USnapshotInputHandler::OnQuickSave()
{
#if WITH_DEBUG_STATE_SNAPSHOT
	if (!SnapshotSubsystem)
	{
		ShowNotification(TEXT("Snapshot system not available"), false);
		return;
	}

	bool bSuccess = SnapshotSubsystem->QuickSave();
	ShowNotification(bSuccess ? TEXT("Quick Save") : TEXT("Quick Save Failed"), bSuccess);
#endif
}

void USnapshotInputHandler::OnQuickLoad()
{
#if WITH_DEBUG_STATE_SNAPSHOT
	if (!SnapshotSubsystem)
	{
		ShowNotification(TEXT("Snapshot system not available"), false);
		return;
	}

	bool bSuccess = SnapshotSubsystem->QuickLoad();
	ShowNotification(bSuccess ? TEXT("Quick Load") : TEXT("Quick Load Failed"), bSuccess);
#endif
}

void USnapshotInputHandler::OnToggleManager()
{
#if WITH_DEBUG_STATE_SNAPSHOT
	if (ManagerWidget)
	{
		ManagerWidget->ToggleVisibility();
	}
#endif
}

void USnapshotInputHandler::ShowNotification(const FString& Message, bool bSuccess)
{
	if (GEngine)
	{
		FColor Color = bSuccess ? FColor::Green : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
}
