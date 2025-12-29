#include "PuzzleCondition.h"
#include "PuzzleBase.h"

UPuzzleCondition::UPuzzleCondition()
{
	ConditionDescription = FText::FromString(TEXT("Requires puzzle to be solved"));
}

bool UPuzzleCondition::IsMet_Implementation(AActor* Interactor) const
{
	if (!TargetPuzzle)
	{
		return false;
	}

	return TargetPuzzle->IsSolved();
}
