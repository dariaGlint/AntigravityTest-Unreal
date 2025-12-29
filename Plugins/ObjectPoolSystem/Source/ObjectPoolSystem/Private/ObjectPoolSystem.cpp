// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectPoolSystem.h"

#define LOCTEXT_NAMESPACE "FObjectPoolSystemModule"

void FObjectPoolSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Log, TEXT("ObjectPoolSystem module started"));
}

void FObjectPoolSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Log, TEXT("ObjectPoolSystem module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FObjectPoolSystemModule, ObjectPoolSystem)
