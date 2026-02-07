#pragma once

#include "CoreMinimal.h"
#include "GimmickConditionBase.h"
#include "GameplayTagContainer.h"
#include "KeyCondition.generated.h"

/**
 * 鍵を持っているかチェックする条件
 * Condition that checks if the interactor has a specific key (using Gameplay Tags)
 */
UCLASS()
class GIMMICKSYSTEM_API UKeyCondition : public UGimmickConditionBase
{
	GENERATED_BODY()

public:
	UKeyCondition();

	virtual bool IsMet_Implementation(AActor* Interactor) const override;

protected:
	/** 必要な鍵のタグ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FGameplayTag RequiredKeyTag;

	/**
	 * アクターが鍵を持っているかチェック
	 * Checks if the actor has the required key tag (via interface or component)
	 */
	UFUNCTION(BlueprintCallable, Category = "Key")
	bool HasKey(AActor* Actor) const;
};
