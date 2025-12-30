#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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
