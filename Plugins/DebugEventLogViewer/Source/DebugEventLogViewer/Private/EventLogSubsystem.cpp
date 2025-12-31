// Copyright Antigravity. All Rights Reserved.

#include "EventLogSubsystem.h"
#include "EventLogUIManager.h"
#include "DebugEventLogViewerModule.h"
#include "DebugCommandSubsystem.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

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

	// Register debug console commands
	RegisterEventLogCommands();

	UE_LOG(LogEventLogViewer, Log, TEXT("EventLogSubsystem initialized (MaxEvents: %d)"), MaxEvents);
#endif
}

void UEventLogSubsystem::RegisterEventLogCommands()
{
#if WITH_EVENT_LOG && WITH_DEBUG_COMMANDS
	UDebugCommandSubsystem* DebugCommands = GetGameInstance()->GetSubsystem<UDebugCommandSubsystem>();
	if (!DebugCommands)
	{
		UE_LOG(LogEventLogViewer, Warning, TEXT("DebugCommandSubsystem not available, event log commands not registered"));
		return;
	}

	// ShowEventLog - Show the event log UI
	DebugCommands->RegisterNativeCommand(
		TEXT("ShowEventLog"),
		TEXT("Show the event log viewer UI"),
		EDebugCommandCategory::Info,
		TEXT("ShowEventLog"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (EventLog)
			{
				EventLog->ShowEventLogUI();
				return TEXT("Event log UI shown");
			}

			return TEXT("EventLogSubsystem not found");
		}
	);

	// HideEventLog - Hide the event log UI
	DebugCommands->RegisterNativeCommand(
		TEXT("HideEventLog"),
		TEXT("Hide the event log viewer UI"),
		EDebugCommandCategory::Info,
		TEXT("HideEventLog"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (EventLog)
			{
				EventLog->HideEventLogUI();
				return TEXT("Event log UI hidden");
			}

			return TEXT("EventLogSubsystem not found");
		}
	);

	// ToggleEventLog - Toggle the event log UI
	DebugCommands->RegisterNativeCommand(
		TEXT("ToggleEventLog"),
		TEXT("Toggle the event log viewer UI"),
		EDebugCommandCategory::Info,
		TEXT("ToggleEventLog"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (EventLog)
			{
				EventLog->ToggleEventLogUI();
				bool bVisible = EventLog->IsEventLogUIVisible();
				return bVisible ? TEXT("Event log UI shown") : TEXT("Event log UI hidden");
			}

			return TEXT("EventLogSubsystem not found");
		}
	);

	// ClearEventLog - Clear all events
	DebugCommands->RegisterNativeCommand(
		TEXT("ClearEventLog"),
		TEXT("Clear all logged events"),
		EDebugCommandCategory::Info,
		TEXT("ClearEventLog"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (EventLog)
			{
				int32 Count = EventLog->GetEventCount();
				EventLog->ClearEvents();
				return FString::Printf(TEXT("Cleared %d events"), Count);
			}

			return TEXT("EventLogSubsystem not found");
		}
	);

	// EventLogCount - Get the number of events
	DebugCommands->RegisterNativeCommand(
		TEXT("EventLogCount"),
		TEXT("Get the number of logged events"),
		EDebugCommandCategory::Info,
		TEXT("EventLogCount"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (EventLog)
			{
				int32 Count = EventLog->GetEventCount();
				int32 MaxCount = EventLog->GetMaxEvents();
				return FString::Printf(TEXT("Event count: %d / %d"), Count, MaxCount);
			}

			return TEXT("EventLogSubsystem not found");
		}
	);

	// ExportEventLog - Export events to file
	DebugCommands->RegisterNativeCommand(
		TEXT("ExportEventLog"),
		TEXT("Export events to CSV or JSON file"),
		EDebugCommandCategory::Info,
		TEXT("ExportEventLog <FilePath> (.csv or .json)"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			if (Args.Num() < 1)
			{
				return TEXT("Usage: ExportEventLog <FilePath> (.csv or .json)");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (!EventLog)
			{
				return TEXT("EventLogSubsystem not found");
			}

			FString FilePath = Args[0];
			FString Extension = FPaths::GetExtension(FilePath).ToLower();

			bool bSuccess = false;
			if (Extension == TEXT("csv"))
			{
				bSuccess = EventLog->ExportToCSV(FilePath);
			}
			else if (Extension == TEXT("json"))
			{
				bSuccess = EventLog->ExportToJSON(FilePath);
			}
			else
			{
				return TEXT("Invalid file extension. Use .csv or .json");
			}

			if (bSuccess)
			{
				return FString::Printf(TEXT("Exported %d events to: %s"), EventLog->GetEventCount(), *FilePath);
			}

			return FString::Printf(TEXT("Failed to export to: %s"), *FilePath);
		}
	);

	// EventLogPause - Pause/Resume event logging
	DebugCommands->RegisterNativeCommand(
		TEXT("EventLogPause"),
		TEXT("Pause or resume event logging"),
		EDebugCommandCategory::Info,
		TEXT("EventLogPause [on|off]"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (!World)
			{
				return TEXT("No world found");
			}

			UEventLogSubsystem* EventLog = UEventLogSubsystem::Get(World);
			if (!EventLog)
			{
				return TEXT("EventLogSubsystem not found");
			}

			if (Args.Num() >= 1)
			{
				FString Arg = Args[0].ToLower();
				if (Arg == TEXT("on") || Arg == TEXT("true") || Arg == TEXT("1"))
				{
					EventLog->SetPaused(true);
					return TEXT("Event logging paused");
				}
				else if (Arg == TEXT("off") || Arg == TEXT("false") || Arg == TEXT("0"))
				{
					EventLog->SetPaused(false);
					return TEXT("Event logging resumed");
				}
			}

			// Toggle if no argument
			bool bCurrentlyPaused = EventLog->IsPaused();
			EventLog->SetPaused(!bCurrentlyPaused);
			return bCurrentlyPaused ? TEXT("Event logging resumed") : TEXT("Event logging paused");
		}
	);

	UE_LOG(LogEventLogViewer, Log, TEXT("Event log commands registered"));
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

void UEventLogSubsystem::ToggleEventLogUI()
{
#if WITH_EVENT_LOG
	if (UIManager)
	{
		UIManager->ToggleEventLogWidget();
	}
#endif
}

void UEventLogSubsystem::ShowEventLogUI()
{
#if WITH_EVENT_LOG
	if (UIManager)
	{
		UIManager->ShowEventLogWidget();
	}
#endif
}

void UEventLogSubsystem::HideEventLogUI()
{
#if WITH_EVENT_LOG
	if (UIManager)
	{
		UIManager->HideEventLogWidget();
	}
#endif
}

bool UEventLogSubsystem::IsEventLogUIVisible() const
{
#if WITH_EVENT_LOG
	if (UIManager)
	{
		return UIManager->IsEventLogWidgetVisible();
	}
#endif
	return false;
}

bool UEventLogSubsystem::ExportToCSV(const FString& FilePath) const
{
#if WITH_EVENT_LOG
	FString CSVContent = GetEventsAsCSV();
	return FFileHelper::SaveStringToFile(CSVContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
#else
	return false;
#endif
}

bool UEventLogSubsystem::ExportToJSON(const FString& FilePath) const
{
#if WITH_EVENT_LOG
	FString JSONContent = GetEventsAsJSON();
	return FFileHelper::SaveStringToFile(JSONContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
#else
	return false;
#endif
}

FString UEventLogSubsystem::GetEventsAsCSV() const
{
	FString Result;
#if WITH_EVENT_LOG
	// CSV Header
	Result = TEXT("EventId,Timestamp,GameTime,FrameNumber,Category,Severity,Message,SourceActor,TargetActor\n");

	// CSV Data (most recent first)
	for (int32 i = Events.Num() - 1; i >= 0; --i)
	{
		const FDebugEvent& Event = Events[i];

		// Escape message for CSV (replace quotes with double quotes)
		FString EscapedMessage = Event.Message.Replace(TEXT("\""), TEXT("\"\""));

		Result += FString::Printf(TEXT("%lld,%s,%.3f,%lld,%s,%s,\"%s\",%s,%s\n"),
			Event.EventId,
			*Event.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S")),
			Event.GameTime,
			Event.FrameNumber,
			*FDebugEvent::GetCategoryName(Event.Category),
			*FDebugEvent::GetSeverityName(Event.Severity),
			*EscapedMessage,
			Event.SourceActorName.IsEmpty() ? TEXT("") : *Event.SourceActorName,
			Event.TargetActorName.IsEmpty() ? TEXT("") : *Event.TargetActorName);
	}
#endif
	return Result;
}

FString UEventLogSubsystem::GetEventsAsJSON() const
{
	FString Result;
#if WITH_EVENT_LOG
	TArray<TSharedPtr<FJsonValue>> EventArray;

	// Build JSON array (most recent first)
	for (int32 i = Events.Num() - 1; i >= 0; --i)
	{
		const FDebugEvent& Event = Events[i];

		TSharedPtr<FJsonObject> EventObj = MakeShared<FJsonObject>();
		EventObj->SetNumberField(TEXT("eventId"), static_cast<double>(Event.EventId));
		EventObj->SetStringField(TEXT("timestamp"), Event.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S.%s")));
		EventObj->SetNumberField(TEXT("gameTime"), Event.GameTime);
		EventObj->SetNumberField(TEXT("frameNumber"), static_cast<double>(Event.FrameNumber));
		EventObj->SetStringField(TEXT("category"), FDebugEvent::GetCategoryName(Event.Category));
		EventObj->SetStringField(TEXT("severity"), FDebugEvent::GetSeverityName(Event.Severity));
		EventObj->SetStringField(TEXT("message"), Event.Message);
		EventObj->SetStringField(TEXT("sourceActor"), Event.SourceActorName);
		EventObj->SetStringField(TEXT("targetActor"), Event.TargetActorName);

		// Add additional data if present
		if (Event.AdditionalData.Num() > 0)
		{
			TSharedPtr<FJsonObject> AdditionalDataObj = MakeShared<FJsonObject>();
			for (const auto& Pair : Event.AdditionalData)
			{
				AdditionalDataObj->SetStringField(Pair.Key, Pair.Value);
			}
			EventObj->SetObjectField(TEXT("additionalData"), AdditionalDataObj);
		}

		EventArray.Add(MakeShared<FJsonValueObject>(EventObj));
	}

	// Create root object
	TSharedPtr<FJsonObject> RootObj = MakeShared<FJsonObject>();
	RootObj->SetArrayField(TEXT("events"), EventArray);
	RootObj->SetNumberField(TEXT("count"), Events.Num());
	RootObj->SetStringField(TEXT("exportTime"), FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S")));

	// Serialize to string
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Result);
	FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);
#endif
	return Result;
}

void UEventLogSubsystem::CopyEventsToClipboard() const
{
#if WITH_EVENT_LOG
	FString ClipboardText;

	for (int32 i = Events.Num() - 1; i >= 0; --i)
	{
		const FDebugEvent& Event = Events[i];
		ClipboardText += FString::Printf(TEXT("[%s] [%s] %s: %s\n"),
			*Event.Timestamp.ToString(TEXT("%H:%M:%S")),
			*FDebugEvent::GetCategoryName(Event.Category),
			*FDebugEvent::GetSeverityName(Event.Severity),
			*Event.Message);
	}

	FPlatformApplicationMisc::ClipboardCopy(*ClipboardText);
	UE_LOG(LogEventLogViewer, Log, TEXT("Copied %d events to clipboard"), Events.Num());
#endif
}
