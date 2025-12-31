#pragma once

#include "CoreMinimal.h"
#include "DebugEventTypes.generated.h"

/**
 * Debug event category
 */
UENUM(BlueprintType)
enum class EDebugEventCategory : uint8
{
	Combat UMETA(DisplayName = "Combat"),
	State UMETA(DisplayName = "State"),
	AI UMETA(DisplayName = "AI"),
	System UMETA(DisplayName = "System"),
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Debug event severity level
 */
UENUM(BlueprintType)
enum class EDebugEventSeverity : uint8
{
	Verbose UMETA(DisplayName = "Verbose"),
	Info UMETA(DisplayName = "Info"),
	Warning UMETA(DisplayName = "Warning"),
	Error UMETA(DisplayName = "Error"),
	Critical UMETA(DisplayName = "Critical")
};

/**
 * Debug event structure
 */
USTRUCT(BlueprintType)
struct DEBUGEVENTLOGVIEWER_API FDebugEvent
{
	GENERATED_BODY()

	/** Timestamp when the event occurred */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FDateTime Timestamp;

	/** Frame number when the event occurred */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	int64 FrameNumber;

	/** Event category */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	EDebugEventCategory Category;

	/** Event severity */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	EDebugEventSeverity Severity;

	/** Event message */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FString Message;

	/** Actor that triggered the event (if any) */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	TWeakObjectPtr<AActor> Actor;

	/** Additional context data (JSON format) */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FString ContextData;

	FDebugEvent()
		: Timestamp(FDateTime::Now())
		, FrameNumber(0)
		, Category(EDebugEventCategory::System)
		, Severity(EDebugEventSeverity::Info)
	{
	}

	FDebugEvent(EDebugEventCategory InCategory, EDebugEventSeverity InSeverity, const FString& InMessage)
		: Timestamp(FDateTime::Now())
		, FrameNumber(GFrameCounter)
		, Category(InCategory)
		, Severity(InSeverity)
		, Message(InMessage)
	{
	}
};
