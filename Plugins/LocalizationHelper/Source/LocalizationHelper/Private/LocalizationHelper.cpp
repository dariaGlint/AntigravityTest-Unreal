#include "LocalizationHelper.h"

#define LOCTEXT_NAMESPACE "FLocalizationHelperModule"

// Define log category for LocalizationHelper plugin
DEFINE_LOG_CATEGORY(LogLocalizationHelper);

void FLocalizationHelperModule::StartupModule()
{
	// Module initialization logic here
	UE_LOG(LogLocalizationHelper, Log, TEXT("LocalizationHelper module started"));
}

void FLocalizationHelperModule::ShutdownModule()
{
	// Module shutdown logic here
	UE_LOG(LogLocalizationHelper, Log, TEXT("LocalizationHelper module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLocalizationHelperModule, LocalizationHelper)
