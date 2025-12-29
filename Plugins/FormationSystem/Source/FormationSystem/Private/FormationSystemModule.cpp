#include "FormationSystemModule.h"

#define LOCTEXT_NAMESPACE "FFormationSystemModule"

void FFormationSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
}

void FFormationSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFormationSystemModule, FormationSystem)
