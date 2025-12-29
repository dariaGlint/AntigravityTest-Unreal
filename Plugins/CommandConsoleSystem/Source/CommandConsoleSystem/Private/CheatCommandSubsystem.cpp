// Copyright Epic Games, Inc. All Rights Reserved.

#include "CheatCommandSubsystem.h"
#include "CommandConsoleManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UCheatCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bGodModeEnabled = false;
	bNoClipEnabled = false;

	RegisterCheatCommands();
}

void UCheatCommandSubsystem::Deinitialize()
{
	UnregisterCheatCommands();

	Super::Deinitialize();
}

void UCheatCommandSubsystem::RegisterCheatCommands()
{
	UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
	if (!Manager)
	{
		return;
	}

	// Register all cheat commands (all are dev build only)
	Manager->RegisterCommand(
		TEXT("god"),
		TEXT("Toggle god mode (invincibility)"),
		TEXT("god [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleGodMode)
	);

	Manager->RegisterCommand(
		TEXT("noclip"),
		TEXT("Toggle no clip mode (fly through walls)"),
		TEXT("noclip [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleNoClip)
	);

	Manager->RegisterCommand(
		TEXT("teleport"),
		TEXT("Teleport to coordinates"),
		TEXT("teleport <x> <y> <z>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleTeleport)
	);

	Manager->RegisterCommand(
		TEXT("give"),
		TEXT("Add item to inventory"),
		TEXT("give <item_name> [quantity]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleGiveItem)
	);

	Manager->RegisterCommand(
		TEXT("sethealth"),
		TEXT("Set player health"),
		TEXT("sethealth <value>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleSetHealth)
	);

	Manager->RegisterCommand(
		TEXT("killenemies"),
		TEXT("Kill all enemies"),
		TEXT("killenemies"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleKillAllEnemies)
	);

	Manager->RegisterCommand(
		TEXT("timescale"),
		TEXT("Set game time scale"),
		TEXT("timescale <value>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleSetTimeScale)
	);

	Manager->RegisterCommand(
		TEXT("unlockall"),
		TEXT("Unlock all content"),
		TEXT("unlockall"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UCheatCommandSubsystem::HandleUnlockAll)
	);
}

void UCheatCommandSubsystem::UnregisterCheatCommands()
{
	UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
	if (!Manager)
	{
		return;
	}

	Manager->UnregisterCommand(TEXT("god"));
	Manager->UnregisterCommand(TEXT("noclip"));
	Manager->UnregisterCommand(TEXT("teleport"));
	Manager->UnregisterCommand(TEXT("give"));
	Manager->UnregisterCommand(TEXT("sethealth"));
	Manager->UnregisterCommand(TEXT("killenemies"));
	Manager->UnregisterCommand(TEXT("timescale"));
	Manager->UnregisterCommand(TEXT("unlockall"));
}

void UCheatCommandSubsystem::GodMode(bool bEnable)
{
	bGodModeEnabled = bEnable;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
			FString::Printf(TEXT("God Mode: %s"), bEnable ? TEXT("ON") : TEXT("OFF")));
	}

	// TODO: Implement actual god mode logic in your game's damage system
}

void UCheatCommandSubsystem::NoClip(bool bEnable)
{
	bNoClipEnabled = bEnable;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
	{
		if (bEnable)
		{
			PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
		else
		{
			PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
			FString::Printf(TEXT("No Clip: %s"), bEnable ? TEXT("ON") : TEXT("OFF")));
	}
}

void UCheatCommandSubsystem::Teleport(float X, float Y, float Z)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter)
	{
		FVector NewLocation(X, Y, Z);
		PlayerCharacter->SetActorLocation(NewLocation);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
				FString::Printf(TEXT("Teleported to: %.0f, %.0f, %.0f"), X, Y, Z));
		}
	}
}

void UCheatCommandSubsystem::GiveItem(const FString& ItemName, int32 Quantity)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
			FString::Printf(TEXT("Gave %d x %s"), Quantity, *ItemName));
	}

	// TODO: Implement actual inventory system integration
}

void UCheatCommandSubsystem::SetHealth(float Health)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
			FString::Printf(TEXT("Health set to: %.0f"), Health));
	}

	// TODO: Implement actual health system integration
}

void UCheatCommandSubsystem::KillAllEnemies()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
			TEXT("All enemies killed"));
	}

	// TODO: Implement actual enemy killing logic
}

void UCheatCommandSubsystem::SetTimeScale(float TimeScale)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(TimeScale);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
				FString::Printf(TEXT("Time scale set to: %.2f"), TimeScale));
		}
	}
}

void UCheatCommandSubsystem::UnlockAll()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
			TEXT("All content unlocked"));
	}

	// TODO: Implement actual unlock logic
}

// Command handlers
void UCheatCommandSubsystem::HandleGodMode(const TArray<FString>& Args)
{
	bool bEnable = !bGodModeEnabled; // Toggle by default

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bEnable = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bEnable = false;
		}
	}

	GodMode(bEnable);
}

void UCheatCommandSubsystem::HandleNoClip(const TArray<FString>& Args)
{
	bool bEnable = !bNoClipEnabled; // Toggle by default

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bEnable = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bEnable = false;
		}
	}

	NoClip(bEnable);
}

void UCheatCommandSubsystem::HandleTeleport(const TArray<FString>& Args)
{
	if (Args.Num() < 3)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: teleport <x> <y> <z>"));
		}
		return;
	}

	float X = FCString::Atof(*Args[0]);
	float Y = FCString::Atof(*Args[1]);
	float Z = FCString::Atof(*Args[2]);

	Teleport(X, Y, Z);
}

void UCheatCommandSubsystem::HandleGiveItem(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: give <item_name> [quantity]"));
		}
		return;
	}

	FString ItemName = Args[0];
	int32 Quantity = 1;

	if (Args.Num() > 1)
	{
		Quantity = FCString::Atoi(*Args[1]);
	}

	GiveItem(ItemName, Quantity);
}

void UCheatCommandSubsystem::HandleSetHealth(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: sethealth <value>"));
		}
		return;
	}

	float Health = FCString::Atof(*Args[0]);
	SetHealth(Health);
}

void UCheatCommandSubsystem::HandleKillAllEnemies(const TArray<FString>& Args)
{
	KillAllEnemies();
}

void UCheatCommandSubsystem::HandleSetTimeScale(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: timescale <value>"));
		}
		return;
	}

	float TimeScale = FCString::Atof(*Args[0]);
	SetTimeScale(TimeScale);
}

void UCheatCommandSubsystem::HandleUnlockAll(const TArray<FString>& Args)
{
	UnlockAll();
}
