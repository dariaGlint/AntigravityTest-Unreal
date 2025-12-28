#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsHealthLow.generated.h"

/**
 * Checks if the controlled pawn's health is below a certain percentage.
 */
UCLASS()
class ANTIGRAVITYTEST_API UBTDecorator_IsHealthLow : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsHealthLow();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/** The health percentage threshold (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThreshold = 0.3f;
};
