#include "WeatherManager.h"

void UWeatherManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// デフォルト値の初期化
	CurrentWeatherState = EWeatherState::Clear;
	TargetWeatherState = EWeatherState::Clear;
	CurrentTimeOfDay = ETimeOfDay::Noon;

	bEnableAutoWeatherTransition = true;
	bIsTransitioning = false;
	TransitionProgress = 0.0f;
	TransitionDuration = 5.0f;
	CurrentWeatherDuration = 0.0f;
	TargetWeatherDuration = 300.0f;

	bEnableTimeProgression = true;
	TimeProgressionSpeed = 1.0f;
	CurrentTimeOfDayProgress = 0.0f;

	LightningTimer = 0.0f;
	LightningCheckInterval = 1.0f;

	// デフォルトの天候設定を初期化
	InitializeDefaultWeatherConfigs();

	// 初期パラメータを設定
	CurrentParameters = GetWeatherParametersForState(CurrentWeatherState);
	TargetParameters = CurrentParameters;
}

void UWeatherManager::Deinitialize()
{
	Super::Deinitialize();
}

void UWeatherManager::Tick(float DeltaTime)
{
	if (!GetWorld())
		return;

	// 天候遷移の更新
	if (bIsTransitioning)
	{
		UpdateWeatherTransition(DeltaTime);
	}
	else if (bEnableAutoWeatherTransition)
	{
		// 自動天候遷移の処理
		CurrentWeatherDuration += DeltaTime;
		if (CurrentWeatherDuration >= TargetWeatherDuration)
		{
			EWeatherState NextWeather = ChooseNextWeatherState();
			TransitionToWeather(NextWeather, 10.0f);
		}
	}

	// 時間帯の更新
	if (bEnableTimeProgression)
	{
		UpdateTimeOfDay(DeltaTime);
	}

	// 雷の更新
	UpdateLightning(DeltaTime);
}

bool UWeatherManager::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UWeatherManager::SetWeather(EWeatherState NewWeather, bool bInstant)
{
	if (CurrentWeatherState == NewWeather)
		return;

	EWeatherState OldWeather = CurrentWeatherState;

	if (bInstant)
	{
		CurrentWeatherState = NewWeather;
		CurrentParameters = GetWeatherParametersForState(NewWeather);
		TargetParameters = CurrentParameters;
		bIsTransitioning = false;
		CurrentWeatherDuration = 0.0f;

		// 新しい継続時間を設定
		if (WeatherConfigs.Contains(NewWeather))
		{
			const FWeatherStateConfig& Config = WeatherConfigs[NewWeather];
			TargetWeatherDuration = FMath::RandRange(Config.MinDuration, Config.MaxDuration);
		}

		OnWeatherChanged.Broadcast(OldWeather, NewWeather);
		OnWeatherParametersUpdated.Broadcast(CurrentParameters);
	}
	else
	{
		TransitionToWeather(NewWeather, 5.0f);
	}
}

void UWeatherManager::TransitionToWeather(EWeatherState NewWeather, float Duration)
{
	if (CurrentWeatherState == NewWeather && !bIsTransitioning)
		return;

	TargetWeatherState = NewWeather;
	TargetParameters = GetWeatherParametersForState(NewWeather);
	TransitionDuration = FMath::Max(Duration, 0.1f);
	TransitionProgress = 0.0f;
	bIsTransitioning = true;
}

void UWeatherManager::SetTimeOfDay(ETimeOfDay NewTime)
{
	if (CurrentTimeOfDay == NewTime)
		return;

	ETimeOfDay OldTime = CurrentTimeOfDay;
	CurrentTimeOfDay = NewTime;
	CurrentTimeOfDayProgress = 0.0f;

	OnTimeOfDayChanged.Broadcast(OldTime, NewTime);
}

void UWeatherManager::SetTimeProgressionEnabled(bool bEnabled)
{
	bEnableTimeProgression = bEnabled;
}

void UWeatherManager::SetTimeProgressionSpeed(float Speed)
{
	TimeProgressionSpeed = FMath::Max(Speed, 0.0f);
}

