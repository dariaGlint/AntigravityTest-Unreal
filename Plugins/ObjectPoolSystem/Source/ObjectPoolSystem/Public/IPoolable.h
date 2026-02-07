// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPoolable.generated.h"

/**
 * プール可能なオブジェクトのインターフェース
 * このインターフェースを実装することで、ActorをObjectPoolで管理できるようになります
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * IPoolable インターフェース
 * プールから取り出された時、返却された時の処理を定義します
 */
class IPoolable
{
	GENERATED_BODY()

public:
	/**
	 * プールから取り出され、アクティブ化される時に呼ばれます
	 * ここでActorの初期化や状態のリセットを行います
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pool")
	void OnPoolActivate();
	virtual void OnPoolActivate_Implementation() {}

	/**
	 * プールに返却され、非アクティブ化される時に呼ばれます
	 * ここでActorの状態をクリーンアップします
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pool")
	void OnPoolDeactivate();
	virtual void OnPoolDeactivate_Implementation() {}

	/**
	 * このActorがプールに返却可能かどうかを判定します
	 * @return プールに返却可能な場合true
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pool")
	bool CanBePooled() const;
	virtual bool CanBePooled_Implementation() const { return true; }
};
