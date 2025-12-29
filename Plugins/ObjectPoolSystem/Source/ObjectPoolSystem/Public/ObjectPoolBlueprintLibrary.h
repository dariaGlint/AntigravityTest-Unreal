// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObjectPool.h"
#include "ObjectPoolBlueprintLibrary.generated.h"

/**
 * Blueprint用のオブジェクトプール関数ライブラリ
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPoolBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * プールからActorをスポーンする
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass スポーンするActorのクラス
	 * @param SpawnTransform スポーン時のトランスフォーム
	 * @param InitialPoolSize プールが存在しない場合の初期サイズ
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 * @return スポーンされたActor
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static AActor* SpawnPooledActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, int32 InitialPoolSize = 10, int32 MaxPoolSize = 0);

	/**
	 * Actorをプールに戻す（デスポーン）
	 * @param WorldContextObject ワールドコンテキスト
	 * @param Actor プールに戻すActor
	 * @return 成功した場合true
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static bool DespawnPooledActor(UObject* WorldContextObject, AActor* Actor);

	/**
	 * プールを事前に温める
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass プールするActorのクラス
	 * @param Count 生成するオブジェクトの数
	 * @param InitialPoolSize プールが存在しない場合の初期サイズ
	 * @param MaxPoolSize 最大プールサイズ（0で無制限）
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void PrewarmPool(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, int32 Count, int32 InitialPoolSize = 10, int32 MaxPoolSize = 0);

	/**
	 * プールをクリアする
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass クリアするプールのActorクラス
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void ClearPool(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass);

	/**
	 * すべてのプールをクリアする
	 * @param WorldContextObject ワールドコンテキスト
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void ClearAllPools(UObject* WorldContextObject);

	/**
	 * プールの統計情報を取得する
	 * @param WorldContextObject ワールドコンテキスト
	 * @param ActorClass 統計情報を取得するActorクラス
	 * @return プールの統計情報
	 */
	UFUNCTION(BlueprintPure, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static FPoolStats GetPoolStats(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass);

	/**
	 * すべてのプールの統計情報をログに出力する
	 * @param WorldContextObject ワールドコンテキスト
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (WorldContext = "WorldContextObject"))
	static void LogPoolStats(UObject* WorldContextObject);
};
