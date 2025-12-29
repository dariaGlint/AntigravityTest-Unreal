#include "InteractableBase.h"
#include "GimmickConditionBase.h"

AInteractableBase::AInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsEnabled = true;
	bCanBeUsedMultipleTimes = true;
	UsageCount = 0;
	InteractionRange = 200.0f;
	InteractionPrompt = FText::FromString(TEXT("Interact"));
}

void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
}

bool AInteractableBase::CanInteract_Implementation(AActor* Interactor) const
{
	if (!bIsEnabled)
	{
		return false;
	}

	if (!bCanBeUsedMultipleTimes && UsageCount > 0)
	{
		return false;
	}

	if (!IsInRange(Interactor))
	{
		return false;
	}

	return CheckAllConditions(Interactor);
}

void AInteractableBase::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract(Interactor))
	{
		OnInteractionFailed.Broadcast(Interactor);
		return;
	}

	OnInteractionStarted.Broadcast(Interactor);

	OnInteract(Interactor);

	UsageCount++;

	OnInteractionCompleted.Broadcast(Interactor);
}

void AInteractableBase::OnInteract_Implementation(AActor* Interactor)
{
	// Override in subclasses
}

void AInteractableBase::AddCondition(UGimmickConditionBase* Condition)
{
	if (Condition)
	{
		Conditions.Add(Condition);
	}
}

bool AInteractableBase::CheckAllConditions(AActor* Interactor) const
{
	for (const UGimmickConditionBase* Condition : Conditions)
	{
		if (Condition && !Condition->IsMet(Interactor))
		{
			return false;
		}
	}
	return true;
}

bool AInteractableBase::IsInRange(AActor* Interactor) const
{
	if (!Interactor)
	{
		return false;
	}

	const float Distance = FVector::Dist(GetActorLocation(), Interactor->GetActorLocation());
	return Distance <= InteractionRange;
}
