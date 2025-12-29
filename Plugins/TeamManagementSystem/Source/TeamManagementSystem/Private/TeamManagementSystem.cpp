#include "TeamManagementSystem.h"

#define LOCTEXT_NAMESPACE "FTeamManagementSystemModule"

void FTeamManagementSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
}

void FTeamManagementSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTeamManagementSystemModule, TeamManagementSystem)
