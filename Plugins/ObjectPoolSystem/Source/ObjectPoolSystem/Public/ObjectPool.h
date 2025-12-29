// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "IPoolable.h"
#include "ObjectPool.generated.h"

/**
 * プールの統計情報
 */
USTRUCT(BlueprintType)
struct FPoolStats
{
	GENERATED_BODY()

	/** プール内の総オブジェクト数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 TotalObjects = 0;

	/** 現在アクティブなオブジェクト数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 ActiveObjects = 0;

	/** プール内で利用可能なオブジェクト数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 InactiveObjects = 0;

	/** これまでのアクティブオブジェクト数のピーク */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 PeakActiveObjects = 0;

	/** プールからのスポーン総回数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 TotalSpawnCount = 0;

	/** プールの自動拡張回数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 PoolExpansionCount = 0;
};

/**
 * 特定のActorクラス用のオブジェクトプール
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPool : public UObject
{
	GENERATED_BODY()

public:
	UObjectPool();

	/**
	 * プールを初期化する
	 * @param InWorld プールを作成するワールド
	 * @param InActorClass プールするActorのクラス
	 * @param InInitialSize 初期プールサイズ
	 * @param InMaxSize 最大プールサイズ（0で無制限）
	 */
	void Initialize(UWorld* InWorld, TSubclassOf<AActor> InActorClass, int32 InInitialSize = 10, int32 InMaxSize = 0);

	/**
	 * プールから新しいActorを取得する（なければ新規作成）
	 * @param SpawnTransform スポーン時のトランスフォーム
	 * @return プールから取得したActor
	 */
	AActor* Spawn(const FTransform& SpawnTransform);

	/**
	 * Actorをプールに戻す
	 * @param Actor プールに戻すActor
	 * @return 成功した場合true
	 */
	bool Despawn(AActor* Actor);

	/**
	 * プールを事前に温める（指定数のオブジェクトを事前生成）
	 * @param Count 生成するオブジェクトの数
	 */
	void Prewarm(int32 Count);

	/**
	 * プールをクリアする（すべてのオブジェクトを破棄）
	 */
	void Clear();

	/**
	 * プールの統計情報を取得する
	 * @return プールの統計情報
	 */
	FPoolStats GetStats() const;

	/**
	 * プールの統計情報をログに出力する
	 */
	void LogStats() const;

private:
	/** プールするActorのクラス */
	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	/** プールが存在するワールド */
	UPROPERTY()
	TWeakObjectPtr<UWorld> World;

	/** 利用可能なActorのプール */
	UPROPERTY()
	TArray<AActor*> AvailableActors;

	/** 現在アクティブなActorの配列 */
	UPROPERTY()
	TArray<AActor*> ActiveActors;

	/** 最大プールサイズ（0で無制限） */
	int32 MaxPoolSize;

	/** 統計情報 */
	FPoolStats Stats;

	/** 新しいActorを作成する */
	AActor* CreateNewActor();
};
