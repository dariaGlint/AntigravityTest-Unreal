#include "EventLogWidget.h"
#include "EventLogSubsystem.h"
#include "Components/ScrollBox.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UEventLogWidget::UEventLogWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsVisible(false)
	, bIsPaused(false)
{
}

void UEventLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get event log subsystem
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		EventLogSubsystem = GameInstance->GetSubsystem<UEventLogSubsystem>();
	}

	if (EventLogSubsystem)
	{
		// Subscribe to event logged
		EventLoggedHandle = EventLogSubsystem->OnEventLogged.AddUObject(this, &UEventLogWidget::OnEventLogged);
	}

	// Bind button events
	if (ClearButton)
	{
		ClearButton->OnClicked.AddDynamic(this, &UEventLogWidget::ClearEvents);
	}

	if (PauseButton)
	{
		PauseButton->OnClicked.AddDynamic(this, &UEventLogWidget::TogglePause);
	}

	if (ExportCSVButton)
	{
		ExportCSVButton->OnClicked.AddDynamic(this, &UEventLogWidget::ExportEvents, FString(TEXT("CSV")));
	}

	if (ExportJSONButton)
	{
		ExportJSONButton->OnClicked.AddDynamic(this, &UEventLogWidget::ExportEvents, FString(TEXT("JSON")));
	}

	// Setup auto-refresh timer (refresh every 0.5 seconds when not paused)
	GetWorld()->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UEventLogWidget::RefreshEventList, 0.5f, true);

	// Initial load
	RefreshEventList();
}

void UEventLogWidget::NativeDestruct()
{
	// Unsubscribe from events
	if (EventLogSubsystem && EventLoggedHandle.IsValid())
	{
		EventLogSubsystem->OnEventLogged.Remove(EventLoggedHandle);
	}

	// Clear timer
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);
	}

	Super::NativeDestruct();
}

FReply UEventLogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// F10 key toggles visibility
	if (InKeyEvent.GetKey() == EKeys::F10)
	{
		ToggleVisibility();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UEventLogWidget::ToggleVisibility()
{
	bIsVisible = !bIsVisible;
	SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UEventLogWidget::RefreshEventList()
{
	if (!EventLogSubsystem || !EventListScrollBox || bIsPaused)
	{
		return;
	}

	// Get filtered events
	TArray<EDebugEventCategory> CategoryFilters = GetActiveCategoryFilters();
	TArray<EDebugEventSeverity> SeverityFilters; // Empty = all severities
	FString SearchText = GetSearchText();

	TArray<FDebugEvent> FilteredEvents = EventLogSubsystem->GetFilteredEvents(CategoryFilters, SeverityFilters, SearchText);

	// Clear existing entries
	EventListScrollBox->ClearChildren();

	// Add event entries
	for (const FDebugEvent& Event : FilteredEvents)
	{
		if (UWidget* EntryWidget = CreateEventEntry(Event))
		{
			EventListScrollBox->AddChild(EntryWidget);
		}
	}

	// Update count
	UpdateEventCount(FilteredEvents.Num(), EventLogSubsystem->GetEvents().Num());

	// Scroll to bottom
	EventListScrollBox->ScrollToEnd();
}

void UEventLogWidget::ClearEvents()
{
	if (EventLogSubsystem)
	{
		EventLogSubsystem->ClearEvents();
		RefreshEventList();
	}
}

void UEventLogWidget::TogglePause()
{
	bIsPaused = !bIsPaused;

	if (EventLogSubsystem)
	{
		EventLogSubsystem->SetLoggingEnabled(!bIsPaused);
	}

	// Update button text if available
	if (PauseButton)
	{
		// This would be better done in Blueprint, but we can set a simple text here
		// In a real implementation, you'd want to have separate text blocks for button labels
	}
}

void UEventLogWidget::ExportEvents(const FString& Format)
{
	if (!EventLogSubsystem)
	{
		return;
	}

	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	const FString FileName = FString::Printf(TEXT("EventLog_%s.%s"), *Timestamp, *Format.ToLower());
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("Logs") / FileName;

	bool bSuccess = false;
	if (Format.Equals(TEXT("CSV"), ESearchCase::IgnoreCase))
	{
		bSuccess = EventLogSubsystem->ExportToCSV(FilePath);
	}
	else if (Format.Equals(TEXT("JSON"), ESearchCase::IgnoreCase))
	{
		bSuccess = EventLogSubsystem->ExportToJSON(FilePath);
	}

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Exported event log to: %s"), *FilePath);
	}
}

void UEventLogWidget::OnEventLogged(const FDebugEvent& Event)
{
	// Events are automatically added via the auto-refresh timer
	// This callback could be used for immediate updates or notifications
}

TArray<EDebugEventCategory> UEventLogWidget::GetActiveCategoryFilters() const
{
	TArray<EDebugEventCategory> Filters;

	// If no checkboxes or all unchecked, return empty (= show all)
	bool bAnyChecked = false;

	if (FilterCombatCheckBox && FilterCombatCheckBox->IsChecked())
	{
		Filters.Add(EDebugEventCategory::Combat);
		bAnyChecked = true;
	}

	if (FilterStateCheckBox && FilterStateCheckBox->IsChecked())
	{
		Filters.Add(EDebugEventCategory::State);
		bAnyChecked = true;
	}

	if (FilterAICheckBox && FilterAICheckBox->IsChecked())
	{
		Filters.Add(EDebugEventCategory::AI);
		bAnyChecked = true;
	}

	if (FilterSystemCheckBox && FilterSystemCheckBox->IsChecked())
	{
		Filters.Add(EDebugEventCategory::System);
		bAnyChecked = true;
	}

	// If none checked, return empty array (which means show all)
	if (!bAnyChecked)
	{
		Filters.Empty();
	}

	return Filters;
}

FString UEventLogWidget::GetSearchText() const
{
	if (SearchTextBox)
	{
		return SearchTextBox->GetText().ToString();
	}

	return FString();
}

void UEventLogWidget::UpdateEventCount(int32 DisplayedCount, int32 TotalCount)
{
	if (EventCountText)
	{
		const FString CountText = FString::Printf(TEXT("Showing %d / %d events"), DisplayedCount, TotalCount);
		EventCountText->SetText(FText::FromString(CountText));
	}
}
