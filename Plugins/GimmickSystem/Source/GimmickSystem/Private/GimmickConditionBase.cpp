#include "GimmickConditionBase.h"

bool UGimmickConditionBase::IsMet_Implementation(AActor* Interactor) const
{
	// Override in subclasses
	return true;
}
