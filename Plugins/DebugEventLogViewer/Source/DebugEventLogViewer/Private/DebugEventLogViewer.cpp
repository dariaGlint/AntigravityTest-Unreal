#include "DebugEventLogViewer.h"
#include "EventLogConsoleCommands.h"

#define LOCTEXT_NAMESPACE "FDebugEventLogViewerModule"

void FDebugEventLogViewerModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("DebugEventLogViewer module has been loaded"));

	// Register console commands
	FEventLogConsoleCommands::Register();
}

void FDebugEventLogViewerModule::ShutdownModule()
{
	// Unregister console commands
	FEventLogConsoleCommands::Unregister();

	UE_LOG(LogTemp, Log, TEXT("DebugEventLogViewer module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDebugEventLogViewerModule, DebugEventLogViewer)