void UWeatherManager::SetAutoWeatherTransitionEnabled(bool bEnabled)
{
	bEnableAutoWeatherTransition = bEnabled;
}

void UWeatherManager::AddWeatherStateConfig(const FWeatherStateConfig& Config)
{
	WeatherConfigs.Add(Config.WeatherState, Config);
}

void UWeatherManager::UpdateWeatherTransition(float DeltaTime)
{
	TransitionProgress += DeltaTime / TransitionDuration;

	if (TransitionProgress >= 1.0f)
	{
		// 遷移完了
		TransitionProgress = 1.0f;
		EWeatherState OldWeather = CurrentWeatherState;
		CurrentWeatherState = TargetWeatherState;
		CurrentParameters = TargetParameters;
		bIsTransitioning = false;
		CurrentWeatherDuration = 0.0f;

		// 新しい継続時間を設定
		if (WeatherConfigs.Contains(CurrentWeatherState))
		{
			const FWeatherStateConfig& Config = WeatherConfigs[CurrentWeatherState];
			TargetWeatherDuration = FMath::RandRange(Config.MinDuration, Config.MaxDuration);
		}

		OnWeatherChanged.Broadcast(OldWeather, CurrentWeatherState);
		OnWeatherParametersUpdated.Broadcast(CurrentParameters);
	}
	else
	{
		// パラメータを補間
		float Alpha = TransitionProgress;

		FWeatherParameters StartParams = GetWeatherParametersForState(CurrentWeatherState);

		CurrentParameters.VisibilityMultiplier = FMath::Lerp(StartParams.VisibilityMultiplier, TargetParameters.VisibilityMultiplier, Alpha);
		CurrentParameters.MovementSpeedMultiplier = FMath::Lerp(StartParams.MovementSpeedMultiplier, TargetParameters.MovementSpeedMultiplier, Alpha);
		CurrentParameters.WindStrength = FMath::Lerp(StartParams.WindStrength, TargetParameters.WindStrength, Alpha);
		CurrentParameters.PrecipitationIntensity = FMath::Lerp(StartParams.PrecipitationIntensity, TargetParameters.PrecipitationIntensity, Alpha);
		CurrentParameters.LightningFrequency = FMath::Lerp(StartParams.LightningFrequency, TargetParameters.LightningFrequency, Alpha);
		CurrentParameters.FogDensity = FMath::Lerp(StartParams.FogDensity, TargetParameters.FogDensity, Alpha);

		OnWeatherParametersUpdated.Broadcast(CurrentParameters);
	}
}

void UWeatherManager::UpdateTimeOfDay(float DeltaTime)
{
	// 1日 = 24時間 = 1440分
	// 各時間帯を240分（4時間）とする
	const float TimePerPeriod = 240.0f * 60.0f; // 秒単位

	CurrentTimeOfDayProgress += DeltaTime * TimeProgressionSpeed;

	if (CurrentTimeOfDayProgress >= TimePerPeriod)
	{
		CurrentTimeOfDayProgress = 0.0f;

		ETimeOfDay OldTime = CurrentTimeOfDay;
		int32 NextTimeIndex = (static_cast<int32>(CurrentTimeOfDay) + 1) % 6;
		CurrentTimeOfDay = static_cast<ETimeOfDay>(NextTimeIndex);

		OnTimeOfDayChanged.Broadcast(OldTime, CurrentTimeOfDay);
	}
}

void UWeatherManager::UpdateLightning(float DeltaTime)
{
	if (CurrentParameters.LightningFrequency <= 0.0f)
		return;

	LightningTimer += DeltaTime;

	if (LightningTimer >= LightningCheckInterval)
	{
		LightningTimer = 0.0f;

		// 雷が発生するかチェック
		float RandomValue = FMath::FRand();
		if (RandomValue < CurrentParameters.LightningFrequency * 0.1f)
		{
			// ランダムな位置で雷を発生
			if (UWorld* World = GetWorld())
			{
				FVector LightningLocation = FVector::ZeroVector;

				// カメラの周辺にランダムな位置を生成
				if (APlayerController* PC = World->GetFirstPlayerController())
				{
					FVector CameraLocation;
					FRotator CameraRotation;
					PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

					float RandomDistance = FMath::RandRange(500.0f, 5000.0f);
					float RandomAngle = FMath::RandRange(0.0f, 360.0f);

					LightningLocation = CameraLocation + FVector(
						FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
						FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
						FMath::RandRange(500.0f, 2000.0f)
					);
				}

				OnLightningStrike.Broadcast(LightningLocation);
			}
		}
	}
}

