# Weather System Plugin

UE5用の包括的な天候システムプラグイン

## 概要

Weather Systemは、Unreal Engine 5向けの柔軟で拡張可能な天候システムです。天候状態の遷移、時間帯の変化、ゲームプレイへの影響、パーティクル/ライティング連携用のイベントシステムを提供します。

## 機能

### 天候状態
- **晴れ (Clear)**: クリアな視界、通常の移動速度
- **曇り (Cloudy)**: わずかに視界が低下
- **雨 (Rainy)**: 視界と移動速度が低下、降水あり
- **嵐 (Storm)**: 大幅な視界低下、移動速度低下、激しい雨と雷
- **雪 (Snowy)**: 視界と移動速度が低下、降雪
- **霧 (Foggy)**: 大幅な視界低下、濃い霧

### 時間帯
- 夜明け (Dawn)
- 朝 (Morning)
- 昼 (Noon)
- 午後 (Afternoon)
- 夕方 (Evening)
- 夜 (Night)

### ゲームプレイへの影響
- **視界倍率**: 天候による視界の変化（0.0 = 完全に見えない、1.0 = クリア）
- **移動速度倍率**: 天候による移動速度の変化（0.0 = 移動不可、1.0 = 通常速度）
- **風の強さ**: パーティクルエフェクトに使用
- **降水量**: 雨や雪の強さ
- **雷の頻度**: 嵐時の雷の発生頻度
- **霧の濃度**: 霧の濃さ

## 使用方法

### セットアップ

1. プラグインを有効化する
2. プロジェクトを再コンパイルする

### Blueprint での使用

#### 天候マネージャーの取得

```cpp
// ワールドサブシステムとして取得
UWeatherManager* WeatherManager = GetWorld()->GetSubsystem<UWeatherManager>();
```

#### 天候の変更

```cpp
// 即座に天候を変更
WeatherManager->SetWeather(EWeatherState::Rainy, true);

// スムーズに遷移
WeatherManager->TransitionToWeather(EWeatherState::Storm, 10.0f);
```

#### 現在の天候情報の取得

```cpp
// 現在の天候状態
EWeatherState CurrentWeather = WeatherManager->GetCurrentWeather();

// 現在の天候パラメータ
FWeatherParameters Params = WeatherManager->GetCurrentWeatherParameters();
float Visibility = Params.VisibilityMultiplier;
float MovementSpeed = Params.MovementSpeedMultiplier;
```

#### 時間帯の制御

```cpp
// 時間の進行を有効化
WeatherManager->SetTimeProgressionEnabled(true);

// 時間の進行速度を設定（2.0 = 2倍速）
WeatherManager->SetTimeProgressionSpeed(2.0f);

// 時間帯を直接設定
WeatherManager->SetTimeOfDay(ETimeOfDay::Night);
```

#### 自動天候遷移

```cpp
// 自動天候遷移を有効化
WeatherManager->SetAutoWeatherTransitionEnabled(true);
```

### イベントの使用

#### 天候変化イベント

```cpp
// WeatherManagerのOnWeatherChangedデリゲートにバインド
WeatherManager->OnWeatherChanged.AddDynamic(this, &AMyActor::OnWeatherChanged);

void AMyActor::OnWeatherChanged(EWeatherState OldWeather, EWeatherState NewWeather)
{
    // 天候が変化した時の処理
    // パーティクルエフェクトの更新など
}
```

#### 天候パラメータ更新イベント

```cpp
WeatherManager->OnWeatherParametersUpdated.AddDynamic(this, &AMyActor::OnParametersUpdated);

void AMyActor::OnParametersUpdated(const FWeatherParameters& NewParameters)
{
    // パラメータが更新された時の処理
    // ポストプロセスエフェクトの更新など
}
```

#### 時間帯変化イベント

```cpp
WeatherManager->OnTimeOfDayChanged.AddDynamic(this, &AMyActor::OnTimeChanged);

void AMyActor::OnTimeChanged(ETimeOfDay OldTime, ETimeOfDay NewTime)
{
    // 時間帯が変化した時の処理
    // ライティングの更新など
}
```

#### 雷イベント

```cpp
WeatherManager->OnLightningStrike.AddDynamic(this, &AMyActor::OnLightning);

void AMyActor::OnLightning(FVector StrikeLocation)
{
    // 雷が発生した時の処理
    // 稲妻のVFX、サウンドエフェクトなど
}
```

### WeatherAffectedComponent の使用

アクターに `UWeatherAffectedComponent` を追加することで、そのアクターが自動的に天候の影響を受けるようになります。

```cpp
// C++での追加例
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
UWeatherAffectedComponent* WeatherComponent;

// コンストラクタで
WeatherComponent = CreateDefaultSubobject<UWeatherAffectedComponent>(TEXT("WeatherComponent"));

// 現在の倍率を取得
float VisibilityMultiplier = WeatherComponent->GetCurrentVisibilityMultiplier();
float MovementMultiplier = WeatherComponent->GetCurrentMovementSpeedMultiplier();
```

## カスタマイズ

### 天候状態の設定をカスタマイズ

```cpp
FWeatherStateConfig CustomRainyConfig;
CustomRainyConfig.WeatherState = EWeatherState::Rainy;
CustomRainyConfig.Parameters.VisibilityMultiplier = 0.5f;
CustomRainyConfig.Parameters.MovementSpeedMultiplier = 0.8f;
CustomRainyConfig.Parameters.PrecipitationIntensity = 0.9f;
CustomRainyConfig.MinDuration = 100.0f;
CustomRainyConfig.MaxDuration = 300.0f;

WeatherManager->AddWeatherStateConfig(CustomRainyConfig);
```

## 統合例

### ポストプロセスエフェクトとの連携

```cpp
void AMyGameMode::OnWeatherParametersUpdated(const FWeatherParameters& Params)
{
    if (PostProcessVolume)
    {
        // 霧の濃度を調整
        PostProcessVolume->Settings.FogDensity = Params.FogDensity;

        // 露出を調整
        PostProcessVolume->Settings.AutoExposureBias = -Params.FogDensity * 2.0f;
    }
}
```

### パーティクルシステムとの連携

```cpp
void AWeatherController::OnWeatherChanged(EWeatherState OldWeather, EWeatherState NewWeather)
{
    // 既存のパーティクルを停止
    StopAllWeatherParticles();

    // 新しい天候のパーティクルを開始
    switch (NewWeather)
    {
    case EWeatherState::Rainy:
        SpawnRainParticles();
        break;
    case EWeatherState::Storm:
        SpawnStormParticles();
        break;
    case EWeatherState::Snowy:
        SpawnSnowParticles();
        break;
    }
}
```

## 技術詳細

- **システムタイプ**: World Subsystem
- **対応Unreal Engineバージョン**: 5.0+
- **モジュールタイプ**: Runtime
- **依存関係**: Core, CoreUObject, Engine

## ライセンス

このプラグインはプロジェクトの一部として提供されています。

## サポート

問題や質問がある場合は、プロジェクトのIssueトラッカーをご利用ください。
