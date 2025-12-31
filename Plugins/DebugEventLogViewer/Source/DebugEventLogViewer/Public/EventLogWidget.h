#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugEventTypes.h"
#include "EventLogWidget.generated.h"

class UEventLogSubsystem;
class UScrollBox;
class UCheckBox;
class UEditableTextBox;
class UButton;
class UTextBlock;

/**
 * UMG Widget for displaying debug event log
 */
UCLASS()
class DEBUGEVENTLOGVIEWER_API UEventLogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UEventLogWidget(const FObjectInitializer& ObjectInitializer);

	// UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	/**
	 * Toggle widget visibility
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void ToggleVisibility();

	/**
	 * Refresh the event list display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void RefreshEventList();

	/**
	 * Clear all events
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void ClearEvents();

	/**
	 * Pause/Resume event logging
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void TogglePause();

	/**
	 * Export events to file
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void ExportEvents(const FString& Format);

protected:
	/** Event log subsystem reference */
	UPROPERTY()
	TObjectPtr<UEventLogSubsystem> EventLogSubsystem;

	/** Whether the widget is currently visible */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event Log")
	bool bIsVisible;

	/** Whether event logging is paused */
	UPROPERTY(BlueprintReadOnly, Category = "Debug Event Log")
	bool bIsPaused;

	// UI Components (to be bound in Blueprint)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UScrollBox> EventListScrollBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> FilterCombatCheckBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> FilterStateCheckBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> FilterAICheckBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> FilterSystemCheckBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> SearchTextBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClearButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UButton> PauseButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ExportCSVButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ExportJSONButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EventCountText;

	/**
	 * Called when a new event is logged
	 */
	UFUNCTION()
	void OnEventLogged(const FDebugEvent& Event);

	/**
	 * Get currently active category filters
	 */
	TArray<EDebugEventCategory> GetActiveCategoryFilters() const;

	/**
	 * Get search text
	 */
	FString GetSearchText() const;

	/**
	 * Create a text entry for an event
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Debug Event Log")
	UWidget* CreateEventEntry(const FDebugEvent& Event);

	/**
	 * Update the event count display
	 */
	void UpdateEventCount(int32 DisplayedCount, int32 TotalCount);

private:
	/** Handle for the event logged delegate */
	FDelegateHandle EventLoggedHandle;

	/** Timer handle for auto-refresh */
	FTimerHandle RefreshTimerHandle;
};
