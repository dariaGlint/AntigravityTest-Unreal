#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Declare log category for LocalizationHelper plugin
DECLARE_LOG_CATEGORY_EXTERN(LogLocalizationHelper, Log, All);

/**
 * Localization Helper Module
 * Provides utilities for localization workflow automation
 */
class FLocalizationHelperModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
