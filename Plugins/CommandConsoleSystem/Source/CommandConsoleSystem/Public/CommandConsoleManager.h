// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommandConsoleManager.generated.h"

DECLARE_DELEGATE_OneParam(FConsoleCommandDelegate, const TArray<FString>& /*Args*/);

/**
 * Console command definition
 */
USTRUCT(BlueprintType)
struct FConsoleCommandDefinition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString CommandName;

	UPROPERTY(BlueprintReadOnly)
	FString Description;

	UPROPERTY(BlueprintReadOnly)
	FString Usage;

	UPROPERTY(BlueprintReadOnly)
	bool bDevBuildOnly;

	FConsoleCommandDelegate ExecuteDelegate;

	FConsoleCommandDefinition()
		: bDevBuildOnly(false)
	{}
};

/**
 * Main manager for command console system
 */
UCLASS(BlueprintType)
class COMMANDCONSOLESYSTEM_API UCommandConsoleManager : public UObject
{
	GENERATED_BODY()

public:
	UCommandConsoleManager();

	/**
	 * Get singleton instance
	 */
	UFUNCTION(BlueprintPure, Category = "Console", meta = (WorldContext = "WorldContextObject"))
	static UCommandConsoleManager* Get(const UObject* WorldContextObject);

	/**
	 * Register a new console command
	 */
	void RegisterCommand(const FString& CommandName, const FString& Description, const FString& Usage, bool bDevBuildOnly, FConsoleCommandDelegate ExecuteDelegate);

	/**
	 * Unregister a console command
	 */
	UFUNCTION(BlueprintCallable, Category = "Console")
	void UnregisterCommand(const FString& CommandName);

	/**
	 * Execute a console command
	 */
	UFUNCTION(BlueprintCallable, Category = "Console")
	bool ExecuteCommand(const FString& CommandLine);

	/**
	 * Get list of all registered commands
	 */
	UFUNCTION(BlueprintPure, Category = "Console")
	TArray<FString> GetAllCommands() const;

	/**
	 * Get command definition
	 */
	bool GetCommandDefinition(const FString& CommandName, FConsoleCommandDefinition& OutDefinition) const;

	/**
	 * Check if command is available (considering dev build restrictions)
	 */
	UFUNCTION(BlueprintPure, Category = "Console")
	bool IsCommandAvailable(const FString& CommandName) const;

	/**
	 * Get command help text
	 */
	UFUNCTION(BlueprintPure, Category = "Console")
	FString GetCommandHelp(const FString& CommandName) const;

private:
	UPROPERTY()
	TMap<FString, FConsoleCommandDefinition> RegisteredCommands;

	bool IsDevBuild() const;
	void ParseCommandLine(const FString& CommandLine, FString& OutCommand, TArray<FString>& OutArgs);
};
