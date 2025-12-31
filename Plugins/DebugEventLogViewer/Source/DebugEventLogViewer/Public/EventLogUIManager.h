// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EventLogUIManager.generated.h"

class SEventLogWidget;
class UEventLogSubsystem;

/**
 * Manager for event log UI
 */
UCLASS()
class DEBUGEVENTLOGVIEWER_API UEventLogUIManager : public UObject
{
	GENERATED_BODY()

public:
	/** Initialize the UI manager */
	void Initialize(UEventLogSubsystem* InSubsystem);

	/** Shutdown the UI manager */
	void Shutdown();

	/** Toggle the event log widget visibility */
	UFUNCTION(BlueprintCallable, Category = "Event Log UI")
	void ToggleEventLogWidget();

	/** Show the event log widget */
	UFUNCTION(BlueprintCallable, Category = "Event Log UI")
	void ShowEventLogWidget();

	/** Hide the event log widget */
	UFUNCTION(BlueprintCallable, Category = "Event Log UI")
	void HideEventLogWidget();

	/** Check if widget is visible */
	UFUNCTION(BlueprintPure, Category = "Event Log UI")
	bool IsEventLogWidgetVisible() const;

private:
#if WITH_EVENT_LOG
	/** The event log widget */
	TSharedPtr<SEventLogWidget> EventLogWidget;

	/** Reference to subsystem */
	TWeakObjectPtr<UEventLogSubsystem> Subsystem;
#endif
};
