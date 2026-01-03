#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventFlagManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEventFlagChanged, FName, FlagName, bool, bNewValue);

/**
 * イベントフラグを管理するゲームインスタンスサブシステム
 */
UCLASS()
class NPCRELATIONSHIPSYSTEM_API UEventFlagManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// デリゲート
	UPROPERTY(BlueprintAssignable, Category = "EventFlag")
	FOnEventFlagChanged OnEventFlagChanged;

	// イベントフラグの設定
	UFUNCTION(BlueprintCallable, Category = "EventFlag")
	void SetEventFlag(FName FlagName, bool bValue);

	// イベントフラグの取得
	UFUNCTION(BlueprintPure, Category = "EventFlag")
	bool GetEventFlag(FName FlagName) const;

	// イベントフラグのトグル
	UFUNCTION(BlueprintCallable, Category = "EventFlag")
	void ToggleEventFlag(FName FlagName);

	// イベントフラグをクリア
	UFUNCTION(BlueprintCallable, Category = "EventFlag")
	void ClearEventFlag(FName FlagName);

	// すべてのイベントフラグをクリア
	UFUNCTION(BlueprintCallable, Category = "EventFlag")
	void ClearAllEventFlags();

	// イベントフラグの存在チェック
	UFUNCTION(BlueprintPure, Category = "EventFlag")
	bool HasEventFlag(FName FlagName) const;

	// すべてのイベントフラグを取得
	UFUNCTION(BlueprintPure, Category = "EventFlag")
	TMap<FName, bool> GetAllEventFlags() const;

	// 複数のフラグがすべてtrueかチェック（AND条件）
	UFUNCTION(BlueprintPure, Category = "EventFlag")
	bool CheckAllFlags(const TArray<FName>& FlagNames) const;

	// 複数のフラグのいずれかがtrueかチェック（OR条件）
	UFUNCTION(BlueprintPure, Category = "EventFlag")
	bool CheckAnyFlag(const TArray<FName>& FlagNames) const;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	// イベントフラグのマップ
	UPROPERTY(SaveGame)
	TMap<FName, bool> EventFlags;
};
