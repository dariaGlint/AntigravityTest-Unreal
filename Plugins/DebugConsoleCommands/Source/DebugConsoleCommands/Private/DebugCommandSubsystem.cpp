// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandSubsystem.h"
#include "DebugConsoleCommandsModule.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"

UDebugCommandSubsystem* UDebugCommandSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UDebugCommandSubsystem>();
}

void UDebugCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_DEBUG_COMMANDS
	UE_LOG(LogDebugCommands, Log, TEXT("DebugCommandSubsystem initializing..."));

	// Register console command
	ConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Debug"),
		TEXT("Execute a debug command. Use 'Debug ListCommands' to see available commands."),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateLambda(
			[this](const TArray<FString>& Args, UWorld* World, FOutputDevice& Ar)
			{
				if (Args.Num() == 0)
				{
					Ar.Log(TEXT("Usage: Debug <command> [args...]"));
					Ar.Log(TEXT("Use 'Debug ListCommands' to see available commands."));
					return;
				}

				FString CommandLine = FString::Join(Args, TEXT(" "));
				FString Result;
				if (ExecuteCommand(CommandLine, Result))
				{
					Ar.Log(*Result);
				}
				else
				{
					Ar.Log(*FString::Printf(TEXT("Unknown command: %s. Use 'Debug ListCommands' for available commands."), *Args[0]));
				}
			}
		),
		ECVF_Cheat
	);

	// Register built-in commands
	RegisterBuiltInCommands();

	UE_LOG(LogDebugCommands, Log, TEXT("DebugCommandSubsystem initialized with %d commands"), Commands.Num());
#endif
}

void UDebugCommandSubsystem::Deinitialize()
{
#if WITH_DEBUG_COMMANDS
	// Unregister console command
	if (ConsoleCommand)
	{
		IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);
		ConsoleCommand = nullptr;
	}

	Commands.Empty();
	UE_LOG(LogDebugCommands, Log, TEXT("DebugCommandSubsystem deinitialized"));
#endif

	Super::Deinitialize();
}

bool UDebugCommandSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_DEBUG_COMMANDS
	return true;
#else
	return false;
#endif
}

void UDebugCommandSubsystem::RegisterCommand(
	const FString& Name,
	const FString& Description,
	EDebugCommandCategory Category,
	const FString& Usage,
	const FDebugCommandDelegate& ExecuteDelegate)
{
#if WITH_DEBUG_COMMANDS
	FDebugCommand Command;
	Command.Name = Name;
	Command.Description = Description;
	Command.Category = Category;
	Command.Usage = Usage;
	Command.ExecuteDelegate = ExecuteDelegate;

	Commands.Add(Name.ToLower(), Command);
	UE_LOG(LogDebugCommands, Verbose, TEXT("Registered command: %s"), *Name);
#endif
}

void UDebugCommandSubsystem::RegisterNativeCommand(
	const FString& Name,
	const FString& Description,
	EDebugCommandCategory Category,
	const FString& Usage,
	TFunction<FString(const TArray<FString>&, UWorld*)> ExecuteFunc)
{
#if WITH_DEBUG_COMMANDS
	FDebugCommand Command(Name, Description, Category, Usage, ExecuteFunc);
	Commands.Add(Name.ToLower(), Command);
	UE_LOG(LogDebugCommands, Verbose, TEXT("Registered native command: %s"), *Name);
#endif
}

void UDebugCommandSubsystem::UnregisterCommand(const FString& Name)
{
#if WITH_DEBUG_COMMANDS
	Commands.Remove(Name.ToLower());
	UE_LOG(LogDebugCommands, Verbose, TEXT("Unregistered command: %s"), *Name);
#endif
}

bool UDebugCommandSubsystem::ExecuteCommand(const FString& CommandLine, FString& OutResult)
{
#if WITH_DEBUG_COMMANDS
	// Parse command line
	TArray<FString> Tokens;
	CommandLine.ParseIntoArrayWS(Tokens);

	if (Tokens.Num() == 0)
	{
		OutResult = TEXT("No command specified");
		return false;
	}

	FString CommandName = Tokens[0].ToLower();
	TArray<FString> Args;
	for (int32 i = 1; i < Tokens.Num(); ++i)
	{
		Args.Add(Tokens[i]);
	}

	// Find and execute command
	FDebugCommand* Command = Commands.Find(CommandName);
	if (!Command)
	{
		OutResult = FString::Printf(TEXT("Unknown command: %s"), *Tokens[0]);
		return false;
	}

	// Get world
	UWorld* World = GetGameInstance()->GetWorld();

	// Execute native command
	if (Command->NativeExecute)
	{
		OutResult = Command->NativeExecute(Args, World);
		return true;
	}

	// Execute delegate command
	if (Command->ExecuteDelegate.IsBound())
	{
		Command->ExecuteDelegate.Execute(Args, OutResult);
		return true;
	}

	OutResult = TEXT("Command has no execution handler");
	return false;
#else
	OutResult = TEXT("Debug commands are not available in this build");
	return false;
#endif
}

TArray<FDebugCommand> UDebugCommandSubsystem::GetAllCommands() const
{
	TArray<FDebugCommand> Result;
#if WITH_DEBUG_COMMANDS
	Commands.GenerateValueArray(Result);
#endif
	return Result;
}

TArray<FDebugCommand> UDebugCommandSubsystem::GetCommandsByCategory(EDebugCommandCategory Category) const
{
	TArray<FDebugCommand> Result;
#if WITH_DEBUG_COMMANDS
	for (const auto& Pair : Commands)
	{
		if (Pair.Value.Category == Category)
		{
			Result.Add(Pair.Value);
		}
	}
#endif
	return Result;
}

bool UDebugCommandSubsystem::GetCommand(const FString& Name, FDebugCommand& OutCommand) const
{
#if WITH_DEBUG_COMMANDS
	const FDebugCommand* Command = Commands.Find(Name.ToLower());
	if (Command)
	{
		OutCommand = *Command;
		return true;
	}
#endif
	return false;
}

bool UDebugCommandSubsystem::AreDebugCommandsEnabled()
{
#if WITH_DEBUG_COMMANDS
	return true;
#else
	return false;
#endif
}

bool UDebugCommandSubsystem::ProcessConsoleCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* World)
{
#if WITH_DEBUG_COMMANDS
	FString Result;
	if (ExecuteCommand(Cmd, Result))
	{
		Ar.Log(*Result);
		return true;
	}
#endif
	return false;
}

void UDebugCommandSubsystem::RegisterBuiltInCommands()
{
#if WITH_DEBUG_COMMANDS
	RegisterPlayerCommands();
	RegisterMovementCommands();
	RegisterCombatCommands();
	RegisterSpawnCommands();
	RegisterSystemCommands();
	RegisterInfoCommands();
#endif
}
