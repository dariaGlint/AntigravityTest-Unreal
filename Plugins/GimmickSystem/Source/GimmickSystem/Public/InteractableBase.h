#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

class UGimmickConditionBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionStarted, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionCompleted, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFailed, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateChanged);

/**
 * インタラクション可能オブジェクトの基底クラス
 * Base class for all interactable objects in the game
 */
UCLASS(Abstract, Blueprintable)
class GIMMICKSYSTEM_API AInteractableBase : public AActor
{
	GENERATED_BODY()

public:
	AInteractableBase();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * インタラクション可能かチェック
	 * Check if this object can be interacted with
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;
	virtual bool CanInteract_Implementation(AActor* Interactor) const;

	/**
	 * インタラクションを実行
	 * Execute the interaction
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

	/**
	 * インタラクション処理の実装（サブクラスでオーバーライド）
	 * Implement interaction logic (override in subclasses)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(AActor* Interactor);
	virtual void OnInteract_Implementation(AActor* Interactor);

	/**
	 * インタラクション表示名を取得
	 * Get the display name for interaction prompts
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	FText GetInteractionPrompt() const { return InteractionPrompt; }

	/**
	 * 条件を追加
	 * Add a condition that must be met to interact
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void AddCondition(UGimmickConditionBase* Condition);

	/**
	 * すべての条件を満たしているかチェック
	 * Check if all conditions are met
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CheckAllConditions(AActor* Interactor) const;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionStarted OnInteractionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionCompleted OnInteractionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionFailed OnInteractionFailed;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnStateChanged OnStateChanged;

protected:
	/** インタラクション表示テキスト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt;

	/** 現在有効か */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsEnabled;

	/** 繰り返し使用可能か */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanBeUsedMultipleTimes;

	/** 使用回数 */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	int32 UsageCount;

	/** インタラクション条件のリスト */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Interaction")
	TArray<TObjectPtr<UGimmickConditionBase>> Conditions;

	/** インタラクション範囲 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange;

	/**
	 * 範囲内チェック
	 * Check if interactor is within range
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInRange(AActor* Interactor) const;
};
