# Wave Spawn System - APIリファレンス

## クラス

### AWaveSpawnManager

ウェーブシステムのメインマネージャークラス。

#### プロパティ

##### イベント

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `OnWaveStarted` | `FOnWaveStarted` | ウェーブが開始された時に発火 |
| `OnWaveCompleted` | `FOnWaveCompleted` | ウェーブが完了した時に発火 |
| `OnAllWavesCompleted` | `FOnAllWavesCompleted` | 全てのウェーブが完了した時に発火 |
| `OnEnemySpawned` | `FOnEnemySpawned` | 敵がスポーンされた時に発火 |
| `OnEnemyDefeated` | `FOnEnemyDefeated` | 敵が倒された時に発火 |

##### 設定

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `Waves` | `TArray<FWaveConfiguration>` | ウェーブの設定配列 |
| `DifficultyScaling` | `FDifficultyScaling` | 難易度スケーリング設定 |
| `SpawnPointSelectionMode` | `ESpawnPointSelectionMode` | スポーンポイント選択モード |
| `bAutoStartFirstWave` | `bool` | 自動開始するかどうか |
| `AutoStartDelay` | `float` | 自動開始の遅延時間 |
| `bLoopWaves` | `bool` | ウェーブをループするかどうか |
| `SpawnPoints` | `TArray<AWaveSpawnPoint*>` | 使用するスポーンポイント配列 |
| `SpawnPointFilterTag` | `FName` | スポーンポイントフィルタリングタグ |

##### 状態

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `CurrentWaveIndex` | `int32` | 現在のウェーブインデックス（0から始まる） |
| `ActiveEnemyCount` | `int32` | 現在アクティブな敵の数 |
| `TotalEnemiesSpawnedThisWave` | `int32` | このウェーブでスポーンした敵の総数 |
| `bWaveActive` | `bool` | ウェーブがアクティブかどうか |

#### メソッド

##### Public

| メソッド | 戻り値 | 説明 |
|---------|--------|------|
| `StartWaves()` | `void` | ウェーブシステムを開始 |
| `StartWave(int32 WaveIndex)` | `void` | 特定のウェーブを開始（1から始まる） |
| `StopCurrentWave()` | `void` | 現在のウェーブを停止 |
| `GetCurrentWaveNumber()` | `int32` | 現在のウェーブ番号を取得（1から始まる） |
| `GetTotalWaveCount()` | `int32` | 設定されたウェーブの総数を取得 |
| `IsWaveActive()` | `bool` | ウェーブがアクティブかチェック |
| `GetActiveEnemyCount()` | `int32` | アクティブな敵の数を取得 |

##### Protected（継承時にオーバーライド可能）

| メソッド | 戻り値 | 説明 |
|---------|--------|------|
| `InitializeSpawnPoints()` | `void` | スポーンポイントを初期化 |
| `SelectSpawnPoint()` | `AWaveSpawnPoint*` | スポーンポイントを選択 |
| `ApplyDifficultyScaling()` | `void` | 難易度スケーリングを適用 |
| `BeginWave(int32 WaveIndex)` | `void` | ウェーブを開始する内部処理 |
| `SpawnWaveEnemies()` | `void` | ウェーブの敵をスポーン |
| `SpawnEnemy()` | `void` | 1体の敵をスポーン |
| `CheckWaveCompletion()` | `void` | ウェーブ完了をチェック |
| `CompleteWave(bool bSuccess)` | `void` | ウェーブを完了 |

---

### AWaveSpawnPoint

敵のスポーン地点を表すアクター。

#### プロパティ

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `bIsEnabled` | `bool` | このスポーンポイントが有効かどうか |
| `Priority` | `int32` | 優先度（高いほど選ばれやすい） |
| `SpawnTag` | `FName` | フィルタリング用のタグ |
| `ExclusionRadius` | `float` | 他のスポーンを避ける半径 |
| `CurrentOccupants` | `int32` | 現在このポイントにいる敵の数 |

#### メソッド

| メソッド | 戻り値 | 説明 |
|---------|--------|------|
| `GetSpawnTransform()` | `FTransform` | スポーン用のトランスフォームを取得 |
| `CanSpawn()` | `bool` | スポーン可能かチェック |
| `OnEnemySpawned()` | `void` | 敵がスポーンされた時に呼ばれる |
| `OnEnemyDestroyed()` | `void` | 敵が破壊された時に呼ばれる |

---

## 構造体

### FWaveConfiguration

ウェーブの設定を定義する構造体。

