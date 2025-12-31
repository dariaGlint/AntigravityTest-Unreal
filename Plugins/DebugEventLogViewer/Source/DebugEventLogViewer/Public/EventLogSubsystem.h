// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugEvent.h"
#include "EventLogSubsystem.generated.h"

/** Delegate for when an event is logged */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventLogged, const FDebugEvent&, Event);

/**
 * Subsystem for managing debug event logging
 */
UCLASS()
class DEBUGEVENTLOGVIEWER_API UEventLogSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Get the subsystem from world context */
	UFUNCTION(BlueprintPure, Category = "Event Log", meta = (WorldContext = "WorldContextObject"))
	static UEventLogSubsystem* Get(const UObject* WorldContextObject);

	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	/**
	 * Log a debug event
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	void LogEvent(
		EDebugEventCategory Category,
		EDebugEventSeverity Severity,
		const FString& Message,
		AActor* SourceActor = nullptr,
		AActor* TargetActor = nullptr);

	/**
	 * Log a debug event with additional data
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	void LogEventWithData(
		EDebugEventCategory Category,
		EDebugEventSeverity Severity,
		const FString& Message,
		AActor* SourceActor,
		AActor* TargetActor,
		const TMap<FString, FString>& AdditionalData);

	/**
	 * Get all events (most recent first)
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	TArray<FDebugEvent> GetAllEvents() const;

	/**
	 * Get events with filter (most recent first)
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	TArray<FDebugEvent> GetFilteredEvents(const FEventLogFilter& Filter) const;

	/**
	 * Get events by category
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	TArray<FDebugEvent> GetEventsByCategory(EDebugEventCategory Category) const;

	/**
	 * Get the number of logged events
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	int32 GetEventCount() const;

	/**
	 * Clear all logged events
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	void ClearEvents();

	/**
	 * Set maximum number of events to keep
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	void SetMaxEvents(int32 NewMaxEvents);

	/**
	 * Get maximum number of events
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	int32 GetMaxEvents() const { return MaxEvents; }

	/**
	 * Set whether logging is paused
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Log")
	void SetPaused(bool bNewPaused);

	/**
	 * Check if logging is paused
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	bool IsPaused() const { return bIsPaused; }

	/**
	 * Check if event logging is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Event Log")
	static bool IsEventLogEnabled();

	/** Called when an event is logged */
	UPROPERTY(BlueprintAssignable, Category = "Event Log")
	FOnEventLogged OnEventLogged;

protected:
	/** Maximum number of events to keep (ring buffer) */
	UPROPERTY()
	int32 MaxEvents = 1000;

	/** Whether logging is paused */
	UPROPERTY()
	bool bIsPaused = false;

	/** Next event ID */
	int64 NextEventId = 1;

private:
	/** Event ring buffer */
	TArray<FDebugEvent> Events;

	/** Add event to buffer (handles ring buffer logic) */
	void AddEvent(FDebugEvent& Event);
};
