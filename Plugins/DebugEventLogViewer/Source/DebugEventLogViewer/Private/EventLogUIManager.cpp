// Copyright Antigravity. All Rights Reserved.

#include "EventLogUIManager.h"
#include "EventLogSubsystem.h"
#include "SEventLogWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SWeakWidget.h"

void UEventLogUIManager::Initialize(UEventLogSubsystem* InSubsystem)
{
#if WITH_EVENT_LOG
	Subsystem = InSubsystem;

	// Create the widget
	if (GEngine && GEngine->GameViewport)
	{
		EventLogWidget = SNew(SEventLogWidget, InSubsystem)
			.MaxVisibleEvents(100);

		// Add to viewport (initially hidden)
		EventLogWidget->SetVisibility(EVisibility::Collapsed);

		GEngine->GameViewport->AddViewportWidgetContent(
			SNew(SWeakWidget)
			.PossiblyNullContent(EventLogWidget)
		);
	}
#endif
}

void UEventLogUIManager::Shutdown()
{
#if WITH_EVENT_LOG
	if (EventLogWidget.IsValid())
	{
		EventLogWidget.Reset();
	}
#endif
}

void UEventLogUIManager::ToggleEventLogWidget()
{
#if WITH_EVENT_LOG
	if (EventLogWidget.IsValid())
	{
		EventLogWidget->ToggleVisibility();
	}
#endif
}

void UEventLogUIManager::ShowEventLogWidget()
{
#if WITH_EVENT_LOG
	if (EventLogWidget.IsValid())
	{
		EventLogWidget->SetVisibility(EVisibility::Visible);
	}
#endif
}

void UEventLogUIManager::HideEventLogWidget()
{
#if WITH_EVENT_LOG
	if (EventLogWidget.IsValid())
	{
		EventLogWidget->SetVisibility(EVisibility::Collapsed);
	}
#endif
}

bool UEventLogUIManager::IsEventLogWidgetVisible() const
{
#if WITH_EVENT_LOG
	if (EventLogWidget.IsValid())
	{
		return EventLogWidget->GetVisibility() == EVisibility::Visible;
	}
#endif
	return false;
}
