# Wave Spawn System - サンプル

## 基本的なウェーブ設定の例

### シンプルな3ウェーブのゲーム

```cpp
// ウェーブ1: 5体の基本敵
Wave 0:
  Enemies[0]:
    Enemy Class: BP_BasicEnemy
    Count: 5
    Spawn Interval: 2.0
    Health Multiplier: 1.0
    Damage Multiplier: 1.0
  Pre Wave Delay: 3.0
  Time Limit: 0 (無制限)
  Require All Enemies Defeated: true

// ウェーブ2: 3体の中級敵 + 2体の基本敵
Wave 1:
  Enemies[0]:
    Enemy Class: BP_IntermediateEnemy
    Count: 3
    Spawn Interval: 3.0
    Health Multiplier: 1.2
    Damage Multiplier: 1.1
  Enemies[1]:
    Enemy Class: BP_BasicEnemy
    Count: 2
    Spawn Interval: 2.0
    Health Multiplier: 1.0
    Damage Multiplier: 1.0
  Pre Wave Delay: 5.0
  Time Limit: 0
  Require All Enemies Defeated: true

// ウェーブ3: 1体のボス敵
Wave 2:
  Enemies[0]:
    Enemy Class: BP_BossEnemy
    Count: 1
    Spawn Interval: 0.0
    Health Multiplier: 2.0
    Damage Multiplier: 1.5
  Pre Wave Delay: 10.0
  Time Limit: 0
  Require All Enemies Defeated: true
```

### 難易度スケーリング設定の例

```cpp
Difficulty Scaling:
  Health Scale Per Wave: 0.15        // 各ウェーブで15%体力増加
  Damage Scale Per Wave: 0.10        // 各ウェーブで10%ダメージ増加
  Count Scale Per Wave: 0.05         // 各ウェーブで5%敵数増加
  Spawn Interval Reduction Per Wave: 0.1  // 各ウェーブで0.1秒短縮
  Max Health Multiplier: 3.0         // 最大300%まで
  Max Damage Multiplier: 2.5         // 最大250%まで
```

この設定の場合：
- ウェーブ1: 体力1.15倍、ダメージ1.10倍
- ウェーブ2: 体力1.30倍、ダメージ1.20倍
- ウェーブ3: 体力1.45倍、ダメージ1.30倍
- ウェーブ10: 体力2.50倍、ダメージ2.00倍
- ウェーブ20: 体力3.00倍（上限）、ダメージ2.50倍（上限）

## Blueprintでの実装例

### ウェーブ開始時のUI表示

```
Event: On Wave Started (AWaveSpawnManager)
├─ Print String: "Wave {WaveNumber} Started!"
├─ Create Widget (WBP_WaveNotification)
├─ Set Wave Number (WaveNumber)
└─ Add to Viewport
```

### ウェーブ完了時の報酬付与

```
Event: On Wave Completed (AWaveSpawnManager)
├─ Branch (Success)
│  ├─ True:
│  │  ├─ Print String: "Wave {WaveNumber} Completed!"
│  │  ├─ Add Score (1000 * WaveNumber)
│  │  └─ Play Sound (SoundWaveComplete)
│  └─ False:
│       ├─ Print String: "Wave {WaveNumber} Failed!"
│       └─ Restart Level
```

### 敵撃破時のカウント表示

```
Event: On Enemy Defeated (AWaveSpawnManager)
├─ Format Text: "Enemies Remaining: {RemainingEnemies}"
├─ Update HUD (Text)
└─ Branch (RemainingEnemies == 0)
    └─ True: Play Sound (SoundWaveComplete)
```

## C++での実装例

### カスタムゲームモードでの使用

```cpp
// YourGameMode.h
UCLASS()
class AYourGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    AWaveSpawnManager* WaveManager;

    virtual void BeginPlay() override;

    UFUNCTION()
    void OnWaveStarted(int32 WaveNumber);

    UFUNCTION()
    void OnWaveCompleted(int32 WaveNumber, bool bSuccess);

    UFUNCTION()
    void OnAllWavesCompleted();
};

// YourGameMode.cpp
void AYourGameMode::BeginPlay()
{
    Super::BeginPlay();

    // ウェーブマネージャーを検索
    TActorIterator<AWaveSpawnManager> It(GetWorld());
    if (It)
    {
        WaveManager = *It;

        // イベントにバインド
        WaveManager->OnWaveStarted.AddDynamic(this, &AYourGameMode::OnWaveStarted);
        WaveManager->OnWaveCompleted.AddDynamic(this, &AYourGameMode::OnWaveCompleted);
        WaveManager->OnAllWavesCompleted.AddDynamic(this, &AYourGameMode::OnAllWavesCompleted);

        // 5秒後にウェーブを開始
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [this]() { WaveManager->StartWaves(); },
            5.0f,
            false
        );
    }
}

void AYourGameMode::OnWaveStarted(int32 WaveNumber)
{
    UE_LOG(LogTemp, Log, TEXT("Wave %d started!"), WaveNumber);
    // UIを更新、音を再生など
}

void AYourGameMode::OnWaveCompleted(int32 WaveNumber, bool bSuccess)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Wave %d completed!"), WaveNumber);
        // スコアを加算、報酬を付与など
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave %d failed!"), WaveNumber);
        // リトライ処理など
    }
}

void AYourGameMode::OnAllWavesCompleted()
{
    UE_LOG(LogTemp, Log, TEXT("All waves completed! Victory!"));
    // 勝利画面を表示など
}
```

