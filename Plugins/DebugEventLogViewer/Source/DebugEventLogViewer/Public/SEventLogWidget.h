// Copyright Antigravity. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "DebugEvent.h"

class UEventLogSubsystem;

/**
 * Slate widget for displaying event log
 */
class DEBUGEVENTLOGVIEWER_API SEventLogWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEventLogWidget)
		: _MaxVisibleEvents(100)
	{}
		SLATE_ARGUMENT(int32, MaxVisibleEvents)
	SLATE_END_ARGS()

	/** Construct this widget */
	void Construct(const FArguments& InArgs, UEventLogSubsystem* InSubsystem);

	/** Tick - update display */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Toggle visibility */
	void ToggleVisibility();

	/** Set category filter */
	void SetCategoryFilter(EDebugEventCategory Category, bool bEnabled);

	/** Set severity filter */
	void SetSeverityFilter(EDebugEventSeverity Severity, bool bEnabled);

	/** Clear all filters */
	void ClearFilters();

private:
	/** Generate row for list view */
	TSharedRef<ITableRow> GenerateEventRow(TSharedPtr<FDebugEvent> Event, const TSharedRef<STableViewBase>& OwnerTable);

	/** Refresh the event list */
	void RefreshEventList();

	/** Get color for severity */
	FSlateColor GetSeverityColor(EDebugEventSeverity Severity) const;

	/** Get category icon text */
	FText GetCategoryIcon(EDebugEventCategory Category) const;

	/** Handle clear button clicked */
	FReply OnClearClicked();

	/** Handle pause button clicked */
	FReply OnPauseClicked();

	/** Handle close button clicked */
	FReply OnCloseClicked();

	/** Get pause button text */
	FText GetPauseButtonText() const;

private:
	/** Reference to subsystem */
	TWeakObjectPtr<UEventLogSubsystem> Subsystem;

	/** List view widget */
	TSharedPtr<SListView<TSharedPtr<FDebugEvent>>> EventListView;

	/** Event items for list view */
	TArray<TSharedPtr<FDebugEvent>> EventItems;

	/** Current filter */
	FEventLogFilter CurrentFilter;

	/** Maximum visible events */
	int32 MaxVisibleEvents = 100;

	/** Time since last refresh */
	float TimeSinceRefresh = 0.0f;

	/** Refresh interval */
	float RefreshInterval = 0.1f;

	/** Auto-scroll enabled */
	bool bAutoScroll = true;
};
