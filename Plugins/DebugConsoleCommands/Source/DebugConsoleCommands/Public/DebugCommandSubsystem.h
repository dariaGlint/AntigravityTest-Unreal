// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugCommand.h"
#include "DebugCommandSubsystem.generated.h"

/**
 * Subsystem for managing debug console commands
 */
UCLASS()
class DEBUGCONSOLECOMMANDS_API UDebugCommandSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Get the subsystem from world context */
	UFUNCTION(BlueprintPure, Category = "Debug Commands", meta = (WorldContext = "WorldContextObject"))
	static UDebugCommandSubsystem* Get(const UObject* WorldContextObject);

	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	/**
	 * Register a command with native C++ execution
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands")
	void RegisterCommand(
		const FString& Name,
		const FString& Description,
		EDebugCommandCategory Category,
		const FString& Usage,
		const FDebugCommandDelegate& ExecuteDelegate);

	/**
	 * Register a native C++ command
	 */
	void RegisterNativeCommand(
		const FString& Name,
		const FString& Description,
		EDebugCommandCategory Category,
		const FString& Usage,
		TFunction<FString(const TArray<FString>&, UWorld*)> ExecuteFunc);

	/**
	 * Unregister a command
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands")
	void UnregisterCommand(const FString& Name);

	/**
	 * Execute a command by command line
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands")
	bool ExecuteCommand(const FString& CommandLine, FString& OutResult);

	/**
	 * Get all registered commands
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands")
	TArray<FDebugCommand> GetAllCommands() const;

	/**
	 * Get commands by category
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands")
	TArray<FDebugCommand> GetCommandsByCategory(EDebugCommandCategory Category) const;

	/**
	 * Get a command by name
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Commands")
	bool GetCommand(const FString& Name, FDebugCommand& OutCommand) const;

	/**
	 * Check if debug commands are enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Debug Commands")
	static bool AreDebugCommandsEnabled();

	/**
	 * Process console command (called from CheatManager or Exec)
	 */
	bool ProcessConsoleCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* World);

protected:
	/** Register all built-in commands */
	void RegisterBuiltInCommands();

	/** Register player commands */
	void RegisterPlayerCommands();

	/** Register movement commands */
	void RegisterMovementCommands();

	/** Register combat commands */
	void RegisterCombatCommands();

	/** Register spawn commands */
	void RegisterSpawnCommands();

	/** Register system commands */
	void RegisterSystemCommands();

	/** Register info/help commands */
	void RegisterInfoCommands();

private:
	/** Registered commands */
	UPROPERTY()
	TMap<FString, FDebugCommand> Commands;

	/** Console command handle */
	IConsoleObject* ConsoleCommand = nullptr;
};
