#include "PuzzleBase.h"

APuzzleBase::APuzzleBase()
{
	bIsSolved = false;
	bCanReset = true;
	InteractionPrompt = FText::FromString(TEXT("Examine Puzzle"));
}

void APuzzleBase::OnInteract_Implementation(AActor* Interactor)
{
	Super::OnInteract_Implementation(Interactor);

	if (!bIsSolved && ValidateSolution())
	{
		SolvePuzzle();
	}
}

void APuzzleBase::ResetPuzzle()
{
	if (bCanReset)
	{
		bIsSolved = false;
		OnPuzzleReset.Broadcast();
		OnStateChanged.Broadcast();
	}
}

void APuzzleBase::SolvePuzzle()
{
	if (!bIsSolved)
	{
		bIsSolved = true;
		OnPuzzleSolved.Broadcast();
		OnStateChanged.Broadcast();
	}
}

bool APuzzleBase::ValidateSolution_Implementation()
{
	// Override in subclasses
	return false;
}
