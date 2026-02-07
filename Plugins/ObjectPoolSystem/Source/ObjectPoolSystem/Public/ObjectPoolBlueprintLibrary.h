// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObjectPool.h"
#include "ObjectPoolBlueprintLibrary.generated.h"

/**
 * Blueprint用のオブジェクトプール関数ライブラリ
 * Blueprintから簡単にオブジェクトプールを使用できます
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPoolBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * プールからActorをスポーンします
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass スポーンするActorクラス
	 * @param Location スポーン位置
	 * @param Rotation スポーン回転
	 * @param InitialPoolSize プールの初期サイズ（新規作成時のみ）
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 * @return スポーンされたActor
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static AActor* SpawnPooledActor(
		UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass,
		FVector Location,
		FRotator Rotation,
		int32 InitialPoolSize = 10,
		int32 MaxPoolSize = 0
	);

	/**
	 * Actorをプールに返却します
	 * @param Actor 返却するActor
	 * @return 成功した場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	static bool DespawnPooledActor(AActor* Actor);

	/**
	 * 指定クラスのプールを事前に生成します
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass Actorクラス
	 * @param Count 生成数
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void PrewarmPool(
		UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass,
		int32 Count,
		int32 MaxPoolSize = 0
	);

	/**
	 * 指定クラスのプール統計を取得します
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass Actorクラス
	 * @param OutStats 統計情報
	 * @return プールが存在する場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static bool GetPoolStats(
		UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass,
		FPoolStats& OutStats
	);

	/**
	 * すべてのプールの統計をログに出力します
	 * @param WorldContextObject ワールドコンテキスト
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void LogPoolStats(UObject* WorldContextObject);

	/**
	 * 指定クラスのプールをクリアします
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass Actorクラス
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void ClearPool(
		UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass
	);

	/**
	 * すべてのプールをクリアします
	 * @param WorldContextObject ワールドコンテキスト
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void ClearAllPools(UObject* WorldContextObject);
};
