// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DebugCommand.generated.h"

/**
 * Debug command category
 */
UENUM(BlueprintType)
enum class EDebugCommandCategory : uint8
{
	Player		UMETA(DisplayName = "Player"),
	Movement	UMETA(DisplayName = "Movement"),
	Combat		UMETA(DisplayName = "Combat"),
	Spawn		UMETA(DisplayName = "Spawn"),
	System		UMETA(DisplayName = "System"),
	Info		UMETA(DisplayName = "Info"),
	Custom		UMETA(DisplayName = "Custom")
};

/**
 * Delegate for command execution
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDebugCommandDelegate, const TArray<FString>&, Args, FString&, OutResult);

/**
 * Debug command structure
 */
USTRUCT(BlueprintType)
struct DEBUGCONSOLECOMMANDS_API FDebugCommand
{
	GENERATED_BODY()

	/** Command name (case-insensitive) */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Command")
	FString Name;

	/** Command description */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Command")
	FString Description;

	/** Command category */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Command")
	EDebugCommandCategory Category = EDebugCommandCategory::Custom;

	/** Usage syntax */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Command")
	FString Usage;

	/** Execution delegate */
	FDebugCommandDelegate ExecuteDelegate;

	/** Native execution function (for C++ commands) */
	TFunction<FString(const TArray<FString>&, UWorld*)> NativeExecute;

	FDebugCommand() = default;

	FDebugCommand(
		const FString& InName,
		const FString& InDescription,
		EDebugCommandCategory InCategory,
		const FString& InUsage,
		TFunction<FString(const TArray<FString>&, UWorld*)> InNativeExecute)
		: Name(InName)
		, Description(InDescription)
		, Category(InCategory)
		, Usage(InUsage)
		, NativeExecute(InNativeExecute)
	{
	}
};
