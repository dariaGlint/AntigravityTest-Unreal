// Copyright Epic Games, Inc. All Rights Reserved.

#include "DebugInfoSubsystem.h"
#include "CommandConsoleManager.h"
#include "Engine/Engine.h"
#include "HAL/PlatformMemory.h"
#include "DrawDebugHelpers.h"

void UDebugInfoSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bShowFPS = false;
	bShowMemory = false;
	bShowNetwork = false;
	bShowAI = false;
	bShowPhysics = false;
	bShowCollision = false;

	RegisterDebugCommands();
}

void UDebugInfoSubsystem::Deinitialize()
{
	UnregisterDebugCommands();

	Super::Deinitialize();
}

void UDebugInfoSubsystem::RegisterDebugCommands()
{
	UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
	if (!Manager)
	{
		return;
	}

	Manager->RegisterCommand(
		TEXT("showfps"),
		TEXT("Toggle FPS display"),
		TEXT("showfps [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleToggleFPS)
	);

	Manager->RegisterCommand(
		TEXT("showmemory"),
		TEXT("Toggle memory usage display"),
		TEXT("showmemory [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleToggleMemory)
	);

	Manager->RegisterCommand(
		TEXT("shownetwork"),
		TEXT("Toggle network stats display"),
		TEXT("shownetwork [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleToggleNetwork)
	);

	Manager->RegisterCommand(
		TEXT("showai"),
		TEXT("Toggle AI debug display"),
		TEXT("showai [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleToggleAI)
	);

	Manager->RegisterCommand(
		TEXT("showphysics"),
		TEXT("Toggle physics debug display"),
		TEXT("showphysics [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleTogglePhysics)
	);

	Manager->RegisterCommand(
		TEXT("showcollision"),
		TEXT("Toggle collision debug display"),
		TEXT("showcollision [on|off]"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleToggleCollision)
	);

	Manager->RegisterCommand(
		TEXT("showall"),
		TEXT("Show all debug information"),
		TEXT("showall"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleShowAll)
	);

	Manager->RegisterCommand(
		TEXT("hideall"),
		TEXT("Hide all debug information"),
		TEXT("hideall"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleHideAll)
	);

	Manager->RegisterCommand(
		TEXT("sysinfo"),
		TEXT("Log system information"),
		TEXT("sysinfo"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UDebugInfoSubsystem::HandleSystemInfo)
	);
}

void UDebugInfoSubsystem::UnregisterDebugCommands()
{
	UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
	if (!Manager)
	{
		return;
	}

	Manager->UnregisterCommand(TEXT("showfps"));
	Manager->UnregisterCommand(TEXT("showmemory"));
	Manager->UnregisterCommand(TEXT("shownetwork"));
	Manager->UnregisterCommand(TEXT("showai"));
	Manager->UnregisterCommand(TEXT("showphysics"));
	Manager->UnregisterCommand(TEXT("showcollision"));
	Manager->UnregisterCommand(TEXT("showall"));
	Manager->UnregisterCommand(TEXT("hideall"));
	Manager->UnregisterCommand(TEXT("sysinfo"));
}

void UDebugInfoSubsystem::ToggleFPS(bool bShow)
{
	bShowFPS = bShow;

	if (GEngine)
	{
		GEngine->bEnableOnScreenDebugMessages = true;

		if (bShow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("FPS Display: ON"));
			// Use stat FPS command
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("stat FPS"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("FPS Display: OFF"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("stat FPS"));
			}
		}
	}
}

void UDebugInfoSubsystem::ToggleMemoryInfo(bool bShow)
{
	bShowMemory = bShow;

	if (GEngine)
	{
		if (bShow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Memory Display: ON"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("stat Memory"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Memory Display: OFF"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("stat Memory"));
			}
		}
	}
}

void UDebugInfoSubsystem::ToggleNetworkStats(bool bShow)
{
	bShowNetwork = bShow;

	if (GEngine)
	{
		if (bShow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Network Stats: ON"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("stat Net"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Network Stats: OFF"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("stat Net"));
			}
		}
	}
}

