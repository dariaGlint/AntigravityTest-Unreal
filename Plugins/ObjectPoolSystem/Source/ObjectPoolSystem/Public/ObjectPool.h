// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
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

	/** アクティブなオブジェクト数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 ActiveObjects = 0;

	/** 非アクティブなオブジェクト数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 InactiveObjects = 0;

	/** これまでのピークアクティブ数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 PeakActiveObjects = 0;

	/** 総スポーン回数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 TotalSpawnCount = 0;

	/** プール拡張回数 */
	UPROPERTY(BlueprintReadOnly, Category = "Pool Stats")
	int32 ExpandCount = 0;
};

/**
 * 個別のActorクラス用のオブジェクトプール
 * 同じクラスのActorを効率的に管理します
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPool : public UObject
{
	GENERATED_BODY()

public:
	UObjectPool();

	/**
	 * プールを初期化します
	 * @param InWorld ワールド参照
	 * @param InActorClass プールするActorクラス
	 * @param InitialSize 初期プールサイズ
	 * @param InMaxSize 最大プールサイズ（0で無制限）
	 */
	void Initialize(UWorld* InWorld, TSubclassOf<AActor> InActorClass, int32 InitialSize = 10, int32 InMaxSize = 0);

	/**
	 * プールからActorを取得します（なければ新規作成）
	 * @param SpawnTransform スポーン位置と回転
	 * @return プールされたActor
	 */
	AActor* Spawn(const FTransform& SpawnTransform);

	/**
	 * Actorをプールに返却します
	 * @param Actor 返却するActor
	 * @return 成功した場合true
	 */
	bool Despawn(AActor* Actor);

	/**
	 * プール内の全オブジェクトを破棄します
	 */
	void Clear();

	/**
	 * 指定数のオブジェクトを事前に生成します
	 * @param Count 生成数
	 */
	void Prewarm(int32 Count);

	/**
	 * プールの統計情報を取得します
	 * @return 統計情報
	 */
	FPoolStats GetStats() const;

	/**
	 * プールされているActorクラスを取得します
	 */
	TSubclassOf<AActor> GetActorClass() const { return ActorClass; }

private:
	/** プールするActorクラス */
	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	/** ワールド参照 */
	UPROPERTY()
	TObjectPtr<UWorld> World;

	/** 非アクティブなActorのプール */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> InactiveActors;

	/** アクティブなActorのセット */
	UPROPERTY()
	TSet<TObjectPtr<AActor>> ActiveActors;

	/** 最大プールサイズ（0で無制限） */
	int32 MaxPoolSize;

	/** 統計情報 */
	FPoolStats Stats;

	/**
	 * 新しいActorを生成します
	 * @return 生成されたActor
	 */
	AActor* CreateNewActor();
};
