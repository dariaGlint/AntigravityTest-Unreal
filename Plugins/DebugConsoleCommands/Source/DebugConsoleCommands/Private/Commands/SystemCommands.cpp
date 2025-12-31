// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandSubsystem.h"
#include "DebugConsoleCommandsModule.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"

#if WITH_DEBUG_COMMANDS

void UDebugCommandSubsystem::RegisterSystemCommands()
{
	// SlowMo - Change game speed
	RegisterNativeCommand(
		TEXT("SlowMo"),
		TEXT("Set game time dilation (slow motion)"),
		EDebugCommandCategory::System,
		TEXT("SlowMo <Scale> (0.1 - 2.0)"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: SlowMo <Scale> (0.1 - 2.0)");
			}

			if (!World)
			{
				return TEXT("No world found");
			}

			float Scale = FCString::Atof(*Args[0]);
			Scale = FMath::Clamp(Scale, 0.1f, 2.0f);

			AWorldSettings* WorldSettings = World->GetWorldSettings();
			if (WorldSettings)
			{
				WorldSettings->TimeDilation = Scale;
				return FString::Printf(TEXT("Time dilation set to %.2fx"), Scale);
			}

			return TEXT("Failed to set time dilation");
		}
	);

	// Pause - Toggle pause
	RegisterNativeCommand(
		TEXT("Pause"),
		TEXT("Toggle game pause"),
		EDebugCommandCategory::System,
		TEXT("Pause"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			APlayerController* PC = World->GetFirstPlayerController();
			if (!PC)
			{
				return TEXT("No player controller found");
			}

			bool bIsPaused = UGameplayStatics::IsGamePaused(World);
			UGameplayStatics::SetGamePaused(World, !bIsPaused);

			return bIsPaused ? TEXT("Game unpaused") : TEXT("Game paused");
		}
	);

	// RestartLevel - Restart current level
	RegisterNativeCommand(
		TEXT("RestartLevel"),
		TEXT("Restart the current level"),
		EDebugCommandCategory::System,
		TEXT("RestartLevel"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			FString CurrentLevel = World->GetMapName();
			UGameplayStatics::OpenLevel(World, FName(*CurrentLevel));

			return FString::Printf(TEXT("Restarting level: %s"), *CurrentLevel);
		}
	);

	// OpenLevel - Open a specific level
	RegisterNativeCommand(
		TEXT("OpenLevel"),
		TEXT("Open a specific level by name"),
		EDebugCommandCategory::System,
		TEXT("OpenLevel <LevelName>"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: OpenLevel <LevelName>");
			}

			if (!World)
			{
				return TEXT("No world found");
			}

			FString LevelName = Args[0];
			UGameplayStatics::OpenLevel(World, FName(*LevelName));

			return FString::Printf(TEXT("Opening level: %s"), *LevelName);
		}
	);

	// ShowFPS - Toggle FPS display
	RegisterNativeCommand(
		TEXT("ShowFPS"),
		TEXT("Toggle FPS display"),
		EDebugCommandCategory::System,
		TEXT("ShowFPS"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			// Use console command to toggle stat fps
			APlayerController* PC = World->GetFirstPlayerController();
			if (PC)
			{
				PC->ConsoleCommand(TEXT("stat fps"));
				return TEXT("FPS display toggled");
			}

			return TEXT("No player controller found");
		}
	);

	// ShowStats - Toggle stat unit display
	RegisterNativeCommand(
		TEXT("ShowUnit"),
		TEXT("Toggle stat unit display (frame time, game time, etc.)"),
		EDebugCommandCategory::System,
		TEXT("ShowUnit"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			APlayerController* PC = World->GetFirstPlayerController();
			if (PC)
			{
				PC->ConsoleCommand(TEXT("stat unit"));
				return TEXT("Unit stats display toggled");
			}

			return TEXT("No player controller found");
		}
	);
}

#endif // WITH_DEBUG_COMMANDS
