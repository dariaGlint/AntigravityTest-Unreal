#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherTypes.h"
#include "WeatherManager.generated.h"

// 天候変化時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherChanged, EWeatherState, OldWeather, EWeatherState, NewWeather);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherParametersUpdated, const FWeatherParameters&, NewParameters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeOfDayChanged, ETimeOfDay, OldTime, ETimeOfDay, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightningStrike, FVector, StrikeLocation);

/**
 * 天候システムを管理するワールドサブシステム
 * 天候状態の遷移、時間帯の変化、ゲームプレイへの影響を処理
 */
UCLASS()
class WEATHERSYSTEM_API UWeatherManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	// 天候状態の取得と設定
	UFUNCTION(BlueprintPure, Category = "Weather")
	EWeatherState GetCurrentWeather() const { return CurrentWeatherState; }

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeather(EWeatherState NewWeather, bool bInstant = false);

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void TransitionToWeather(EWeatherState NewWeather, float TransitionDuration = 5.0f);

	// 現在の天候パラメータを取得
	UFUNCTION(BlueprintPure, Category = "Weather")
	FWeatherParameters GetCurrentWeatherParameters() const { return CurrentParameters; }

	// 時間帯の取得と設定
	UFUNCTION(BlueprintPure, Category = "Weather")
	ETimeOfDay GetCurrentTimeOfDay() const { return CurrentTimeOfDay; }

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetTimeOfDay(ETimeOfDay NewTime);

	// 時間経過の有効化/無効化
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetTimeProgressionEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Weather")
	bool IsTimeProgressionEnabled() const { return bEnableTimeProgression; }

	// 時間の進行速度を設定（1.0 = リアルタイム）
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetTimeProgressionSpeed(float Speed);

	UFUNCTION(BlueprintPure, Category = "Weather")
	float GetTimeProgressionSpeed() const { return TimeProgressionSpeed; }

	// 天候の自動遷移の有効化/無効化
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetAutoWeatherTransitionEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Weather")
	bool IsAutoWeatherTransitionEnabled() const { return bEnableAutoWeatherTransition; }

	// 天候状態の設定を追加
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void AddWeatherStateConfig(const FWeatherStateConfig& Config);

	// デリゲート
	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherChanged OnWeatherChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherParametersUpdated OnWeatherParametersUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnTimeOfDayChanged OnTimeOfDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnLightningStrike OnLightningStrike;

protected:
	// 現在の天候状態
	UPROPERTY()
	EWeatherState CurrentWeatherState;

	// 遷移中の目標天候状態
	UPROPERTY()
	EWeatherState TargetWeatherState;

	// 現在の天候パラメータ
	UPROPERTY()
	FWeatherParameters CurrentParameters;

	// 遷移中の目標パラメータ
	UPROPERTY()
	FWeatherParameters TargetParameters;

	// 現在の時間帯
	UPROPERTY()
	ETimeOfDay CurrentTimeOfDay;

	// 天候状態の設定マップ
	UPROPERTY()
	TMap<EWeatherState, FWeatherStateConfig> WeatherConfigs;

	// 天候遷移の設定
	UPROPERTY(EditAnywhere, Category = "Weather")
	bool bEnableAutoWeatherTransition;

	UPROPERTY(EditAnywhere, Category = "Weather")
	bool bIsTransitioning;

	UPROPERTY(EditAnywhere, Category = "Weather")
	float TransitionProgress;

	UPROPERTY(EditAnywhere, Category = "Weather")
	float TransitionDuration;

	UPROPERTY(EditAnywhere, Category = "Weather")
	float CurrentWeatherDuration;

	UPROPERTY(EditAnywhere, Category = "Weather")
	float TargetWeatherDuration;

	// 時間経過の設定
	UPROPERTY(EditAnywhere, Category = "Time")
	bool bEnableTimeProgression;

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeProgressionSpeed;

	UPROPERTY(EditAnywhere, Category = "Time")
	float CurrentTimeOfDayProgress;

	// 雷の設定
	UPROPERTY(EditAnywhere, Category = "Weather|Lightning")
	float LightningTimer;

	UPROPERTY(EditAnywhere, Category = "Weather|Lightning")
	float LightningCheckInterval;

private:
	void UpdateWeatherTransition(float DeltaTime);
	void UpdateTimeOfDay(float DeltaTime);
	void UpdateLightning(float DeltaTime);
	void InitializeDefaultWeatherConfigs();
	FWeatherParameters GetWeatherParametersForState(EWeatherState State) const;
	EWeatherState ChooseNextWeatherState() const;
};
