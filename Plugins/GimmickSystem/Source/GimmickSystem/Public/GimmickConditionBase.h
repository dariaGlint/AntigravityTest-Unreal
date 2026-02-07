#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GimmickConditionBase.generated.h"

/**
 * ギミック条件の基底クラス
 * Base class for all gimmick conditions (keys, puzzles, etc.)
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class GIMMICKSYSTEM_API UGimmickConditionBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 条件が満たされているかチェック
	 * Check if the condition is met
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Condition")
	bool IsMet(AActor* Interactor) const;
	virtual bool IsMet_Implementation(AActor* Interactor) const;

	/**
	 * 条件の説明を取得
	 * Get a description of this condition
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Condition")
	FText GetConditionDescription() const { return ConditionDescription; }

protected:
	/** 条件の説明文 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FText ConditionDescription;
};
