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

	FString GetCategoryName(EDebugCommandCategory Category)
	{
		switch (Category)
		{
		case EDebugCommandCategory::Player:   return TEXT("Player");
		case EDebugCommandCategory::Movement: return TEXT("Movement");
		case EDebugCommandCategory::Combat:   return TEXT("Combat");
		case EDebugCommandCategory::Spawn:    return TEXT("Spawn");
		case EDebugCommandCategory::System:   return TEXT("System");
		case EDebugCommandCategory::Info:     return TEXT("Info");
		case EDebugCommandCategory::Custom:   return TEXT("Custom");
		default: return TEXT("Unknown");
		}
	}
}

void UDebugCommandSubsystem::RegisterInfoCommands()
{
	// ListCommands - Show available commands
	RegisterNativeCommand(
		TEXT("ListCommands"),
		TEXT("List all available debug commands"),
		EDebugCommandCategory::Info,
		TEXT("ListCommands [Category]"),
		[this](const TArray<FString>& Args, UWorld* World) -> FString
		{
			FString Result;

			// Filter by category if specified
			EDebugCommandCategory FilterCategory = EDebugCommandCategory::Custom;
			bool bHasFilter = false;

			if (Args.Num() > 0)
			{
				FString CategoryName = Args[0].ToLower();
				if (CategoryName == TEXT("player")) { FilterCategory = EDebugCommandCategory::Player; bHasFilter = true; }
				else if (CategoryName == TEXT("movement")) { FilterCategory = EDebugCommandCategory::Movement; bHasFilter = true; }
				else if (CategoryName == TEXT("combat")) { FilterCategory = EDebugCommandCategory::Combat; bHasFilter = true; }
				else if (CategoryName == TEXT("spawn")) { FilterCategory = EDebugCommandCategory::Spawn; bHasFilter = true; }
				else if (CategoryName == TEXT("system")) { FilterCategory = EDebugCommandCategory::System; bHasFilter = true; }
				else if (CategoryName == TEXT("info")) { FilterCategory = EDebugCommandCategory::Info; bHasFilter = true; }
				else if (CategoryName == TEXT("custom")) { FilterCategory = EDebugCommandCategory::Custom; bHasFilter = true; }
			}

			// Group commands by category
			TMap<EDebugCommandCategory, TArray<FDebugCommand>> GroupedCommands;
			for (const auto& Pair : Commands)
			{
				if (!bHasFilter || Pair.Value.Category == FilterCategory)
				{
					GroupedCommands.FindOrAdd(Pair.Value.Category).Add(Pair.Value);
				}
			}

			// Build result string
			Result = TEXT("Available Debug Commands:\n");
			Result += TEXT("========================\n");

			if (bHasFilter)
			{
				Result += FString::Printf(TEXT("[%s]\n"), *DebugCommands::GetCategoryName(FilterCategory));
			}

			for (const auto& CategoryPair : GroupedCommands)
			{
				if (!bHasFilter)
				{
					Result += FString::Printf(TEXT("\n[%s]\n"), *DebugCommands::GetCategoryName(CategoryPair.Key));
				}

				for (const FDebugCommand& Cmd : CategoryPair.Value)
				{
					Result += FString::Printf(TEXT("  %s - %s\n"), *Cmd.Name, *Cmd.Description);
				}
			}

			Result += TEXT("\nUse 'Debug Help <command>' for detailed usage.");

			return Result;
		}
	);

	// Help - Show command help
	RegisterNativeCommand(
		TEXT("Help"),
		TEXT("Show detailed help for a command"),
		EDebugCommandCategory::Info,
		TEXT("Help <CommandName>"),
		[this](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: Help <CommandName>\nUse 'ListCommands' to see available commands.");
			}

			FString CommandName = Args[0].ToLower();
			FDebugCommand Command;

			if (GetCommand(CommandName, Command))
			{
				FString Result;
				Result += FString::Printf(TEXT("Command: %s\n"), *Command.Name);
				Result += FString::Printf(TEXT("Category: %s\n"), *DebugCommands::GetCategoryName(Command.Category));
				Result += FString::Printf(TEXT("Description: %s\n"), *Command.Description);
				Result += FString::Printf(TEXT("Usage: Debug %s"), *Command.Usage);
				return Result;
			}

			return FString::Printf(TEXT("Unknown command: %s"), *Args[0]);
		}
	);

	// ShowStats - Show player statistics
	RegisterNativeCommand(
		TEXT("ShowStats"),
		TEXT("Display current player statistics"),
		EDebugCommandCategory::Info,
		TEXT("ShowStats"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			FString Result = TEXT("Player Statistics:\n");
			Result += TEXT("==================\n");

			// Location
			FVector Location = Character->GetActorLocation();
			Result += FString::Printf(TEXT("Location: (%.1f, %.1f, %.1f)\n"), Location.X, Location.Y, Location.Z);

			// Rotation
			FRotator Rotation = Character->GetActorRotation();
			Result += FString::Printf(TEXT("Rotation: (Pitch: %.1f, Yaw: %.1f, Roll: %.1f)\n"),
				Rotation.Pitch, Rotation.Yaw, Rotation.Roll);

			// Velocity
			FVector Velocity = Character->GetVelocity();
			Result += FString::Printf(TEXT("Velocity: %.1f (%.1f, %.1f, %.1f)\n"),
				Velocity.Size(), Velocity.X, Velocity.Y, Velocity.Z);

			// Movement mode
			UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
			if (MovementComp)
			{
				FString MovementMode;
				switch (MovementComp->MovementMode)
				{
				case MOVE_Walking: MovementMode = TEXT("Walking"); break;
				case MOVE_Falling: MovementMode = TEXT("Falling"); break;
				case MOVE_Flying: MovementMode = TEXT("Flying"); break;
				case MOVE_Swimming: MovementMode = TEXT("Swimming"); break;
				case MOVE_Custom: MovementMode = TEXT("Custom"); break;
				default: MovementMode = TEXT("None"); break;
				}
				Result += FString::Printf(TEXT("Movement Mode: %s\n"), *MovementMode);
				Result += FString::Printf(TEXT("Max Walk Speed: %.1f\n"), MovementComp->MaxWalkSpeed);
			}

			// Can be damaged
			Result += FString::Printf(TEXT("Can Be Damaged: %s\n"), Character->CanBeDamaged() ? TEXT("Yes") : TEXT("No (God Mode)"));

			// Try to get HP info via UFunction
			UFunction* GetHPFunc = Character->FindFunction(TEXT("DebugGetHP"));
			if (GetHPFunc)
			{
				struct { float CurrentHP; float MaxHP; } HPInfo = { 0.0f, 0.0f };
				Character->ProcessEvent(GetHPFunc, &HPInfo);
				Result += FString::Printf(TEXT("HP: %.1f / %.1f\n"), HPInfo.CurrentHP, HPInfo.MaxHP);
			}

			return Result;
		}
	);

	// ShowPosition - Show only position info
	RegisterNativeCommand(
		TEXT("ShowPosition"),
		TEXT("Display current position in copy-paste format"),
		EDebugCommandCategory::Info,
		TEXT("ShowPosition"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			FVector Location = Character->GetActorLocation();
			return FString::Printf(TEXT("Position: %.1f %.1f %.1f\nUse: Debug Teleport %.1f %.1f %.1f"),
				Location.X, Location.Y, Location.Z,
				Location.X, Location.Y, Location.Z);
		}
	);
}

#endif // WITH_DEBUG_COMMANDS
