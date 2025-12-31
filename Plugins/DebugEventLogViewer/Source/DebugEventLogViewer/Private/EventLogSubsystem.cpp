// Copyright Antigravity. All Rights Reserved.

#include "EventLogSubsystem.h"
#include "DebugEventLogViewerModule.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

UEventLogSubsystem* UEventLogSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UEventLogSubsystem>();
}

void UEventLogSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EVENT_LOG
	// Pre-allocate event buffer
	Events.Reserve(MaxEvents);

	UE_LOG(LogEventLogViewer, Log, TEXT("EventLogSubsystem initialized (MaxEvents: %d)"), MaxEvents);
#endif
}

void UEventLogSubsystem::Deinitialize()
{
#if WITH_EVENT_LOG
	Events.Empty();
	UE_LOG(LogEventLogViewer, Log, TEXT("EventLogSubsystem deinitialized"));
#endif

	Super::Deinitialize();
}

bool UEventLogSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EVENT_LOG
	return true;
#else
	return false;
#endif
}

void UEventLogSubsystem::LogEvent(
	EDebugEventCategory Category,
	EDebugEventSeverity Severity,
	const FString& Message,
	AActor* SourceActor,
	AActor* TargetActor)
{
#if WITH_EVENT_LOG
	if (bIsPaused)
	{
		return;
	}

	FDebugEvent Event(Category, Severity, Message, SourceActor, TargetActor);
	AddEvent(Event);
#endif
}

void UEventLogSubsystem::LogEventWithData(
	EDebugEventCategory Category,
	EDebugEventSeverity Severity,
	const FString& Message,
	AActor* SourceActor,
	AActor* TargetActor,
	const TMap<FString, FString>& AdditionalData)
{
#if WITH_EVENT_LOG
	if (bIsPaused)
	{
		return;
	}

	FDebugEvent Event(Category, Severity, Message, SourceActor, TargetActor);
	Event.AdditionalData = AdditionalData;
	AddEvent(Event);
#endif
}

void UEventLogSubsystem::AddEvent(FDebugEvent& Event)
{
#if WITH_EVENT_LOG
	// Set event metadata
	Event.EventId = NextEventId++;
	Event.Timestamp = FDateTime::Now();
	Event.FrameNumber = GFrameCounter;

	// Get game time from world
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		Event.GameTime = World->GetTimeSeconds();
	}

	// Ring buffer logic
	if (Events.Num() >= MaxEvents)
	{
		// Remove oldest event
		Events.RemoveAt(0);
	}

	// Add new event
	Events.Add(Event);

	// Broadcast event
	OnEventLogged.Broadcast(Event);

	UE_LOG(LogEventLogViewer, Verbose, TEXT("[%s] %s: %s"),
		*FDebugEvent::GetCategoryName(Event.Category),
		*FDebugEvent::GetSeverityName(Event.Severity),
		*Event.Message);
#endif
}

TArray<FDebugEvent> UEventLogSubsystem::GetAllEvents() const
{
	TArray<FDebugEvent> Result;
#if WITH_EVENT_LOG
	// Return in reverse order (most recent first)
	Result.Reserve(Events.Num());
	for (int32 i = Events.Num() - 1; i >= 0; --i)
	{
		Result.Add(Events[i]);
	}
#endif
	return Result;
}

TArray<FDebugEvent> UEventLogSubsystem::GetFilteredEvents(const FEventLogFilter& Filter) const
{
	TArray<FDebugEvent> Result;
#if WITH_EVENT_LOG
	const float CurrentTime = GetGameInstance()->GetWorld() ? GetGameInstance()->GetWorld()->GetTimeSeconds() : 0.0f;

	for (int32 i = Events.Num() - 1; i >= 0; --i)
	{
		const FDebugEvent& Event = Events[i];

		// Time range filter
		if (Filter.TimeRangeSeconds > 0.0f)
		{
			if ((CurrentTime - Event.GameTime) > Filter.TimeRangeSeconds)
			{
				continue;
			}
		}

		// Other filters
		if (Filter.PassesFilter(Event))
		{
			Result.Add(Event);
		}
	}
#endif
	return Result;
}

TArray<FDebugEvent> UEventLogSubsystem::GetEventsByCategory(EDebugEventCategory Category) const
{
	FEventLogFilter Filter;
	Filter.Categories.Add(Category);
	return GetFilteredEvents(Filter);
}

int32 UEventLogSubsystem::GetEventCount() const
{
#if WITH_EVENT_LOG
	return Events.Num();
#else
	return 0;
#endif
}

void UEventLogSubsystem::ClearEvents()
{
#if WITH_EVENT_LOG
	Events.Empty();
	UE_LOG(LogEventLogViewer, Log, TEXT("Event log cleared"));
#endif
}

void UEventLogSubsystem::SetMaxEvents(int32 NewMaxEvents)
{
#if WITH_EVENT_LOG
	MaxEvents = FMath::Max(10, NewMaxEvents);

	// Trim if necessary
	while (Events.Num() > MaxEvents)
	{
		Events.RemoveAt(0);
	}

	UE_LOG(LogEventLogViewer, Log, TEXT("MaxEvents set to %d"), MaxEvents);
#endif
}

void UEventLogSubsystem::SetPaused(bool bNewPaused)
{
#if WITH_EVENT_LOG
	bIsPaused = bNewPaused;
	UE_LOG(LogEventLogViewer, Log, TEXT("Event logging %s"), bIsPaused ? TEXT("paused") : TEXT("resumed"));
#endif
}

bool UEventLogSubsystem::IsEventLogEnabled()
{
#if WITH_EVENT_LOG
	return true;
#else
	return false;
#endif
}
