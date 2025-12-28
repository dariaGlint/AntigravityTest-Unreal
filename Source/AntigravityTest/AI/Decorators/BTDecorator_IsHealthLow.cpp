#include "AI/Decorators/BTDecorator_IsHealthLow.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Variant_Combat/CombatCharacter.h"

UBTDecorator_IsHealthLow::UBTDecorator_IsHealthLow()
{
	NodeName = "Is Health Low";
}

bool UBTDecorator_IsHealthLow::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	// Try to cast to CombatCharacter to access health
	ACombatCharacter* CombatChar = Cast<ACombatCharacter>(ControlledPawn);
	if (CombatChar)
	{
//		if (CombatChar->MaxHP <= 0.0f)
		{
			return false; // Avoid division by zero
		}

//		float HealthPct = CombatChar->CurrentHP / CombatChar->MaxHP;
//		return HealthPct <= HealthThreshold;
	}

	// Fallback or other character types could be handled here
	return false;
}
