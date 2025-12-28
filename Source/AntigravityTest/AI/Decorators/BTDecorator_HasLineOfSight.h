#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasLineOfSight.generated.h"

/**
 * Checks if the AI controller has line of sight to the specified target actor.
 */
UCLASS()
class ANTIGRAVITYTEST_API UBTDecorator_HasLineOfSight : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasLineOfSight();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/** Blackboard key that holds the target actor to check line of sight to */
	UPROPERTY(EditAnywhere, Category = "Condition")
	FBlackboardKeySelector TargetKey;
};
