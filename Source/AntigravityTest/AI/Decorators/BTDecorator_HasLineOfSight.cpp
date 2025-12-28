#include "AI/Decorators/BTDecorator_HasLineOfSight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

UBTDecorator_HasLineOfSight::UBTDecorator_HasLineOfSight()
{
	NodeName = "Has Line Of Sight";
	
	// Filter the Blackboard Key Selector to only allow Actor keys
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_HasLineOfSight, TargetKey), AActor::StaticClass());
}

bool UBTDecorator_HasLineOfSight::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	// Get the target actor from the blackboard
	UObject* KeyValue = BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(KeyValue);

	if (!TargetActor)
	{
		return false;
	}

	// Check line of sight
	return AIController->LineOfSightTo(TargetActor);
}
