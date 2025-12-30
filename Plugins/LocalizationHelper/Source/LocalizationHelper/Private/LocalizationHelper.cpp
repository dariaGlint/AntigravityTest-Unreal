#include "LocalizationHelper.h"

#define LOCTEXT_NAMESPACE "FLocalizationHelperModule"

void FLocalizationHelperModule::StartupModule()
{
	// Module initialization logic here
	UE_LOG(LogTemp, Log, TEXT("LocalizationHelper module started"));
}

void FLocalizationHelperModule::ShutdownModule()
{
	// Module shutdown logic here
	UE_LOG(LogTemp, Log, TEXT("LocalizationHelper module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLocalizationHelperModule, LocalizationHelper)
