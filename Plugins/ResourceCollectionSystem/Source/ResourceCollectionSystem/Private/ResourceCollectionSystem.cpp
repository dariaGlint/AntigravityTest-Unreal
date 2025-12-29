#include "ResourceCollectionSystem.h"

#define LOCTEXT_NAMESPACE "FResourceCollectionSystemModule"

void FResourceCollectionSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
}

void FResourceCollectionSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FResourceCollectionSystemModule, ResourceCollectionSystem)
