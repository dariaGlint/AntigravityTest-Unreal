// Copyright Antigravity. All Rights Reserved.

#include "DebugConsoleCommandsModule.h"

#define LOCTEXT_NAMESPACE "FDebugConsoleCommandsModule"

DEFINE_LOG_CATEGORY(LogDebugCommands);

void FDebugConsoleCommandsModule::StartupModule()
{
#if WITH_DEBUG_COMMANDS
	UE_LOG(LogDebugCommands, Log, TEXT("DebugConsoleCommands module started"));
#endif
}

void FDebugConsoleCommandsModule::ShutdownModule()
{
#if WITH_DEBUG_COMMANDS
	UE_LOG(LogDebugCommands, Log, TEXT("DebugConsoleCommands module shutdown"));
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDebugConsoleCommandsModule, DebugConsoleCommands)
