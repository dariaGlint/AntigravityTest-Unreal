#pragma once

#include "CoreMinimal.h"
#include "WeatherTypes.generated.h"

// 天候状態の列挙型
UENUM(BlueprintType)
enum class EWeatherState : uint8
{
	Clear UMETA(DisplayName = "晴れ"),
	Cloudy UMETA(DisplayName = "曇り"),
	Rainy UMETA(DisplayName = "雨"),
	Storm UMETA(DisplayName = "嵐"),
	Snowy UMETA(DisplayName = "雪"),
	Foggy UMETA(DisplayName = "霧")
};

// 時間帯の列挙型
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Dawn UMETA(DisplayName = "夜明け"),
	Morning UMETA(DisplayName = "朝"),
	Noon UMETA(DisplayName = "昼"),
	Afternoon UMETA(DisplayName = "午後"),
	Evening UMETA(DisplayName = "夕方"),
	Night UMETA(DisplayName = "夜")
};

// 天候パラメータ
USTRUCT(BlueprintType)
struct FWeatherParameters
{
	GENERATED_BODY()

	// 視界への影響 (0.0 = 完全に見えない, 1.0 = クリア)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Gameplay", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VisibilityMultiplier;

	// 移動速度への影響 (0.0 = 移動不可, 1.0 = 通常速度)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Gameplay", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MovementSpeedMultiplier;

	// 風の強さ (0.0 = 無風, 1.0 = 最大)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Visual", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WindStrength;

	// 降水量 (0.0 = なし, 1.0 = 最大)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Visual", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PrecipitationIntensity;

	// 雷の頻度 (0.0 = なし, 1.0 = 頻繁)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Visual", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LightningFrequency;

	// 霧の濃度 (0.0 = なし, 1.0 = 最大)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Visual", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FogDensity;

	FWeatherParameters()
		: VisibilityMultiplier(1.0f)
		, MovementSpeedMultiplier(1.0f)
		, WindStrength(0.0f)
		, PrecipitationIntensity(0.0f)
		, LightningFrequency(0.0f)
		, FogDensity(0.0f)
	{}
};

// 天候状態の設定
USTRUCT(BlueprintType)
struct FWeatherStateConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EWeatherState WeatherState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FWeatherParameters Parameters;

	// この天候状態の最小継続時間（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
	float MinDuration;

	// この天候状態の最大継続時間（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
	float MaxDuration;

	FWeatherStateConfig()
		: WeatherState(EWeatherState::Clear)
		, MinDuration(300.0f)
		, MaxDuration(600.0f)
	{}
};
