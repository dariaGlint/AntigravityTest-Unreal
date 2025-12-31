// Copyright Antigravity. All Rights Reserved.

#include "EventLogBlueprintLibrary.h"
#include "EventLogSubsystem.h"

void UEventLogBlueprintLibrary::LogDebugEvent(
	const UObject* WorldContextObject,
	EDebugEventCategory Category,
	EDebugEventSeverity Severity,
	const FString& Message,
	AActor* SourceActor,
	AActor* TargetActor)
{
#if WITH_EVENT_LOG
	if (UEventLogSubsystem* Subsystem = UEventLogSubsystem::Get(WorldContextObject))
	{
		Subsystem->LogEvent(Category, Severity, Message, SourceActor, TargetActor);
	}
#endif
}

void UEventLogBlueprintLibrary::LogCombatEvent(
	const UObject* WorldContextObject,
	const FString& Message,
	AActor* SourceActor,
	AActor* TargetActor,
	EDebugEventSeverity Severity)
{
	LogDebugEvent(WorldContextObject, EDebugEventCategory::Combat, Severity, Message, SourceActor, TargetActor);
}

void UEventLogBlueprintLibrary::LogStateEvent(
	const UObject* WorldContextObject,
	const FString& Message,
	AActor* SourceActor,
	EDebugEventSeverity Severity)
{
	LogDebugEvent(WorldContextObject, EDebugEventCategory::State, Severity, Message, SourceActor, nullptr);
}

void UEventLogBlueprintLibrary::LogAIEvent(
	const UObject* WorldContextObject,
	const FString& Message,
	AActor* SourceActor,
	AActor* TargetActor,
	EDebugEventSeverity Severity)
{
	LogDebugEvent(WorldContextObject, EDebugEventCategory::AI, Severity, Message, SourceActor, TargetActor);
}

void UEventLogBlueprintLibrary::LogSystemEvent(
	const UObject* WorldContextObject,
	const FString& Message,
	EDebugEventSeverity Severity)
{
	LogDebugEvent(WorldContextObject, EDebugEventCategory::System, Severity, Message, nullptr, nullptr);
}

bool UEventLogBlueprintLibrary::IsEventLogEnabled()
{
	return UEventLogSubsystem::IsEventLogEnabled();
}

FString UEventLogBlueprintLibrary::GetCategoryDisplayName(EDebugEventCategory Category)
{
	return FDebugEvent::GetCategoryName(Category);
}

FString UEventLogBlueprintLibrary::GetSeverityDisplayName(EDebugEventSeverity Severity)
{
	return FDebugEvent::GetSeverityName(Severity);
}

TArray<FDebugEvent> UEventLogBlueprintLibrary::GetAllEvents(const UObject* WorldContextObject)
{
#if WITH_EVENT_LOG
	if (UEventLogSubsystem* Subsystem = UEventLogSubsystem::Get(WorldContextObject))
	{
		return Subsystem->GetAllEvents();
	}
#endif
	return TArray<FDebugEvent>();
}

int32 UEventLogBlueprintLibrary::GetEventCount(const UObject* WorldContextObject)
{
#if WITH_EVENT_LOG
	if (UEventLogSubsystem* Subsystem = UEventLogSubsystem::Get(WorldContextObject))
	{
		return Subsystem->GetEventCount();
	}
#endif
	return 0;
}

void UEventLogBlueprintLibrary::ClearAllEvents(const UObject* WorldContextObject)
{
#if WITH_EVENT_LOG
	if (UEventLogSubsystem* Subsystem = UEventLogSubsystem::Get(WorldContextObject))
	{
		Subsystem->ClearEvents();
	}
#endif
}

void UEventLogBlueprintLibrary::SetEventLogPaused(const UObject* WorldContextObject, bool bPaused)
{
#if WITH_EVENT_LOG
	if (UEventLogSubsystem* Subsystem = UEventLogSubsystem::Get(WorldContextObject))
	{
		Subsystem->SetPaused(bPaused);
	}
#endif
}

bool UEventLogBlueprintLibrary::IsEventLogPaused(const UObject* WorldContextObject)
{
#if WITH_EVENT_LOG
	if (UEventLogSubsystem* Subsystem = UEventLogSubsystem::Get(WorldContextObject))
	{
		return Subsystem->IsPaused();
	}
#endif
	return false;
}