void UDebugInfoSubsystem::ToggleAIDebug(bool bShow)
{
	bShowAI = bShow;

	if (GEngine)
	{
		if (bShow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("AI Debug: ON"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("ShowDebug AI"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("AI Debug: OFF"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("ShowDebug"));
			}
		}
	}
}

void UDebugInfoSubsystem::TogglePhysicsDebug(bool bShow)
{
	bShowPhysics = bShow;

	if (GEngine)
	{
		if (bShow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Physics Debug: ON"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("show Collision"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Physics Debug: OFF"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("show Collision"));
			}
		}
	}
}

void UDebugInfoSubsystem::ToggleCollisionDebug(bool bShow)
{
	bShowCollision = bShow;

	if (GEngine)
	{
		if (bShow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Collision Debug: ON"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("show CollisionPawn"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Collision Debug: OFF"));
			if (GEngine->GameViewport)
			{
				GEngine->GameViewport->ConsoleCommand(TEXT("show CollisionPawn"));
			}
		}
	}
}

void UDebugInfoSubsystem::ShowAllDebugInfo()
{
	ToggleFPS(true);
	ToggleMemoryInfo(true);
	ToggleNetworkStats(true);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("All debug info enabled"));
	}
}

void UDebugInfoSubsystem::HideAllDebugInfo()
{
	ToggleFPS(false);
	ToggleMemoryInfo(false);
	ToggleNetworkStats(false);
	ToggleAIDebug(false);
	TogglePhysicsDebug(false);
	ToggleCollisionDebug(false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("All debug info disabled"));
	}
}

void UDebugInfoSubsystem::LogSystemInfo()
{
	FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();

	UE_LOG(LogTemp, Log, TEXT("=== System Information ==="));
	UE_LOG(LogTemp, Log, TEXT("Platform: %s"), ANSI_TO_TCHAR(FPlatformProperties::IniPlatformName()));
	UE_LOG(LogTemp, Log, TEXT("CPU: %s"), *FPlatformMisc::GetCPUBrand());
	UE_LOG(LogTemp, Log, TEXT("GPU: %s"), *FPlatformMisc::GetPrimaryGPUBrand());
	UE_LOG(LogTemp, Log, TEXT("Memory: %.2f GB"), MemStats.TotalPhysical / (1024.0 * 1024.0 * 1024.0));
	UE_LOG(LogTemp, Log, TEXT("Available Memory: %.2f GB"), MemStats.AvailablePhysical / (1024.0 * 1024.0 * 1024.0));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("System information logged to console"));
	}
}

// Command handlers
void UDebugInfoSubsystem::HandleToggleFPS(const TArray<FString>& Args)
{
	bool bShow = !bShowFPS;

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bShow = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bShow = false;
		}
	}

	ToggleFPS(bShow);
}

void UDebugInfoSubsystem::HandleToggleMemory(const TArray<FString>& Args)
{
	bool bShow = !bShowMemory;

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bShow = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bShow = false;
		}
	}

	ToggleMemoryInfo(bShow);
}

void UDebugInfoSubsystem::HandleToggleNetwork(const TArray<FString>& Args)
{
	bool bShow = !bShowNetwork;

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bShow = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bShow = false;
		}
	}

	ToggleNetworkStats(bShow);
}

void UDebugInfoSubsystem::HandleToggleAI(const TArray<FString>& Args)
{
	bool bShow = !bShowAI;

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bShow = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bShow = false;
		}
	}

	ToggleAIDebug(bShow);
}

void UDebugInfoSubsystem::HandleTogglePhysics(const TArray<FString>& Args)
{
	bool bShow = !bShowPhysics;

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bShow = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bShow = false;
		}
	}

	TogglePhysicsDebug(bShow);
}

void UDebugInfoSubsystem::HandleToggleCollision(const TArray<FString>& Args)
{
	bool bShow = !bShowCollision;

	if (Args.Num() > 0)
	{
		if (Args[0].Equals(TEXT("on"), ESearchCase::IgnoreCase))
		{
			bShow = true;
		}
		else if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			bShow = false;
		}
	}

	ToggleCollisionDebug(bShow);
}

void UDebugInfoSubsystem::HandleShowAll(const TArray<FString>& Args)
{
	ShowAllDebugInfo();
}

void UDebugInfoSubsystem::HandleHideAll(const TArray<FString>& Args)
{
	HideAllDebugInfo();
}

void UDebugInfoSubsystem::HandleSystemInfo(const TArray<FString>& Args)
{
	LogSystemInfo();
}