### 難易度スケーリングを反映する敵の実装

```cpp
// Enemy.h
UCLASS()
class AEnemy : public ACharacter
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float BaseHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float BaseDamage = 10.0f;

    UPROPERTY()
    float HealthMultiplier = 1.0f;

    UPROPERTY()
    float DamageMultiplier = 1.0f;

    UPROPERTY()
    float CurrentHealth;

public:
    virtual void BeginPlay() override;

    void SetDifficultyMultipliers(float Health, float Damage);

    float GetAttackDamage() const;
};

// Enemy.cpp
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    // 難易度倍率を適用した体力を設定
    CurrentHealth = BaseHealth * HealthMultiplier;
}

void AEnemy::SetDifficultyMultipliers(float Health, float Damage)
{
    HealthMultiplier = Health;
    DamageMultiplier = Damage;
}

float AEnemy::GetAttackDamage() const
{
    return BaseDamage * DamageMultiplier;
}
```

### カスタムスポーンロジックの実装

```cpp
// ウェーブマネージャーを継承してカスタマイズ
UCLASS()
class ACustomWaveManager : public AWaveSpawnManager
{
    GENERATED_BODY()

protected:
    virtual AWaveSpawnPoint* SelectSpawnPoint() override
    {
        // カスタムロジック: プレイヤーの背後を優先
        APawn* PlayerPawn = GetPlayerPawn();
        if (!PlayerPawn || SpawnPoints.Num() == 0)
        {
            return Super::SelectSpawnPoint();
        }

        FVector PlayerForward = PlayerPawn->GetActorForwardVector();
        FVector PlayerLocation = PlayerPawn->GetActorLocation();

        AWaveSpawnPoint* BestPoint = nullptr;
        float BestScore = -1.0f;

        for (AWaveSpawnPoint* Point : SpawnPoints)
        {
            if (!Point || !Point->CanSpawn()) continue;

            FVector ToPoint = (Point->GetActorLocation() - PlayerLocation).GetSafeNormal();
            float DotProduct = FVector::DotProduct(PlayerForward, ToPoint);

            // 背後ほど高スコア（-1に近いほど良い）
            float Score = -DotProduct;

            if (Score > BestScore)
            {
                BestScore = Score;
                BestPoint = Point;
            }
        }

        return BestPoint ? BestPoint : Super::SelectSpawnPoint();
    }
};
```

## アリーナ型ゲームの完全な例

### 設定
- 無限ループウェーブ
- 徐々に難易度が上昇
- プレイヤーから遠い場所にスポーン

```cpp
Wave Spawn Manager:
  Waves:
    Wave 0:
      Enemies[0]:
        Enemy Class: BP_BasicEnemy
        Count: 3
        Spawn Interval: 2.0
    Wave 1:
      Enemies[0]:
        Enemy Class: BP_BasicEnemy
        Count: 5
        Spawn Interval: 1.5
    Wave 2:
      Enemies[0]:
        Enemy Class: BP_IntermediateEnemy
        Count: 2
        Spawn Interval: 3.0
      Enemies[1]:
        Enemy Class: BP_BasicEnemy
        Count: 3
        Spawn Interval: 2.0

  Difficulty Scaling:
    Health Scale Per Wave: 0.2
    Damage Scale Per Wave: 0.1
    Count Scale Per Wave: 0.1
    Max Health Multiplier: 5.0
    Max Damage Multiplier: 3.0

  Spawn Point Selection Mode: Farthest From Player
  Auto Start First Wave: true
  Auto Start Delay: 5.0
  Loop Waves: true
```

この設定により、プレイヤーは徐々に難しくなる無限のウェーブに挑戦できます。