#### プロパティ

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `Enemies` | `TArray<FEnemySpawnInfo>` | このウェーブでスポーンする敵の配列 |
| `PreWaveDelay` | `float` | ウェーブ開始前の遅延（秒） |
| `TimeLimit` | `float` | 制限時間（0で無制限） |
| `bRequireAllEnemiesDefeated` | `bool` | 全ての敵を倒す必要があるか |
| `RewardIdentifier` | `FString` | 報酬の識別子 |

---

### FEnemySpawnInfo

個々の敵タイプの情報を定義する構造体。

#### プロパティ

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `EnemyClass` | `TSubclassOf<AActor>` | スポーンする敵のクラス |
| `Count` | `int32` | スポーンする数 |
| `SpawnInterval` | `float` | スポーン間隔（秒） |
| `HealthMultiplier` | `float` | 体力倍率 |
| `DamageMultiplier` | `float` | ダメージ倍率 |

---

### FDifficultyScaling

難易度スケーリング設定を定義する構造体。

#### プロパティ

| プロパティ | 型 | 説明 |
|---------|-----|------|
| `HealthScalePerWave` | `float` | ウェーブごとの体力倍率増加 |
| `DamageScalePerWave` | `float` | ウェーブごとのダメージ倍率増加 |
| `CountScalePerWave` | `float` | ウェーブごとの敵数倍率増加 |
| `SpawnIntervalReductionPerWave` | `float` | ウェーブごとのスポーン間隔短縮（秒） |
| `MaxHealthMultiplier` | `float` | 体力倍率の上限 |
| `MaxDamageMultiplier` | `float` | ダメージ倍率の上限 |

---

## 列挙型

### ESpawnPointSelectionMode

スポーンポイントの選択方法を定義する列挙型。

| 値 | 説明 |
|----|------|
| `Sequential` | 順番に選択 |
| `Random` | ランダムに選択 |
| `FarthestFromPlayer` | プレイヤーから最も遠い地点を選択 |
| `OutOfSight` | プレイヤーの視界外を選択 |

---

## デリゲート

### FOnWaveStarted

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
```

ウェーブが開始された時に呼ばれるデリゲート。

**パラメータ:**
- `WaveNumber` (int32): 開始されたウェーブ番号（1から始まる）

---

### FOnWaveCompleted

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCompleted, int32, WaveNumber, bool, bSuccess);
```

ウェーブが完了した時に呼ばれるデリゲート。

**パラメータ:**
- `WaveNumber` (int32): 完了したウェーブ番号（1から始まる）
- `bSuccess` (bool): 成功したかどうか

---

### FOnAllWavesCompleted

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCompleted);
```

全てのウェーブが完了した時に呼ばれるデリゲート。

---

### FOnEnemySpawned

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemySpawned, AActor*, Enemy, int32, WaveNumber, int32, EnemyIndex);
```

敵がスポーンされた時に呼ばれるデリゲート。

**パラメータ:**
- `Enemy` (AActor*): スポーンされた敵のアクター
- `WaveNumber` (int32): 現在のウェーブ番号（1から始まる）
- `EnemyIndex` (int32): この敵のインデックス

---

### FOnEnemyDefeated

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyDefeated, AActor*, Enemy, int32, WaveNumber, int32, RemainingEnemies);
```

敵が倒された時に呼ばれるデリゲート。

**パラメータ:**
- `Enemy` (AActor*): 倒された敵のアクター
- `WaveNumber` (int32): 現在のウェーブ番号（1から始まる）
- `RemainingEnemies` (int32): 残りの敵の数

---

## 使用例

### C++での基本的な使用

```cpp
#include "WaveSpawnManager.h"
#include "WaveSpawnPoint.h"

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // ウェーブマネージャーを検索
    AWaveSpawnManager* Manager = // ... 取得

    // イベントにバインド
    Manager->OnWaveStarted.AddDynamic(this, &AMyGameMode::HandleWaveStarted);
    Manager->OnEnemyDefeated.AddDynamic(this, &AMyGameMode::HandleEnemyDefeated);

    // ウェーブを開始
    Manager->StartWaves();
}

void AMyGameMode::HandleWaveStarted(int32 WaveNumber)
{
    UE_LOG(LogTemp, Log, TEXT("Wave %d started"), WaveNumber);
}

void AMyGameMode::HandleEnemyDefeated(AActor* Enemy, int32 WaveNumber, int32 RemainingEnemies)
{
    UE_LOG(LogTemp, Log, TEXT("Enemy defeated! %d remaining"), RemainingEnemies);
}
```

### Blueprintでの基本的な使用

1. ウェーブマネージャーへの参照を取得
2. `Bind Event to On Wave Started`などのノードでイベントをバインド
3. `Start Waves`ノードを呼び出してウェーブを開始
