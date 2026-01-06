#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeatherTypes.h"
#include "WeatherAffectedComponent.generated.h"

/**
 * アクターに追加して天候の影響を受けるようにするコンポーネント
 * 視界、移動速度などのゲームプレイパラメータが自動的に天候によって調整される
 */
UCLASS(ClassGroup=(Weather), meta=(BlueprintSpawnableComponent))
class WEATHERSYSTEM_API UWeatherAffectedComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeatherAffectedComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 天候による影響を受けるかどうか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	bool bAffectedByWeather;

	// 視界への影響を受けるかどうか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Effects")
	bool bAffectedByVisibility;

	// 移動速度への影響を受けるかどうか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Effects")
	bool bAffectedByMovementSpeed;

	// 現在の視界倍率を取得
	UFUNCTION(BlueprintPure, Category = "Weather")
	float GetCurrentVisibilityMultiplier() const { return CurrentVisibilityMultiplier; }

	// 現在の移動速度倍率を取得
	UFUNCTION(BlueprintPure, Category = "Weather")
	float GetCurrentMovementSpeedMultiplier() const { return CurrentMovementSpeedMultiplier; }

	// 現在の天候パラメータを取得
	UFUNCTION(BlueprintPure, Category = "Weather")
	FWeatherParameters GetCurrentWeatherParameters() const { return CurrentWeatherParameters; }

protected:
	UFUNCTION()
	void OnWeatherParametersUpdated(const FWeatherParameters& NewParameters);

private:
	UPROPERTY()
	FWeatherParameters CurrentWeatherParameters;

	UPROPERTY()
	float CurrentVisibilityMultiplier;

	UPROPERTY()
	float CurrentMovementSpeedMultiplier;

	void UpdateEffects();
};
