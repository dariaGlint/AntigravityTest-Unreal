// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandSubsystem.h"
#include "DebugConsoleCommandsModule.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "BrainComponent.h"

#if WITH_DEBUG_COMMANDS

namespace DebugCommands
{
	extern ACharacter* GetPlayerCharacter(UWorld* World);
}

void UDebugCommandSubsystem::RegisterCombatCommands()
{
	// SetHP - Set player health
	RegisterNativeCommand(
		TEXT("SetHP"),
		TEXT("Set player's current HP"),
		EDebugCommandCategory::Combat,
		TEXT("SetHP <Amount>"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: SetHP <Amount>");
			}

			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			float Amount = FCString::Atof(*Args[0]);
			if (Amount < 0)
			{
				return TEXT("HP amount must be non-negative");
			}

			// Try to call a SetHP function via UFunction
			UFunction* SetHPFunc = Character->FindFunction(TEXT("DebugSetHP"));
			if (SetHPFunc)
			{
				struct { float HP; } Params = { Amount };
				Character->ProcessEvent(SetHPFunc, &Params);
				return FString::Printf(TEXT("HP set to %.1f"), Amount);
			}

			return FString::Printf(TEXT("SetHP %.1f requested (implement DebugSetHP function)"), Amount);
		}
	);

	// SetMaxHP - Set player max health
	RegisterNativeCommand(
		TEXT("SetMaxHP"),
		TEXT("Set player's maximum HP"),
		EDebugCommandCategory::Combat,
		TEXT("SetMaxHP <Amount>"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: SetMaxHP <Amount>");
			}

			ACharacter* Character = DebugCommands::GetPlayerCharacter(World);
			if (!Character)
			{
				return TEXT("No player character found");
			}

			float Amount = FCString::Atof(*Args[0]);
			if (Amount <= 0)
			{
				return TEXT("MaxHP must be greater than 0");
			}

			// Try to call a SetMaxHP function via UFunction
			UFunction* SetMaxHPFunc = Character->FindFunction(TEXT("DebugSetMaxHP"));
			if (SetMaxHPFunc)
			{
				struct { float MaxHP; } Params = { Amount };
				Character->ProcessEvent(SetMaxHPFunc, &Params);
				return FString::Printf(TEXT("MaxHP set to %.1f"), Amount);
			}

			return FString::Printf(TEXT("SetMaxHP %.1f requested (implement DebugSetMaxHP function)"), Amount);
		}
	);

	// KillAllEnemies - Kill all enemies
	RegisterNativeCommand(
		TEXT("KillAllEnemies"),
		TEXT("Kill all enemy characters in the level"),
		EDebugCommandCategory::Combat,
		TEXT("KillAllEnemies"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			int32 KillCount = 0;

			// Find all characters with AI controllers (likely enemies)
			for (TActorIterator<ACharacter> It(World); It; ++It)
			{
				ACharacter* Character = *It;

				// Skip player-controlled characters
				if (Character->IsPlayerControlled())
				{
					continue;
				}

				// Check if it has an AI controller
				AAIController* AIController = Cast<AAIController>(Character->GetController());
				if (AIController)
				{
					// Try to call a Kill function
					UFunction* KillFunc = Character->FindFunction(TEXT("DebugKill"));
					if (KillFunc)
					{
						Character->ProcessEvent(KillFunc, nullptr);
					}
					else
					{
						// Use TakeDamage as fallback
						Character->TakeDamage(999999.0f, FDamageEvent(), nullptr, nullptr);
					}
					KillCount++;
				}
			}

			return FString::Printf(TEXT("Killed %d enemies"), KillCount);
		}
	);

	// ToggleAI - Toggle AI enabled/disabled
	RegisterNativeCommand(
		TEXT("ToggleAI"),
		TEXT("Toggle AI behavior on/off for all AI characters"),
		EDebugCommandCategory::Combat,
		TEXT("ToggleAI"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			static bool bAIEnabled = true;
			bAIEnabled = !bAIEnabled;

			int32 AffectedCount = 0;

			// Find all AI controllers
			for (TActorIterator<AAIController> It(World); It; ++It)
			{
				AAIController* AIController = *It;
				if (AIController)
				{
					UBrainComponent* BrainComp = AIController->GetBrainComponent();
					if (BrainComp)
					{
						if (bAIEnabled)
						{
							BrainComp->RestartLogic();
						}
						else
						{
							BrainComp->StopLogic(TEXT("Debug command"));
						}
						AffectedCount++;
					}
				}
			}

			return FString::Printf(TEXT("AI %s (%d controllers affected)"),
				bAIEnabled ? TEXT("enabled") : TEXT("disabled"),
				AffectedCount);
		}
	);
}

#endif // WITH_DEBUG_COMMANDS
