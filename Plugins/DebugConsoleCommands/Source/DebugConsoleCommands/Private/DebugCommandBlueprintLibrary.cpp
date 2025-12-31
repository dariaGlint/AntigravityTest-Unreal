// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandBlueprintLibrary.h"
#include "DebugCommandSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

bool UDebugCommandBlueprintLibrary::ExecuteDebugCommand(const UObject* WorldContextObject, const FString& CommandLine, FString& OutResult)
{
#if WITH_DEBUG_COMMANDS
	UDebugCommandSubsystem* Subsystem = UDebugCommandSubsystem::Get(WorldContextObject);
	if (Subsystem)
	{
		return Subsystem->ExecuteCommand(CommandLine, OutResult);
	}
	OutResult = TEXT("Debug command subsystem not available");
	return false;
#else
	OutResult = TEXT("Debug commands are not available in this build");
	return false;
#endif
}

bool UDebugCommandBlueprintLibrary::IsDebugCommandsEnabled()
{
#if WITH_DEBUG_COMMANDS
	return true;
#else
	return false;
#endif
}

TArray<FDebugCommand> UDebugCommandBlueprintLibrary::GetAllDebugCommands(const UObject* WorldContextObject)
{
#if WITH_DEBUG_COMMANDS
	UDebugCommandSubsystem* Subsystem = UDebugCommandSubsystem::Get(WorldContextObject);
	if (Subsystem)
	{
		return Subsystem->GetAllCommands();
	}
#endif
	return TArray<FDebugCommand>();
}

FString UDebugCommandBlueprintLibrary::GetCategoryDisplayName(EDebugCommandCategory Category)
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
