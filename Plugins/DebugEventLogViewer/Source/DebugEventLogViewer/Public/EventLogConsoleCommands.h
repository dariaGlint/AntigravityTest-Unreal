#pragma once

#include "CoreMinimal.h"

/**
 * Console commands for Debug Event Log
 */
class DEBUGEVENTLOGVIEWER_API FEventLogConsoleCommands
{
public:
	/** Register console commands */
	static void Register();

	/** Unregister console commands */
	static void Unregister();

private:
	/** Show event log widget */
	static void ShowEventLog(const TArray<FString>& Args, UWorld* World);

	/** Export event log to file */
	static void ExportEventLog(const TArray<FString>& Args, UWorld* World);

	/** Clear event log */
	static void ClearEventLog(const TArray<FString>& Args, UWorld* World);

	/** Filter event log */
	static void FilterEventLog(const TArray<FString>& Args, UWorld* World);

	/** Set max events */
	static void SetMaxEvents(const TArray<FString>& Args, UWorld* World);

	static TArray<IConsoleObject*> ConsoleCommands;
};
