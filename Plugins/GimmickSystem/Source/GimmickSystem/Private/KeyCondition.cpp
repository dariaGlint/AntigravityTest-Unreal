#include "KeyCondition.h"
#include "InventoryInterface.h"

UKeyCondition::UKeyCondition()
{
	ConditionDescription = FText::FromString(TEXT("Requires a key"));
}

bool UKeyCondition::IsMet_Implementation(AActor* Interactor) const
{
	return HasKey(Interactor);
}

bool UKeyCondition::HasKey(AActor* Actor) const
{
	if (!Actor || !RequiredKeyTag.IsValid())
	{
		return false;
	}

	// Check if actor implements inventory interface
	if (Actor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(Actor);
		if (InventoryInterface)
		{
			return InventoryInterface->Execute_HasItemWithTag(Actor, RequiredKeyTag);
		}
	}

	// Could also check for a component with the key tag
	// This is a fallback or alternative implementation

	return false;
}
