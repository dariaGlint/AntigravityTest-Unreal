// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPoolable.generated.h"

/**
 * プール可能なオブジェクトのためのインターフェース
 * このインターフェースを実装することで、オブジェクトプールシステムで管理できるようになります
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

class IPoolable
{
	GENERATED_BODY()

public:
	/**
	 * プールからアクティブ化される時に呼ばれる
	 * オブジェクトの初期化や有効化処理をここで行います
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pool")
	void OnPoolActivate();
	virtual void OnPoolActivate_Implementation() {}

	/**
	 * プールに戻される時に呼ばれる
	 * オブジェクトの状態リセットや無効化処理をここで行います
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pool")
	void OnPoolDeactivate();
	virtual void OnPoolDeactivate_Implementation() {}

	/**
	 * プールに戻すことができるかどうかをチェック
	 * @return trueの場合、プールに戻すことができます
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pool")
	bool CanBePooled() const;
	virtual bool CanBePooled_Implementation() const { return true; }
};
