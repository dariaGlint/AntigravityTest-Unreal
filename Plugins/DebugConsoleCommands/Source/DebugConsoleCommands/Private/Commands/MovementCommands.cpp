// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandSubsystem.h"
#include "DebugConsoleCommandsModule.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

#if WITH_DEBUG_COMMANDS

namespace DebugCommands
{
	extern ACharacter* GetPlayerCharacter(UWorld* World);
	extern APawn* GetPlayerPawn(UWorld* World);
}

void UDebugCommandSubsystem::RegisterMovementCommands()
{
	// Teleport - Move to coordinates
	RegisterNativeCommand(
		TEXT("Teleport"),
		TEXT("Teleport player to specified coordinates"),
		EDebugCommandCategory::Movement,
		TEXT("Teleport X Y Z"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 3)
			{
				return TEXT("Usage: Teleport X Y Z");
			}

			APawn* Pawn = DebugCommands::GetPlayerPawn(World);
			if (!Pawn)
			{
				return TEXT("No player pawn found");
			}

			float X = FCString::Atof(*Args[0]);
			float Y = FCString::Atof(*Args[1]);
			float Z = FCString::Atof(*Args[2]);

			FVector NewLocation(X, Y, Z);
			Pawn->SetActorLocation(NewLocation);

			return FString::Printf(TEXT("Teleported to (%.1f, %.1f, %.1f)"), X, Y, Z);
		}
	);

	// SetSpeed - Change movement speed multiplier
	RegisterNativeCommand(
		TEXT("SetSpeed"),
		TEXT("Set movement speed multiplier"),
		EDebugCommandCategory::Movement,
		TEXT("SetSpeed <Multiplier>"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: SetSpeed <Multiplier>");
			}

			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
			if (!MovementComp)
			{
				return TEXT("No movement component found");
			}

			float Multiplier = FCString::Atof(*Args[0]);
			if (Multiplier <= 0)
			{
				return TEXT("Multiplier must be greater than 0");
			}

			// Store original speed if not stored yet
			static float OriginalMaxWalkSpeed = 0.0f;
			if (OriginalMaxWalkSpeed == 0.0f)
			{
				OriginalMaxWalkSpeed = MovementComp->MaxWalkSpeed;
			}

			MovementComp->MaxWalkSpeed = OriginalMaxWalkSpeed * Multiplier;

			return FString::Printf(TEXT("Speed multiplier set to %.2fx (MaxWalkSpeed: %.1f)"), Multiplier, MovementComp->MaxWalkSpeed);
		}
	);

	// Fly - Toggle flying mode
	RegisterNativeCommand(
		TEXT("Fly"),
		TEXT("Toggle flying mode"),
		EDebugCommandCategory::Movement,
		TEXT("Fly"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
			if (!MovementComp)
			{
				return TEXT("No movement component found");
			}

			if (MovementComp->MovementMode == MOVE_Flying)
			{
				MovementComp->SetMovementMode(MOVE_Walking);
				return TEXT("Flying disabled");
			}
			else
			{
				MovementComp->SetMovementMode(MOVE_Flying);
				return TEXT("Flying enabled");
			}
		}
	);

	// Ghost - Toggle no-clip mode
	RegisterNativeCommand(
		TEXT("Ghost"),
		TEXT("Toggle ghost (no-clip) mode"),
		EDebugCommandCategory::Movement,
		TEXT("Ghost"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
			if (!MovementComp)
			{
				return TEXT("No movement component found");
			}

			// Toggle collision and flying
			bool bIsGhost = !Character->GetActorEnableCollision();

			Character->SetActorEnableCollision(!bIsGhost);

			if (bIsGhost)
			{
				// Disable ghost mode
				MovementComp->SetMovementMode(MOVE_Walking);
				return TEXT("Ghost mode disabled");
			}
			else
			{
				// Enable ghost mode
				MovementComp->SetMovementMode(MOVE_Flying);
				return TEXT("Ghost mode enabled (no-clip + flying)");
			}
		}
	);
}

#endif // WITH_DEBUG_COMMANDS
