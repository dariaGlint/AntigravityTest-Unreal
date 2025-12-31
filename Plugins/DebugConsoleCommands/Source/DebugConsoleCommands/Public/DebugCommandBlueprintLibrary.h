// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugCommand.h"
#include "DebugCommandBlueprintLibrary.generated.h"

/**
 * Blueprint function library for debug commands
 */
UCLASS()
class DEBUGCONSOLECOMMANDS_API UDebugCommandBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Execute a debug command
	 * @param WorldContextObject World context
	 * @param CommandLine Full command line to execute
	 * @param OutResult Result message from command
	 * @return True if command was found and executed
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands", meta = (WorldContext = "WorldContextObject"))
	static bool ExecuteDebugCommand(const UObject* WorldContextObject, const FString& CommandLine, FString& OutResult);

	/**
	 * Check if debug commands are enabled in this build
	 */
	UFUNCTION(BlueprintPure, Category = "Debug Commands")
	static bool IsDebugCommandsEnabled();

	/**
	 * Get all available debug commands
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands", meta = (WorldContext = "WorldContextObject"))
	static TArray<FDebugCommand> GetAllDebugCommands(const UObject* WorldContextObject);

	/**
	 * Get the category display name
	 */
	UFUNCTION(BlueprintPure, Category = "Debug Commands")
	static FString GetCategoryDisplayName(EDebugCommandCategory Category);
};
