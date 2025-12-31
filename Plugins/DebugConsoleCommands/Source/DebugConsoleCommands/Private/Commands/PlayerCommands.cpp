// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandSubsystem.h"
#include "DebugConsoleCommandsModule.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

#if WITH_DEBUG_COMMANDS

namespace DebugCommands
{
	// Helper to get player character
	ACharacter* GetPlayerCharacter(UWorld* World)
	{
		if (!World) return nullptr;

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC) return nullptr;

		return PC->GetCharacter();
	}

	// Helper to get player pawn
	APawn* GetPlayerPawn(UWorld* World)
	{
		if (!World) return nullptr;

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC) return nullptr;

		return PC->GetPawn();
	}
}

void UDebugCommandSubsystem::RegisterPlayerCommands()
{
	// God - Toggle invincibility
	RegisterNativeCommand(
		TEXT("God"),
		TEXT("Toggle invincibility mode"),
		EDebugCommandCategory::Player,
		TEXT("God"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			// Toggle god mode using CanBeDamaged
			bool bCurrentlyDamageable = Character->CanBeDamaged();
			Character->SetCanBeDamaged(!bCurrentlyDamageable);

			return bCurrentlyDamageable ? TEXT("God mode enabled") : TEXT("God mode disabled");
		}
	);

	// Heal - Restore health
	RegisterNativeCommand(
		TEXT("Heal"),
		TEXT("Restore player health. If no amount specified, fully heals."),
		EDebugCommandCategory::Player,
		TEXT("Heal [Amount]"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			// Try to find and call a Heal function via interface or direct call
			// For now, we'll just report that the character was found
			// The actual healing depends on the game's health system implementation

			float Amount = 0.0f;
			bool bFullHeal = (Args.Num() == 0);

			if (!bFullHeal)
			{
				Amount = FCString::Atof(*Args[0]);
				if (Amount <= 0)
				{
					return TEXT("Invalid heal amount");
				}
			}

			// Call Blueprint-implementable heal through UFunction
			UFunction* HealFunc = Character->FindFunction(TEXT("DebugHeal"));
			if (HealFunc)
			{
				struct { float Amount; bool bFullHeal; } Params = { Amount, bFullHeal };
				Character->ProcessEvent(HealFunc, &Params);
				return bFullHeal ? TEXT("Fully healed") : FString::Printf(TEXT("Healed for %.1f"), Amount);
			}

			return bFullHeal ? TEXT("Full heal requested (implement DebugHeal function)")
			                 : FString::Printf(TEXT("Heal %.1f requested (implement DebugHeal function)"), Amount);
		}
	);

	// Kill - Kill player
	RegisterNativeCommand(
		TEXT("Kill"),
		TEXT("Kill the player character"),
		EDebugCommandCategory::Player,
		TEXT("Kill"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			// Try to call a Kill/Die function
			UFunction* KillFunc = Character->FindFunction(TEXT("DebugKill"));
			if (KillFunc)
			{
				Character->ProcessEvent(KillFunc, nullptr);
				return TEXT("Player killed");
			}

			// Alternative: Try TakeDamage with large amount
			Character->TakeDamage(999999.0f, FDamageEvent(), nullptr, nullptr);
			return TEXT("Player killed (via TakeDamage)");
		}
	);
}

#endif // WITH_DEBUG_COMMANDS
