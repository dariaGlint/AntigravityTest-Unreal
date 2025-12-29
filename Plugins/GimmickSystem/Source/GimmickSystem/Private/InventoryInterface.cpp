#include "InventoryInterface.h"

// Default implementation
bool IInventoryInterface::Execute_HasItemWithTag(const UObject* Object, const FGameplayTag& Tag)
{
	// This will be overridden by implementing classes
	return false;
}
