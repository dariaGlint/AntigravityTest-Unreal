#include "EventLogSubsystem.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

UEventLogSubsystem::UEventLogSubsystem()
	: MaxStoredEvents(1000)
	, bIsLoggingEnabled(true)
{
}

void UEventLogSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Events.Reserve(MaxStoredEvents);
	UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem initialized with max %d events"), MaxStoredEvents);
}

void UEventLogSubsystem::Deinitialize()
{
	Events.Empty();
	Super::Deinitialize();
}

void UEventLogSubsystem::LogEvent(EDebugEventCategory Category, EDebugEventSeverity Severity, const FString& Message, AActor* Actor, const FString& ContextData)
{
#if WITH_DEBUG_EVENT_LOG
	if (!bIsLoggingEnabled)
	{
		return;
	}

	FDebugEvent NewEvent(Category, Severity, Message);
	NewEvent.Actor = Actor;
	NewEvent.ContextData = ContextData;

	AddEventToBuffer(NewEvent);

	// Broadcast event
	OnEventLogged.Broadcast(NewEvent);

	// Also log to standard UE log
	const FString ActorName = Actor ? Actor->GetName() : TEXT("None");
	UE_LOG(LogTemp, Log, TEXT("[DebugEventLog][%s][%s] %s (Actor: %s)"),
		*UEnum::GetValueAsString(Category),
		*UEnum::GetValueAsString(Severity),
		*Message,
		*ActorName);
#endif
}

void UEventLogSubsystem::AddEventToBuffer(const FDebugEvent& Event)
{
	if (MaxStoredEvents > 0 && Events.Num() >= MaxStoredEvents)
	{
		// Ring buffer behavior: remove oldest event
		Events.RemoveAt(0);
	}

	Events.Add(Event);
}

TArray<FDebugEvent> UEventLogSubsystem::GetEvents() const
{
	return Events;
}

TArray<FDebugEvent> UEventLogSubsystem::GetFilteredEvents(const TArray<EDebugEventCategory>& Categories, const TArray<EDebugEventSeverity>& Severities, const FString& SearchText) const
{
	TArray<FDebugEvent> FilteredEvents;

	for (const FDebugEvent& Event : Events)
	{
		// Filter by category
		if (Categories.Num() > 0 && !Categories.Contains(Event.Category))
		{
			continue;
		}

		// Filter by severity
		if (Severities.Num() > 0 && !Severities.Contains(Event.Severity))
		{
			continue;
		}

		// Filter by search text
		if (!SearchText.IsEmpty() && !Event.Message.Contains(SearchText))
		{
			continue;
		}

		FilteredEvents.Add(Event);
	}

	return FilteredEvents;
}

void UEventLogSubsystem::ClearEvents()
{
	Events.Empty();
	UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem: Cleared all events"));
}

void UEventLogSubsystem::SetMaxEvents(int32 MaxEvents)
{
	MaxStoredEvents = FMath::Max(0, MaxEvents);

	// Trim events if necessary
	if (MaxStoredEvents > 0 && Events.Num() > MaxStoredEvents)
	{
		const int32 NumToRemove = Events.Num() - MaxStoredEvents;
		Events.RemoveAt(0, NumToRemove);
	}

	UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem: Max events set to %d"), MaxStoredEvents);
}

void UEventLogSubsystem::SetLoggingEnabled(bool bEnabled)
{
	bIsLoggingEnabled = bEnabled;
	UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem: Logging %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

FString UEventLogSubsystem::ConvertToCSV(const TArray<FDebugEvent>& EventsToExport) const
{
	FString CSV;

	// Header
	CSV += TEXT("Timestamp,FrameNumber,Category,Severity,Message,Actor,ContextData\n");

	// Data rows
	for (const FDebugEvent& Event : EventsToExport)
	{
		const FString ActorName = Event.Actor.IsValid() ? Event.Actor->GetName() : TEXT("");
		const FString EscapedMessage = Event.Message.Replace(TEXT("\""), TEXT("\"\""));
		const FString EscapedContext = Event.ContextData.Replace(TEXT("\""), TEXT("\"\""));

		CSV += FString::Printf(TEXT("%s,%lld,%s,%s,\"%s\",%s,\"%s\"\n"),
			*Event.Timestamp.ToString(),
			Event.FrameNumber,
			*UEnum::GetValueAsString(Event.Category),
			*UEnum::GetValueAsString(Event.Severity),
			*EscapedMessage,
			*ActorName,
			*EscapedContext);
	}

	return CSV;
}

FString UEventLogSubsystem::ConvertToJSON(const TArray<FDebugEvent>& EventsToExport) const
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (const FDebugEvent& Event : EventsToExport)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

		JsonObject->SetStringField(TEXT("Timestamp"), Event.Timestamp.ToString());
		JsonObject->SetNumberField(TEXT("FrameNumber"), Event.FrameNumber);
		JsonObject->SetStringField(TEXT("Category"), UEnum::GetValueAsString(Event.Category));
		JsonObject->SetStringField(TEXT("Severity"), UEnum::GetValueAsString(Event.Severity));
		JsonObject->SetStringField(TEXT("Message"), Event.Message);
		JsonObject->SetStringField(TEXT("Actor"), Event.Actor.IsValid() ? Event.Actor->GetName() : TEXT(""));
		JsonObject->SetStringField(TEXT("ContextData"), Event.ContextData);

		JsonArray.Add(MakeShared<FJsonValueObject>(JsonObject));
	}

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonArray, Writer);

	return OutputString;
}

bool UEventLogSubsystem::ExportToCSV(const FString& FilePath)
{
	const FString CSV = ConvertToCSV(Events);

	if (FFileHelper::SaveStringToFile(CSV, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem: Exported %d events to CSV: %s"), Events.Num(), *FilePath);
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("EventLogSubsystem: Failed to export to CSV: %s"), *FilePath);
	return false;
}

bool UEventLogSubsystem::ExportToJSON(const FString& FilePath)
{
	const FString JSON = ConvertToJSON(Events);

	if (FFileHelper::SaveStringToFile(JSON, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem: Exported %d events to JSON: %s"), Events.Num(), *FilePath);
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("EventLogSubsystem: Failed to export to JSON: %s"), *FilePath);
	return false;
}

void UEventLogSubsystem::CopyToClipboard(const FString& Format)
{
	FString ClipboardText;

	if (Format.Equals(TEXT("JSON"), ESearchCase::IgnoreCase))
	{
		ClipboardText = ConvertToJSON(Events);
	}
	else
	{
		ClipboardText = ConvertToCSV(Events);
	}

	FPlatformApplicationMisc::ClipboardCopy(*ClipboardText);
	UE_LOG(LogTemp, Log, TEXT("EventLogSubsystem: Copied %d events to clipboard as %s"), Events.Num(), *Format);
}
