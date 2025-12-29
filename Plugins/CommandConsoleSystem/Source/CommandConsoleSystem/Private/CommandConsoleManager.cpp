// Copyright Epic Games, Inc. All Rights Reserved.

#include "CommandConsoleManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UCommandConsoleManager::UCommandConsoleManager()
{
}

UCommandConsoleManager* UCommandConsoleManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UCommandConsoleManager* Manager = World->GetSubsystem<UCommandConsoleManager>();
	if (!Manager)
	{
		Manager = NewObject<UCommandConsoleManager>(World);
	}

	return Manager;
}

void UCommandConsoleManager::RegisterCommand(const FString& CommandName, const FString& Description, const FString& Usage, bool bDevBuildOnly, FConsoleCommandDelegate ExecuteDelegate)
{
	FConsoleCommandDefinition Definition;
	Definition.CommandName = CommandName;
	Definition.Description = Description;
	Definition.Usage = Usage;
	Definition.bDevBuildOnly = bDevBuildOnly;
	Definition.ExecuteDelegate = ExecuteDelegate;

	RegisteredCommands.Add(CommandName.ToLower(), Definition);
}

void UCommandConsoleManager::UnregisterCommand(const FString& CommandName)
{
	RegisteredCommands.Remove(CommandName.ToLower());
}

bool UCommandConsoleManager::ExecuteCommand(const FString& CommandLine)
{
	FString Command;
	TArray<FString> Args;
	ParseCommandLine(CommandLine, Command, Args);

	if (Command.IsEmpty())
	{
		return false;
	}

	FConsoleCommandDefinition* Definition = RegisteredCommands.Find(Command.ToLower());
	if (!Definition)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Unknown command: %s"), *Command));
		}
		return false;
	}

	// Check dev build restriction
	if (Definition->bDevBuildOnly && !IsDevBuild())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("This command is only available in development builds"));
		}
		return false;
	}

	// Execute command
	if (Definition->ExecuteDelegate.IsBound())
	{
		Definition->ExecuteDelegate.Execute(Args);
		return true;
	}

	return false;
}

TArray<FString> UCommandConsoleManager::GetAllCommands() const
{
	TArray<FString> Commands;
	RegisteredCommands.GetKeys(Commands);
	return Commands;
}

bool UCommandConsoleManager::GetCommandDefinition(const FString& CommandName, FConsoleCommandDefinition& OutDefinition) const
{
	const FConsoleCommandDefinition* Definition = RegisteredCommands.Find(CommandName.ToLower());
	if (Definition)
	{
		OutDefinition = *Definition;
		return true;
	}
	return false;
}

bool UCommandConsoleManager::IsCommandAvailable(const FString& CommandName) const
{
	const FConsoleCommandDefinition* Definition = RegisteredCommands.Find(CommandName.ToLower());
	if (!Definition)
	{
		return false;
	}

	// Check dev build restriction
	if (Definition->bDevBuildOnly && !IsDevBuild())
	{
		return false;
	}

	return true;
}

FString UCommandConsoleManager::GetCommandHelp(const FString& CommandName) const
{
	FConsoleCommandDefinition Definition;
	if (GetCommandDefinition(CommandName, Definition))
	{
		FString Help = FString::Printf(TEXT("%s: %s\nUsage: %s"),
			*Definition.CommandName,
			*Definition.Description,
			*Definition.Usage);

		if (Definition.bDevBuildOnly)
		{
			Help += TEXT("\n[Dev Build Only]");
		}

		return Help;
	}

	return TEXT("Command not found");
}

bool UCommandConsoleManager::IsDevBuild() const
{
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	return true;
#else
	return false;
#endif
}

void UCommandConsoleManager::ParseCommandLine(const FString& CommandLine, FString& OutCommand, TArray<FString>& OutArgs)
{
	OutCommand.Empty();
	OutArgs.Empty();

	FString TrimmedLine = CommandLine.TrimStartAndEnd();
	if (TrimmedLine.IsEmpty())
	{
		return;
	}

	// Split by spaces
	TArray<FString> Tokens;
	TrimmedLine.ParseIntoArray(Tokens, TEXT(" "), true);

	if (Tokens.Num() > 0)
	{
		OutCommand = Tokens[0];

		for (int32 i = 1; i < Tokens.Num(); i++)
		{
			OutArgs.Add(Tokens[i]);
		}
	}
}
