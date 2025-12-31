// Copyright Antigravity. All Rights Reserved.

#include "DebugEventLogViewerModule.h"

#define LOCTEXT_NAMESPACE "FDebugEventLogViewerModule"

DEFINE_LOG_CATEGORY(LogEventLogViewer);

void FDebugEventLogViewerModule::StartupModule()
{
#if WITH_EVENT_LOG
	UE_LOG(LogEventLogViewer, Log, TEXT("DebugEventLogViewer module started"));
#endif
}

void FDebugEventLogViewerModule::ShutdownModule()
{
#if WITH_EVENT_LOG
	UE_LOG(LogEventLogViewer, Log, TEXT("DebugEventLogViewer module shutdown"));
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDebugEventLogViewerModule, DebugEventLogViewer)