void UWeatherManager::InitializeDefaultWeatherConfigs()
{
	// 晴れ
	FWeatherStateConfig ClearConfig;
	ClearConfig.WeatherState = EWeatherState::Clear;
	ClearConfig.Parameters.VisibilityMultiplier = 1.0f;
	ClearConfig.Parameters.MovementSpeedMultiplier = 1.0f;
	ClearConfig.Parameters.WindStrength = 0.1f;
	ClearConfig.Parameters.PrecipitationIntensity = 0.0f;
	ClearConfig.Parameters.LightningFrequency = 0.0f;
	ClearConfig.Parameters.FogDensity = 0.0f;
	ClearConfig.MinDuration = 300.0f;
	ClearConfig.MaxDuration = 600.0f;
	WeatherConfigs.Add(EWeatherState::Clear, ClearConfig);

	// 曇り
	FWeatherStateConfig CloudyConfig;
	CloudyConfig.WeatherState = EWeatherState::Cloudy;
	CloudyConfig.Parameters.VisibilityMultiplier = 0.9f;
	CloudyConfig.Parameters.MovementSpeedMultiplier = 1.0f;
	CloudyConfig.Parameters.WindStrength = 0.2f;
	CloudyConfig.Parameters.PrecipitationIntensity = 0.0f;
	CloudyConfig.Parameters.LightningFrequency = 0.0f;
	CloudyConfig.Parameters.FogDensity = 0.1f;
	CloudyConfig.MinDuration = 200.0f;
	CloudyConfig.MaxDuration = 400.0f;
	WeatherConfigs.Add(EWeatherState::Cloudy, CloudyConfig);

	// 雨
	FWeatherStateConfig RainyConfig;
	RainyConfig.WeatherState = EWeatherState::Rainy;
	RainyConfig.Parameters.VisibilityMultiplier = 0.7f;
	RainyConfig.Parameters.MovementSpeedMultiplier = 0.9f;
	RainyConfig.Parameters.WindStrength = 0.4f;
	RainyConfig.Parameters.PrecipitationIntensity = 0.6f;
	RainyConfig.Parameters.LightningFrequency = 0.0f;
	RainyConfig.Parameters.FogDensity = 0.2f;
	RainyConfig.MinDuration = 150.0f;
	RainyConfig.MaxDuration = 400.0f;
	WeatherConfigs.Add(EWeatherState::Rainy, RainyConfig);

	// 嵐
	FWeatherStateConfig StormConfig;
	StormConfig.WeatherState = EWeatherState::Storm;
	StormConfig.Parameters.VisibilityMultiplier = 0.5f;
	StormConfig.Parameters.MovementSpeedMultiplier = 0.7f;
	StormConfig.Parameters.WindStrength = 0.9f;
	StormConfig.Parameters.PrecipitationIntensity = 1.0f;
	StormConfig.Parameters.LightningFrequency = 0.8f;
	StormConfig.Parameters.FogDensity = 0.3f;
	StormConfig.MinDuration = 100.0f;
	StormConfig.MaxDuration = 300.0f;
	WeatherConfigs.Add(EWeatherState::Storm, StormConfig);

	// 雪
	FWeatherStateConfig SnowyConfig;
	SnowyConfig.WeatherState = EWeatherState::Snowy;
	SnowyConfig.Parameters.VisibilityMultiplier = 0.6f;
	SnowyConfig.Parameters.MovementSpeedMultiplier = 0.8f;
	SnowyConfig.Parameters.WindStrength = 0.3f;
	SnowyConfig.Parameters.PrecipitationIntensity = 0.5f;
	SnowyConfig.Parameters.LightningFrequency = 0.0f;
	SnowyConfig.Parameters.FogDensity = 0.4f;
	SnowyConfig.MinDuration = 200.0f;
	SnowyConfig.MaxDuration = 500.0f;
	WeatherConfigs.Add(EWeatherState::Snowy, SnowyConfig);

	// 霧
	FWeatherStateConfig FoggyConfig;
	FoggyConfig.WeatherState = EWeatherState::Foggy;
	FoggyConfig.Parameters.VisibilityMultiplier = 0.4f;
	FoggyConfig.Parameters.MovementSpeedMultiplier = 0.85f;
	FoggyConfig.Parameters.WindStrength = 0.1f;
	FoggyConfig.Parameters.PrecipitationIntensity = 0.0f;
	FoggyConfig.Parameters.LightningFrequency = 0.0f;
	FoggyConfig.Parameters.FogDensity = 0.9f;
	FoggyConfig.MinDuration = 150.0f;
	FoggyConfig.MaxDuration = 350.0f;
	WeatherConfigs.Add(EWeatherState::Foggy, FoggyConfig);
}

