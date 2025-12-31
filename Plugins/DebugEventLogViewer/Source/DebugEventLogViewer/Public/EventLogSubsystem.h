#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugEventTypes.h"
#include "EventLogSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventLogged, const FDebugEvent&, Event);

/**
 * Game Instance Subsystem for managing debug event logs
 */
UCLASS()
class DEBUGEVENTLOGVIEWER_API UEventLogSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UEventLogSubsystem();

	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Log an event to the event log
	 * @param Category Event category
	 * @param Severity Event severity level
	 * @param Message Event message
	 * @param Actor Optional actor that triggered the event
	 * @param ContextData Optional additional context data in JSON format
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void LogEvent(EDebugEventCategory Category, EDebugEventSeverity Severity, const FString& Message, AActor* Actor = nullptr, const FString& ContextData = TEXT(""));

	/**
	 * Get all logged events
	 * @return Array of all logged events
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	TArray<FDebugEvent> GetEvents() const;

	/**
	 * Get filtered events
	 * @param Categories Categories to include (empty = all)
	 * @param Severities Severities to include (empty = all)
	 * @param SearchText Text to search in messages (empty = no filter)
	 * @return Filtered array of events
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	TArray<FDebugEvent> GetFilteredEvents(const TArray<EDebugEventCategory>& Categories, const TArray<EDebugEventSeverity>& Severities, const FString& SearchText = TEXT("")) const;

	/**
	 * Clear all logged events
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void ClearEvents();

	/**
	 * Set maximum number of events to store
	 * @param MaxEvents Maximum number of events (0 = unlimited, but not recommended)
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void SetMaxEvents(int32 MaxEvents);

	/**
	 * Get maximum number of events
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	int32 GetMaxEvents() const { return MaxStoredEvents; }

	/**
	 * Export events to CSV format
	 * @param FilePath File path to export to
	 * @return True if export succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	bool ExportToCSV(const FString& FilePath);

	/**
	 * Export events to JSON format
	 * @param FilePath File path to export to
	 * @return True if export succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	bool ExportToJSON(const FString& FilePath);

	/**
	 * Copy events to clipboard
	 * @param Format Format to use (CSV or JSON)
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void CopyToClipboard(const FString& Format = TEXT("CSV"));

	/**
	 * Enable or disable event logging
	 * @param bEnabled True to enable logging
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void SetLoggingEnabled(bool bEnabled);

	/**
	 * Check if logging is enabled
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	bool IsLoggingEnabled() const { return bIsLoggingEnabled; }

	/** Event fired when a new event is logged */
	UPROPERTY(BlueprintAssignable, Category = "Debug Event Log")
	FOnEventLogged OnEventLogged;

private:
	/** Ring buffer of logged events */
	TArray<FDebugEvent> Events;

	/** Maximum number of events to store */
	UPROPERTY(Config)
	int32 MaxStoredEvents;

	/** Whether logging is currently enabled */
	UPROPERTY(Config)
	bool bIsLoggingEnabled;

	/** Helper function to add event to ring buffer */
	void AddEventToBuffer(const FDebugEvent& Event);

	/** Helper function to convert events to CSV */
	FString ConvertToCSV(const TArray<FDebugEvent>& EventsToExport) const;

	/** Helper function to convert events to JSON */
	FString ConvertToJSON(const TArray<FDebugEvent>& EventsToExport) const;
};
