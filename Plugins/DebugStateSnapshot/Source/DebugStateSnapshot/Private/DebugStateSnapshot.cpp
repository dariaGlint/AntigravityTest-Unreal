// Copyright Epic Games, Inc. All Rights Reserved.

#include "DebugStateSnapshot.h"

#define LOCTEXT_NAMESPACE "FDebugStateSnapshotModule"

void FDebugStateSnapshotModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_DEBUG_STATE_SNAPSHOT
	UE_LOG(LogTemp, Log, TEXT("DebugStateSnapshot module started"));
#endif
}

void FDebugStateSnapshotModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if WITH_DEBUG_STATE_SNAPSHOT
	UE_LOG(LogTemp, Log, TEXT("DebugStateSnapshot module shutdown"));
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDebugStateSnapshotModule, DebugStateSnapshot)
