// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugEvent.h"
#include "EventLogBlueprintLibrary.generated.h"

/**
 * Blueprint function library for event logging
 */
UCLASS()
class DEBUGEVENTLOGVIEWER_API UEventLogBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Log a debug event
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void LogDebugEvent(
		const UObject* WorldContextObject,
		EDebugEventCategory Category,
		EDebugEventSeverity Severity,
		const FString& Message,
		AActor* SourceActor = nullptr,
		AActor* TargetActor = nullptr);

	/**
	 * Log a combat event (convenience function)
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void LogCombatEvent(
		const UObject* WorldContextObject,
		const FString& Message,
		AActor* SourceActor = nullptr,
		AActor* TargetActor = nullptr,
		EDebugEventSeverity Severity = EDebugEventSeverity::Info);

	/**
	 * Log a state change event (convenience function)
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void LogStateEvent(
		const UObject* WorldContextObject,
		const FString& Message,
		AActor* SourceActor = nullptr,
		EDebugEventSeverity Severity = EDebugEventSeverity::Info);

	/**
	 * Log an AI event (convenience function)
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void LogAIEvent(
		const UObject* WorldContextObject,
		const FString& Message,
		AActor* SourceActor = nullptr,
		AActor* TargetActor = nullptr,
		EDebugEventSeverity Severity = EDebugEventSeverity::Info);

	/**
	 * Log a system event (convenience function)
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void LogSystemEvent(
		const UObject* WorldContextObject,
		const FString& Message,
		EDebugEventSeverity Severity = EDebugEventSeverity::Info);

	/**
	 * Check if event logging is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	static bool IsEventLogEnabled();

	/**
	 * Get category display name
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	static FString GetCategoryDisplayName(EDebugEventCategory Category);

	/**
	 * Get severity display name
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	static FString GetSeverityDisplayName(EDebugEventSeverity Severity);

	/**
	 * Get all events
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static TArray<FDebugEvent> GetAllEvents(const UObject* WorldContextObject);

	/**
	 * Get event count
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static int32 GetEventCount(const UObject* WorldContextObject);

	/**
	 * Clear all events
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void ClearAllEvents(const UObject* WorldContextObject);

	/**
	 * Set logging paused
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static void SetEventLogPaused(const UObject* WorldContextObject, bool bPaused);

	/**
	 * Check if logging is paused
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static bool IsEventLogPaused(const UObject* WorldContextObject);
};
