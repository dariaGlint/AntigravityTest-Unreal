// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DebugEvent.generated.h"

/**
 * Debug event category
 */
UENUM(BlueprintType)
enum class EDebugEventCategory : uint8
{
	Combat		UMETA(DisplayName = "Combat"),
	State		UMETA(DisplayName = "State"),
	AI			UMETA(DisplayName = "AI"),
	Input		UMETA(DisplayName = "Input"),
	System		UMETA(DisplayName = "System"),
	Custom		UMETA(DisplayName = "Custom")
};

/**
 * Debug event severity level
 */
UENUM(BlueprintType)
enum class EDebugEventSeverity : uint8
{
	Info		UMETA(DisplayName = "Info"),
	Warning		UMETA(DisplayName = "Warning"),
	Error		UMETA(DisplayName = "Error")
};

/**
 * Debug event structure
 */
USTRUCT(BlueprintType)
struct DEBUGEVENTLOGVIEWER_API FDebugEvent
{
	GENERATED_BODY()

	/** Unique event ID */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	int64 EventId = 0;

	/** Real-world timestamp */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FDateTime Timestamp;

	/** Game time when event occurred */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	float GameTime = 0.0f;

	/** Frame number when event occurred */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	int64 FrameNumber = 0;

	/** Event category */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	EDebugEventCategory Category = EDebugEventCategory::Custom;

	/** Event severity */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	EDebugEventSeverity Severity = EDebugEventSeverity::Info;

	/** Event message */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FString Message;

	/** Source actor name (if applicable) */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FString SourceActorName;

	/** Target actor name (if applicable) */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	FString TargetActorName;

	/** Additional key-value data */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event")
	TMap<FString, FString> AdditionalData;

	/** Source actor weak reference */
	TWeakObjectPtr<AActor> SourceActor;

	/** Target actor weak reference */
	TWeakObjectPtr<AActor> TargetActor;

	FDebugEvent() = default;

	FDebugEvent(
		EDebugEventCategory InCategory,
		EDebugEventSeverity InSeverity,
		const FString& InMessage,
		AActor* InSourceActor = nullptr,
		AActor* InTargetActor = nullptr)
		: Category(InCategory)
		, Severity(InSeverity)
		, Message(InMessage)
	{
		if (InSourceActor)
		{
			SourceActor = InSourceActor;
			SourceActorName = InSourceActor->GetName();
		}
		if (InTargetActor)
		{
			TargetActor = InTargetActor;
			TargetActorName = InTargetActor->GetName();
		}
	}

	/** Get formatted timestamp string */
	FString GetTimestampString() const
	{
		return Timestamp.ToString(TEXT("%H:%M:%S.%s"));
	}

	/** Get category display name */
	static FString GetCategoryName(EDebugEventCategory InCategory)
	{
		switch (InCategory)
		{
		case EDebugEventCategory::Combat: return TEXT("Combat");
		case EDebugEventCategory::State: return TEXT("State");
		case EDebugEventCategory::AI: return TEXT("AI");
		case EDebugEventCategory::Input: return TEXT("Input");
		case EDebugEventCategory::System: return TEXT("System");
		case EDebugEventCategory::Custom: return TEXT("Custom");
		default: return TEXT("Unknown");
		}
	}

	/** Get severity display name */
	static FString GetSeverityName(EDebugEventSeverity InSeverity)
	{
		switch (InSeverity)
		{
		case EDebugEventSeverity::Info: return TEXT("Info");
		case EDebugEventSeverity::Warning: return TEXT("Warning");
		case EDebugEventSeverity::Error: return TEXT("Error");
		default: return TEXT("Unknown");
		}
	}
};

/**
 * Event log filter structure
 */
USTRUCT(BlueprintType)
struct DEBUGEVENTLOGVIEWER_API FEventLogFilter
{
	GENERATED_BODY()

	/** Categories to include (empty = all) */
	UPROPERTY(BlueprintReadWrite, Category = "Filter")
	TArray<EDebugEventCategory> Categories;

	/** Severities to include (empty = all) */
	UPROPERTY(BlueprintReadWrite, Category = "Filter")
	TArray<EDebugEventSeverity> Severities;

	/** Keyword filter for message */
	UPROPERTY(BlueprintReadWrite, Category = "Filter")
	FString KeywordFilter;

	/** Filter by specific actor name */
	UPROPERTY(BlueprintReadWrite, Category = "Filter")
	FString ActorNameFilter;

	/** Only show events from last N seconds (0 = no limit) */
	UPROPERTY(BlueprintReadWrite, Category = "Filter")
	float TimeRangeSeconds = 0.0f;

	/** Check if an event passes this filter */
	bool PassesFilter(const FDebugEvent& Event) const
	{
		// Category filter
		if (Categories.Num() > 0 && !Categories.Contains(Event.Category))
		{
			return false;
		}

		// Severity filter
		if (Severities.Num() > 0 && !Severities.Contains(Event.Severity))
		{
			return false;
		}

		// Keyword filter
		if (!KeywordFilter.IsEmpty() && !Event.Message.Contains(KeywordFilter))
		{
			return false;
		}

		// Actor name filter
		if (!ActorNameFilter.IsEmpty())
		{
			if (!Event.SourceActorName.Contains(ActorNameFilter) &&
				!Event.TargetActorName.Contains(ActorNameFilter))
			{
				return false;
			}
		}

		return true;
	}
};
