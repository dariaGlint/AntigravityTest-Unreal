// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPool.h"
#include "ObjectPoolManagerSubsystem.generated.h"

/**
 * ワールド内の全オブジェクトプールを管理するサブシステム
 * Actorクラスごとにプールを自動的に作成・管理します
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPoolManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End of USubsystem interface

	/**
	 * プールからActorをスポーンします（プールがない場合は自動作成）
	 * @param ActorClass スポーンするActorクラス
	 * @param SpawnTransform スポーン位置と回転
	 * @param InitialPoolSize プールの初期サイズ（新規作成時のみ）
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 * @return スポーンされたActor
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	AActor* SpawnFromPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, int32 InitialPoolSize = 10, int32 MaxPoolSize = 0);

	/**
	 * Actorをプールに返却します
	 * @param Actor 返却するActor
	 * @return 成功した場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	bool ReturnToPool(AActor* Actor);

	/**
	 * 指定クラスのプールを事前に生成します
	 * @param ActorClass Actorクラス
	 * @param Count 生成数
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void PrewarmPool(TSubclassOf<AActor> ActorClass, int32 Count, int32 MaxPoolSize = 0);

	/**
	 * 指定クラスのプールをクリアします
	 * @param ActorClass Actorクラス
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ClearPool(TSubclassOf<AActor> ActorClass);

	/**
	 * すべてのプールをクリアします
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ClearAllPools();

	/**
	 * 指定クラスのプール統計を取得します
	 * @param ActorClass Actorクラス
	 * @param OutStats 統計情報
	 * @return プールが存在する場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	bool GetPoolStats(TSubclassOf<AActor> ActorClass, FPoolStats& OutStats) const;

	/**
	 * すべてのプールの統計情報を取得します
	 * @return クラス名と統計情報のマップ
	 */
	TMap<TSubclassOf<AActor>, FPoolStats> GetAllPoolStats() const;

	/**
	 * すべてのプールの統計をログに出力します
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void LogAllPoolStats() const;

private:
	/** Actorクラスごとのプール */
	UPROPERTY()
	TMap<TSubclassOf<AActor>, TObjectPtr<UObjectPool>> Pools;

	/** Actorとそのプールの対応マップ */
	UPROPERTY()
	TMap<TObjectPtr<AActor>, TObjectPtr<UObjectPool>> ActorToPoolMap;

	/**
	 * 指定クラスのプールを取得または作成します
	 * @param ActorClass Actorクラス
	 * @param InitialSize 初期サイズ（新規作成時のみ）
	 * @param MaxSize 最大サイズ（新規作成時のみ）
	 * @return プール
	 */
	UObjectPool* GetOrCreatePool(TSubclassOf<AActor> ActorClass, int32 InitialSize = 10, int32 MaxSize = 0);
};
