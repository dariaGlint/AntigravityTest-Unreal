#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InputCoreTypes.h"
#include "ControlSettings.generated.h"

/**
 * キーバインディング情報
 * Key binding information
 */
USTRUCT(BlueprintType)
struct FKeyBinding
{
	GENERATED_BODY()

	// アクション名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	FName ActionName;

	// 割り当てられたキー
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	FKey Key;

	// 修飾キー（Shift、Ctrl、Alt）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bCtrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bAlt;

	FKeyBinding()
		: ActionName(NAME_None)
		, Key(EKeys::Invalid)
		, bShift(false)
		, bCtrl(false)
		, bAlt(false)
	{}

	FKeyBinding(FName InActionName, FKey InKey)
		: ActionName(InActionName)
		, Key(InKey)
		, bShift(false)
		, bCtrl(false)
		, bAlt(false)
	{}
};

/**
 * コントロール設定を管理するクラス
 * Control settings management class
 */
UCLASS(BlueprintType)
class SETTINGSMANAGEMENT_API UControlSettings : public UObject
{
	GENERATED_BODY()

public:
	UControlSettings();

	// マウス感度 (0.1 - 2.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float MouseSensitivity;

	// マウスY軸反転
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bInvertMouseY;

	// コントローラー感度 (0.1 - 2.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float ControllerSensitivity;

	// コントローラーY軸反転
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bInvertControllerY;

	// エイムアシスト有効化
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bAimAssist;

	// バイブレーション有効化
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	bool bVibration;

	// キーバインディングのリスト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Settings")
	TArray<FKeyBinding> KeyBindings;

	/**
	 * デフォルト値に復元
	 * Reset to default values
	 */
	UFUNCTION(BlueprintCallable, Category = "Control Settings")
	void ResetToDefault();

	/**
	 * キーバインディングを設定
	 * Set key binding
	 */
	UFUNCTION(BlueprintCallable, Category = "Control Settings")
	void SetKeyBinding(FName ActionName, FKey Key);

	/**
	 * キーバインディングを取得
	 * Get key binding
	 */
	UFUNCTION(BlueprintCallable, Category = "Control Settings")
	FKey GetKeyBinding(FName ActionName) const;

	/**
	 * 設定を適用
	 * Apply settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Control Settings")
	void ApplySettings();

	/**
	 * 設定のコピーを作成
	 * Create a copy of settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Control Settings")
	UControlSettings* CreateCopy() const;

private:
	void InitializeDefaultKeyBindings();
};
