#include "MultiplayerSessionSystem.h"

#define LOCTEXT_NAMESPACE "FMultiplayerSessionSystemModule"

void FMultiplayerSessionSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
}

void FMultiplayerSessionSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMultiplayerSessionSystemModule, MultiplayerSessionSystem)
