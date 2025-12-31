#include "EventLogConsoleCommands.h"
#include "EventLogSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

TArray<IConsoleObject*> FEventLogConsoleCommands::ConsoleCommands;

void FEventLogConsoleCommands::Register()
{
#if WITH_DEBUG_EVENT_LOG
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Debug.ShowEventLog"),
		TEXT("Show or hide the debug event log widget"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FEventLogConsoleCommands::ShowEventLog),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Debug.ExportEventLog"),
		TEXT("Export event log to file. Usage: Debug.ExportEventLog [CSV|JSON]"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FEventLogConsoleCommands::ExportEventLog),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Debug.ClearEventLog"),
		TEXT("Clear all events from the event log"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FEventLogConsoleCommands::ClearEventLog),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Debug.FilterEventLog"),
		TEXT("Filter event log by category. Usage: Debug.FilterEventLog [Combat|State|AI|System]"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FEventLogConsoleCommands::FilterEventLog),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Debug.SetMaxEvents"),
		TEXT("Set maximum number of events to store. Usage: Debug.SetMaxEvents <number>"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FEventLogConsoleCommands::SetMaxEvents),
		ECVF_Default
	));

	UE_LOG(LogTemp, Log, TEXT("EventLog console commands registered"));
#endif
}

void FEventLogConsoleCommands::Unregister()
{
	for (IConsoleObject* Command : ConsoleCommands)
	{
		IConsoleManager::Get().UnregisterConsoleObject(Command);
	}
	ConsoleCommands.Empty();
}

void FEventLogConsoleCommands::ShowEventLog(const TArray<FString>& Args, UWorld* World)
{
#if WITH_DEBUG_EVENT_LOG
	if (!World)
	{
		return;
	}

	// This would typically show the widget
	// In practice, you'd want to create or toggle the widget here
	// For now, just log the command
	UE_LOG(LogTemp, Log, TEXT("ShowEventLog command called - implement widget toggle in your game code"));
#endif
}

void FEventLogConsoleCommands::ExportEventLog(const TArray<FString>& Args, UWorld* World)
{
#if WITH_DEBUG_EVENT_LOG
	if (!World || !World->GetGameInstance())
	{
		return;
	}

	UEventLogSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UEventLogSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("EventLogSubsystem not found"));
		return;
	}

	FString Format = Args.Num() > 0 ? Args[0] : TEXT("CSV");
	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	const FString FileName = FString::Printf(TEXT("EventLog_%s.%s"), *Timestamp, *Format.ToLower());
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("Logs") / FileName;

	bool bSuccess = false;
	if (Format.Equals(TEXT("JSON"), ESearchCase::IgnoreCase))
	{
		bSuccess = Subsystem->ExportToJSON(FilePath);
	}
	else
	{
		bSuccess = Subsystem->ExportToCSV(FilePath);
	}

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Exported event log to: %s"), *FilePath);
	}
#endif
}

void FEventLogConsoleCommands::ClearEventLog(const TArray<FString>& Args, UWorld* World)
{
#if WITH_DEBUG_EVENT_LOG
	if (!World || !World->GetGameInstance())
	{
		return;
	}

	UEventLogSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UEventLogSubsystem>();
	if (Subsystem)
	{
		Subsystem->ClearEvents();
		UE_LOG(LogTemp, Log, TEXT("Event log cleared"));
	}
#endif
}

void FEventLogConsoleCommands::FilterEventLog(const TArray<FString>& Args, UWorld* World)
{
#if WITH_DEBUG_EVENT_LOG
	if (!World || !World->GetGameInstance())
	{
		return;
	}

	if (Args.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: Debug.FilterEventLog [Combat|State|AI|System]"));
		return;
	}

	// This would set filter in the widget
	// For now, just log
	UE_LOG(LogTemp, Log, TEXT("FilterEventLog command called with filter: %s"), *Args[0]);
#endif
}

void FEventLogConsoleCommands::SetMaxEvents(const TArray<FString>& Args, UWorld* World)
{
#if WITH_DEBUG_EVENT_LOG
	if (!World || !World->GetGameInstance())
	{
		return;
	}

	if (Args.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: Debug.SetMaxEvents <number>"));
		return;
	}

	const int32 MaxEvents = FCString::Atoi(*Args[0]);

	UEventLogSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UEventLogSubsystem>();
	if (Subsystem)
	{
		Subsystem->SetMaxEvents(MaxEvents);
		UE_LOG(LogTemp, Log, TEXT("Max events set to: %d"), MaxEvents);
	}
#endif
}
