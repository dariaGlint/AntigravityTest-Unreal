// Copyright Antigravity. All Rights Reserved.

#include "SEventLogWidget.h"
#include "EventLogSubsystem.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/STableRow.h"

#if WITH_EVENT_LOG

void SEventLogWidget::Construct(const FArguments& InArgs, UEventLogSubsystem* InSubsystem)
{
	Subsystem = InSubsystem;
	MaxVisibleEvents = InArgs._MaxVisibleEvents;

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.DarkGroupBorder"))
		.Padding(8.0f)
		[
			SNew(SBox)
			.WidthOverride(500.0f)
			.HeightOverride(300.0f)
			[
				SNew(SVerticalBox)

				// Header
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 4)
				[
					SNew(SHorizontalBox)

					// Title
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Event Log")))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					]

					// Pause button
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(4, 0)
					[
						SNew(SButton)
						.Text(this, &SEventLogWidget::GetPauseButtonText)
						.OnClicked(this, &SEventLogWidget::OnPauseClicked)
					]

					// Clear button
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(4, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Clear")))
						.OnClicked(this, &SEventLogWidget::OnClearClicked)
					]

					// Close button
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("X")))
						.OnClicked(this, &SEventLogWidget::OnCloseClicked)
					]
				]

				// Event list
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
					.Padding(2.0f)
					[
						SAssignNew(EventListView, SListView<TSharedPtr<FDebugEvent>>)
						.ListItemsSource(&EventItems)
						.OnGenerateRow(this, &SEventLogWidget::GenerateEventRow)
						.SelectionMode(ESelectionMode::None)
					]
				]

				// Footer
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 4, 0, 0)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						return FText::Format(
							FText::FromString(TEXT("Events: {0}")),
							FText::AsNumber(EventItems.Num())
						);
					})
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]
			]
		]
	];

	// Initial refresh
	RefreshEventList();
}

void SEventLogWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	TimeSinceRefresh += InDeltaTime;
	if (TimeSinceRefresh >= RefreshInterval)
	{
		TimeSinceRefresh = 0.0f;
		RefreshEventList();
	}
}

void SEventLogWidget::ToggleVisibility()
{
	SetVisibility(GetVisibility() == EVisibility::Visible ? EVisibility::Collapsed : EVisibility::Visible);
}

void SEventLogWidget::SetCategoryFilter(EDebugEventCategory Category, bool bEnabled)
{
	if (bEnabled)
	{
		CurrentFilter.Categories.AddUnique(Category);
	}
	else
	{
		CurrentFilter.Categories.Remove(Category);
	}
	RefreshEventList();
}

void SEventLogWidget::SetSeverityFilter(EDebugEventSeverity Severity, bool bEnabled)
{
	if (bEnabled)
	{
		CurrentFilter.Severities.AddUnique(Severity);
	}
	else
	{
		CurrentFilter.Severities.Remove(Severity);
	}
	RefreshEventList();
}

void SEventLogWidget::ClearFilters()
{
	CurrentFilter = FEventLogFilter();
	RefreshEventList();
}

TSharedRef<ITableRow> SEventLogWidget::GenerateEventRow(TSharedPtr<FDebugEvent> Event, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FDebugEvent>>, OwnerTable)
		[
			SNew(SHorizontalBox)

			// Timestamp
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2, 1)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Event->GetTimestampString()))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
			]

			// Category
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4, 1)
			[
				SNew(STextBlock)
				.Text(FText::Format(FText::FromString(TEXT("[{0}]")), FText::FromString(FDebugEvent::GetCategoryName(Event->Category))))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.8f, 1.0f)))
			]

			// Message
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(4, 1)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Event->Message))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(GetSeverityColor(Event->Severity))
			]
		];
}

void SEventLogWidget::RefreshEventList()
{
	if (!Subsystem.IsValid())
	{
		return;
	}

	// Get filtered events
	TArray<FDebugEvent> Events = Subsystem->GetFilteredEvents(CurrentFilter);

	// Limit to max visible
	if (Events.Num() > MaxVisibleEvents)
	{
		Events.SetNum(MaxVisibleEvents);
	}

	// Update items
	EventItems.Empty(Events.Num());
	for (const FDebugEvent& Event : Events)
	{
		EventItems.Add(MakeShared<FDebugEvent>(Event));
	}

	// Refresh list view
	if (EventListView.IsValid())
	{
		EventListView->RequestListRefresh();

		// Auto-scroll to bottom (newest events are at top, so scroll to top)
		if (bAutoScroll && EventItems.Num() > 0)
		{
			EventListView->ScrollToTop();
		}
	}
}

FSlateColor SEventLogWidget::GetSeverityColor(EDebugEventSeverity Severity) const
{
	switch (Severity)
	{
	case EDebugEventSeverity::Warning:
		return FSlateColor(FLinearColor(1.0f, 0.8f, 0.0f));
	case EDebugEventSeverity::Error:
		return FSlateColor(FLinearColor(1.0f, 0.3f, 0.3f));
	case EDebugEventSeverity::Info:
	default:
		return FSlateColor(FLinearColor::White);
	}
}

FText SEventLogWidget::GetCategoryIcon(EDebugEventCategory Category) const
{
	switch (Category)
	{
	case EDebugEventCategory::Combat: return FText::FromString(TEXT("[Combat]"));
	case EDebugEventCategory::State: return FText::FromString(TEXT("[State]"));
	case EDebugEventCategory::AI: return FText::FromString(TEXT("[AI]"));
	case EDebugEventCategory::Input: return FText::FromString(TEXT("[Input]"));
	case EDebugEventCategory::System: return FText::FromString(TEXT("[System]"));
	case EDebugEventCategory::Custom: return FText::FromString(TEXT("[Custom]"));
	default: return FText::FromString(TEXT("[?]"));
	}
}

FReply SEventLogWidget::OnClearClicked()
{
	if (Subsystem.IsValid())
	{
		Subsystem->ClearEvents();
		RefreshEventList();
	}
	return FReply::Handled();
}

FReply SEventLogWidget::OnPauseClicked()
{
	if (Subsystem.IsValid())
	{
		Subsystem->SetPaused(!Subsystem->IsPaused());
	}
	return FReply::Handled();
}

FReply SEventLogWidget::OnCloseClicked()
{
	SetVisibility(EVisibility::Collapsed);
	return FReply::Handled();
}

FText SEventLogWidget::GetPauseButtonText() const
{
	if (Subsystem.IsValid() && Subsystem->IsPaused())
	{
		return FText::FromString(TEXT("Resume"));
	}
	return FText::FromString(TEXT("Pause"));
}

#endif // WITH_EVENT_LOG