FWeatherParameters UWeatherManager::GetWeatherParametersForState(EWeatherState State) const
{
	if (WeatherConfigs.Contains(State))
	{
		return WeatherConfigs[State].Parameters;
	}
	return FWeatherParameters();
}

EWeatherState UWeatherManager::ChooseNextWeatherState() const
{
	// 現在の天候に基づいて次の天候を選択
	// 遷移の重み付けマップ
	TMap<EWeatherState, float> TransitionWeights;

	switch (CurrentWeatherState)
	{
	case EWeatherState::Clear:
		TransitionWeights.Add(EWeatherState::Clear, 0.4f);
		TransitionWeights.Add(EWeatherState::Cloudy, 0.5f);
		TransitionWeights.Add(EWeatherState::Foggy, 0.1f);
		break;

	case EWeatherState::Cloudy:
		TransitionWeights.Add(EWeatherState::Clear, 0.3f);
		TransitionWeights.Add(EWeatherState::Cloudy, 0.2f);
		TransitionWeights.Add(EWeatherState::Rainy, 0.4f);
		TransitionWeights.Add(EWeatherState::Foggy, 0.1f);
		break;

	case EWeatherState::Rainy:
		TransitionWeights.Add(EWeatherState::Cloudy, 0.4f);
		TransitionWeights.Add(EWeatherState::Rainy, 0.3f);
		TransitionWeights.Add(EWeatherState::Storm, 0.2f);
		TransitionWeights.Add(EWeatherState::Clear, 0.1f);
		break;

	case EWeatherState::Storm:
		TransitionWeights.Add(EWeatherState::Rainy, 0.6f);
		TransitionWeights.Add(EWeatherState::Storm, 0.2f);
		TransitionWeights.Add(EWeatherState::Cloudy, 0.2f);
		break;

	case EWeatherState::Snowy:
		TransitionWeights.Add(EWeatherState::Snowy, 0.4f);
		TransitionWeights.Add(EWeatherState::Cloudy, 0.4f);
		TransitionWeights.Add(EWeatherState::Clear, 0.2f);
		break;

	case EWeatherState::Foggy:
		TransitionWeights.Add(EWeatherState::Foggy, 0.3f);
		TransitionWeights.Add(EWeatherState::Cloudy, 0.4f);
		TransitionWeights.Add(EWeatherState::Clear, 0.3f);
		break;

	default:
		return EWeatherState::Clear;
	}

	// 重み付けに基づいてランダムに選択
	float TotalWeight = 0.0f;
	for (const auto& Pair : TransitionWeights)
	{
		TotalWeight += Pair.Value;
	}

	float RandomValue = FMath::FRand() * TotalWeight;
	float CurrentWeight = 0.0f;

	for (const auto& Pair : TransitionWeights)
	{
		CurrentWeight += Pair.Value;
		if (RandomValue <= CurrentWeight)
		{
			return Pair.Key;
		}
	}

	return EWeatherState::Clear;
}
