#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "PuzzleBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleReset);

/**
 * パズルの基底クラス
 * Base class for puzzles
 */
UCLASS(Abstract, Blueprintable)
class GIMMICKSYSTEM_API APuzzleBase : public AInteractableBase
{
	GENERATED_BODY()

public:
	APuzzleBase();

	virtual void OnInteract_Implementation(AActor* Interactor) override;

	/**
	 * パズルが解かれたかチェック
	 * Check if puzzle is solved
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Puzzle")
	bool IsSolved() const { return bIsSolved; }

	/**
	 * パズルをリセット
	 * Reset the puzzle
	 */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void ResetPuzzle();

	/**
	 * パズルを解決済みにする
	 * Mark puzzle as solved
	 */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void SolvePuzzle();

	/**
	 * パズルの検証（サブクラスで実装）
	 * Validate puzzle solution (implement in subclasses)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Puzzle")
	bool ValidateSolution();
	virtual bool ValidateSolution_Implementation();

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FOnPuzzleSolved OnPuzzleSolved;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FOnPuzzleReset OnPuzzleReset;

protected:
	/** 解決済みフラグ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	bool bIsSolved;

	/** リセット可能か */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bCanReset;
};
