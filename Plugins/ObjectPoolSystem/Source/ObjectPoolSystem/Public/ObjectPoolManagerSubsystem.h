// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPool.h"
#include "ObjectPoolManagerSubsystem.generated.h"

/**
 * ワールド内のすべてのオブジェクトプールを管理するサブシステム
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPoolManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** USubsystem implementation */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * プールからActorをスポーンする
	 * 指定されたクラスのプールが存在しない場合は自動的に作成します
	 * @param ActorClass スポーンするActorのクラス
	 * @param SpawnTransform スポーン時のトランスフォーム
	 * @param InitialPoolSize プールが存在しない場合の初期サイズ
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 * @return スポーンされたActor
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	AActor* SpawnFromPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, int32 InitialPoolSize = 10, int32 MaxPoolSize = 0);

	/**
	 * Actorをプールに戻す
	 * @param Actor プールに戻すActor
	 * @return 成功した場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	bool ReturnToPool(AActor* Actor);

	/**
	 * 指定されたクラスのプールを事前に温める
	 * @param ActorClass プールするActorのクラス
	 * @param Count 生成するオブジェクトの数
	 * @param InitialPoolSize プールが存在しない場合の初期サイズ
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void PrewarmPool(TSubclassOf<AActor> ActorClass, int32 Count, int32 InitialPoolSize = 10, int32 MaxPoolSize = 0);

	/**
	 * 指定されたクラスのプールをクリアする
	 * @param ActorClass クリアするプールのActorクラス
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ClearPool(TSubclassOf<AActor> ActorClass);

	/**
	 * すべてのプールをクリアする
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ClearAllPools();

	/**
	 * 指定されたクラスのプールの統計情報を取得する
	 * @param ActorClass 統計情報を取得するActorクラス
	 * @return プールの統計情報
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	FPoolStats GetPoolStats(TSubclassOf<AActor> ActorClass) const;

	/**
	 * すべてのプールの統計情報を取得する
	 * @return すべてのプールの統計情報のマップ
	 */
	TMap<TSubclassOf<AActor>, FPoolStats> GetAllPoolStats() const;

	/**
	 * すべてのプールの統計情報をログに出力する
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void LogAllPoolStats() const;

private:
	/** Actorクラスごとのプール */
	UPROPERTY()
	TMap<TSubclassOf<AActor>, UObjectPool*> Pools;

	/** 指定されたクラスのプールを取得または作成する */
	UObjectPool* GetOrCreatePool(TSubclassOf<AActor> ActorClass, int32 InitialPoolSize, int32 MaxPoolSize);

	/** Actorが属するプールを見つける */
	UObjectPool* FindPoolForActor(AActor* Actor) const;
};
