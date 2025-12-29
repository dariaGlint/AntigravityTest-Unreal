#pragma once

#include "CoreMinimal.h"
#include "GimmickConditionBase.h"
#include "PuzzleCondition.generated.h"

class APuzzleBase;

/**
 * パズルが解かれているかチェックする条件
 * Condition that checks if a specific puzzle has been solved
 */
UCLASS()
class GIMMICKSYSTEM_API UPuzzleCondition : public UGimmickConditionBase
{
	GENERATED_BODY()

public:
	UPuzzleCondition();

	virtual bool IsMet_Implementation(AActor* Interactor) const override;

protected:
	/** 参照するパズル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TObjectPtr<APuzzleBase> TargetPuzzle;
};
