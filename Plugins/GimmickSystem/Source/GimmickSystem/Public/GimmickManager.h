#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GimmickManager.generated.h"

class AInteractableBase;
class ADoorGimmick;
class APuzzleBase;

/**
 * ギミック管理システム
 * Global manager for all gimmicks and interactable objects
 */
UCLASS()
class GIMMICKSYSTEM_API UGimmickManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * ギミックを登録
	 * Register a gimmick with the manager
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void RegisterGimmick(AInteractableBase* Gimmick);

	/**
	 * ギミックの登録を解除
	 * Unregister a gimmick
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void UnregisterGimmick(AInteractableBase* Gimmick);

	/**
	 * IDでギミックを検索
	 * Find a gimmick by its unique ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	AInteractableBase* FindGimmickByID(FName GimmickID) const;

	/**
	 * すべてのドアを取得
	 * Get all registered doors
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	TArray<ADoorGimmick*> GetAllDoors() const;

	/**
	 * すべてのパズルを取得
	 * Get all registered puzzles
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	TArray<APuzzleBase*> GetAllPuzzles() const;

	/**
	 * プレイヤーに最も近いインタラクション可能なギミックを取得
	 * Get the nearest interactable gimmick to the player
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	AInteractableBase* GetNearestInteractable(AActor* Interactor, float MaxDistance = 500.0f) const;

	/**
	 * すべてのギミックの状態をセーブ用のデータに変換
	 * Convert all gimmick states to save data (for save system integration)
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Save")
	void SaveAllGimmickStates();

	/**
	 * セーブデータからギミックの状態を復元
	 * Restore gimmick states from save data
	 */
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Save")
	void LoadAllGimmickStates();

protected:
	/** 登録されたすべてのギミック */
	UPROPERTY()
	TArray<TObjectPtr<AInteractableBase>> RegisteredGimmicks;

	/** IDでアクセスするためのマップ */
	UPROPERTY()
	TMap<FName, TObjectPtr<AInteractableBase>> GimmickMap;
};
