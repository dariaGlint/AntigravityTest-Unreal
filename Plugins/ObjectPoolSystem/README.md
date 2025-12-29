# Object Pool System

Unreal Engine 5用のオブジェクトプールシステムプラグインです。頻繁に生成・破棄されるActorを再利用することで、パフォーマンスを大幅に向上させます。

## 概要

オブジェクトプールは、頻繁に生成・破棄されるオブジェクト（弾丸、敵キャラクター、エフェクトなど）を事前に作成し、再利用するデザインパターンです。これにより以下のメリットがあります：

- **パフォーマンス向上**: メモリアロケーション/デアロケーションのオーバーヘッドを削減
- **メモリフラグメンテーション軽減**: オブジェクトの生成・破棄を最小限に抑制
- **予測可能な動作**: オブジェクト数を事前に管理できる
- **デバッグの容易さ**: 使用状況の統計情報を簡単に取得可能

## 機能

- ✅ 頻繁に生成/破棄されるActorの再利用
- ✅ 自動プール管理（Actorクラスごとに自動作成）
- ✅ 自動拡張（プール空時に新しいオブジェクトを自動生成）
- ✅ 最大サイズ制限（メモリ使用量のコントロール）
- ✅ 使用状況モニタリング（リアルタイム統計情報）
- ✅ Blueprint完全対応
- ✅ C++とBlueprintの両方から使用可能
- ✅ ワールドサブシステムによる自動ライフサイクル管理

## インストール

1. このプラグインフォルダを `YourProject/Plugins/` ディレクトリにコピー
2. プロジェクトを再生成（右クリック > Generate Visual Studio project files）
3. Unreal Editorを起動
4. Edit > Plugins からプラグインを検索して有効化
5. エディタを再起動

## 使い方

### Blueprint での使用

#### 1. Poolable インターフェースの実装

プールしたいActorのBlueprintで、IPoolableインターフェースを実装します：

1. Actor Blueprintを開く
2. Class Settings > Interfaces から `Poolable` インターフェースを追加
3. `OnPoolActivate` イベントを実装（プールから取り出された時の処理）
4. `OnPoolDeactivate` イベントを実装（プールに戻される時の処理）
5. （オプション）`CanBePooled` イベントを実装（プールに戻せるかのチェック）

**例：敵キャラクターの実装**
```
Event OnPoolActivate:
  - Set Actor Hidden in Game: false
  - Set Health: 100
  - Start AI Behavior

Event OnPoolDeactivate:
  - Set Actor Hidden in Game: true
  - Stop AI Behavior
  - Reset Position
```

#### 2. プールからのスポーン

`SpawnPooledActor` ノードを使用してActorをスポーンします：

```
SpawnPooledActor
  - Actor Class: BP_Enemy
  - Spawn Transform: (Location, Rotation, Scale)
  - Initial Pool Size: 10
  - Max Pool Size: 50
  → Return Value: スポーンされた敵Actor
```

#### 3. プールへの返却

使用が終わったら `DespawnPooledActor` ノードでプールに戻します：

```
DespawnPooledActor
  - Actor: 返却するActor
  → Return Value: 成功/失敗
```

#### 4. プールの事前ウォーミング

ゲーム開始時にプールを事前に準備しておく場合：

```
Event BeginPlay:
  PrewarmPool
    - Actor Class: BP_Bullet
    - Count: 50
    - Initial Pool Size: 50
    - Max Pool Size: 100
```

#### 5. 統計情報の取得

プールの使用状況を確認する場合：

```
GetPoolStats
  - Actor Class: BP_Enemy
  → Return Value: Pool Stats
    - Total Objects
    - Active Objects
    - Inactive Objects
    - Peak Active Objects
    - Total Spawn Count
    - Pool Expansion Count
```

### C++ での使用

#### 1. IPoolable インターフェースの実装

```cpp
// EnemyActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPoolable.h"
#include "EnemyActor.generated.h"

UCLASS()
class YOURGAME_API AEnemyActor : public AActor, public IPoolable
{
    GENERATED_BODY()

public:
    // IPoolable インターフェースの実装
    virtual void OnPoolActivate_Implementation() override;
    virtual void OnPoolDeactivate_Implementation() override;
    virtual bool CanBePooled_Implementation() const override;

private:
    UPROPERTY(EditAnywhere)
    int32 Health;
};
```

```cpp
// EnemyActor.cpp
#include "EnemyActor.h"

void AEnemyActor::OnPoolActivate_Implementation()
{
    // プールから取り出された時の処理
    SetActorHiddenInGame(false);
    Health = 100;
    // AI の開始など
}

void AEnemyActor::OnPoolDeactivate_Implementation()
{
    // プールに戻される時の処理
    SetActorHiddenInGame(true);
    // AI の停止など
}

bool AEnemyActor::CanBePooled_Implementation() const
{
    // プールに戻せる状態かチェック
    return true;
}
```

#### 2. プールからのスポーンと返却

```cpp
#include "ObjectPoolManagerSubsystem.h"

void AMyGameMode::SpawnEnemies()
{
    UWorld* World = GetWorld();
    UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();

    // プールからスポーン
    FTransform SpawnTransform(FVector(0, 0, 100));
    AActor* Enemy = PoolManager->SpawnFromPool(
        AEnemyActor::StaticClass(),
        SpawnTransform,
        10,  // Initial Pool Size
        50   // Max Pool Size
    );

    // 使用後、プールに返却
    PoolManager->ReturnToPool(Enemy);
}

void AMyGameMode::PrewarmPools()
{
    UWorld* World = GetWorld();
    UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();

    // プールを事前にウォーミング
    PoolManager->PrewarmPool(AEnemyActor::StaticClass(), 20);
    PoolManager->PrewarmPool(ABulletActor::StaticClass(), 100);
}

void AMyGameMode::LogPoolStatistics()
{
    UWorld* World = GetWorld();
    UObjectPoolManagerSubsystem* PoolManager = World->GetSubsystem<UObjectPoolManagerSubsystem>();

    // 統計情報をログ出力
    PoolManager->LogAllPoolStats();

    // 特定のプールの統計情報を取得
    FPoolStats Stats = PoolManager->GetPoolStats(AEnemyActor::StaticClass());
    UE_LOG(LogTemp, Log, TEXT("Enemy Pool - Active: %d, Inactive: %d"),
        Stats.ActiveObjects, Stats.InactiveObjects);
}
```

## API リファレンス

### IPoolable インターフェース

Actorがプールシステムで管理されるために実装するインターフェース。

#### メソッド

- **OnPoolActivate()**: プールから取り出され、アクティブ化される時に呼ばれる
- **OnPoolDeactivate()**: プールに戻される時に呼ばれる（状態リセット用）
- **CanBePooled()**: プールに戻すことができるかチェック（デフォルト: true）

### UObjectPool クラス

個別のActorクラス用プール管理クラス。通常は直接使用せず、UObjectPoolManagerSubsystem経由で使用します。

#### メソッド

- **Initialize()**: プールを初期化
- **Spawn()**: プールからActorを取得（なければ新規作成）
- **Despawn()**: Actorをプールに戻す
- **Prewarm()**: 指定数のオブジェクトを事前生成
- **Clear()**: プールをクリア（全オブジェクトを破棄）
- **GetStats()**: プールの統計情報を取得
- **LogStats()**: 統計情報をログ出力

### UObjectPoolManagerSubsystem クラス

ワールド内のすべてのプールを統合管理するサブシステム。

#### メソッド

- **SpawnFromPool(ActorClass, SpawnTransform, InitialPoolSize, MaxPoolSize)**: プールからスポーン
- **ReturnToPool(Actor)**: プールに返却
- **PrewarmPool(ActorClass, Count, InitialPoolSize, MaxPoolSize)**: プールを事前ウォーミング
- **ClearPool(ActorClass)**: 特定プールをクリア
- **ClearAllPools()**: すべてのプールをクリア
- **GetPoolStats(ActorClass)**: 特定プールの統計情報を取得
- **GetAllPoolStats()**: すべてのプールの統計情報を取得
- **LogAllPoolStats()**: すべてのプールの統計情報をログ出力

### UObjectPoolBlueprintLibrary クラス

Blueprint用の便利な関数ライブラリ。

#### メソッド

- **SpawnPooledActor()**: プールからActorをスポーン
- **DespawnPooledActor()**: プールにActorを返却
- **PrewarmPool()**: プールを事前ウォーミング
- **ClearPool()**: 特定プールをクリア
- **ClearAllPools()**: すべてのプールをクリア
- **GetPoolStats()**: 特定プールの統計情報を取得
- **LogPoolStats()**: すべてのプールの統計情報をログ出力

### FPoolStats 構造体

プールの統計情報を保持する構造体。

#### プロパティ

- **TotalObjects**: プール内の総オブジェクト数
- **ActiveObjects**: 現在アクティブなオブジェクト数
- **InactiveObjects**: プール内で利用可能なオブジェクト数
- **PeakActiveObjects**: これまでのアクティブオブジェクト数のピーク
- **TotalSpawnCount**: プールからのスポーン総回数
- **PoolExpansionCount**: プールの自動拡張回数

## パフォーマンス最適化のヒント

### 1. 適切な初期プールサイズ

ゲームプレイ中に必要となる平均的なオブジェクト数を初期プールサイズとして設定します。

```cpp
// 悪い例：小さすぎる初期サイズ
PoolManager->SpawnFromPool(ABulletActor::StaticClass(), Transform, 1, 100);

// 良い例：予想される使用量に基づいた初期サイズ
PoolManager->SpawnFromPool(ABulletActor::StaticClass(), Transform, 50, 100);
```

### 2. 最大サイズの設定

メモリ使用量をコントロールするために、最大サイズを設定します。

```cpp
// 無制限（注意：メモリリークの可能性）
PoolManager->SpawnFromPool(AEnemyActor::StaticClass(), Transform, 10, 0);

// 最大サイズを設定（推奨）
PoolManager->SpawnFromPool(AEnemyActor::StaticClass(), Transform, 10, 50);
```

### 3. プールの事前ウォーミング

ゲーム開始時やレベルロード時にプールを事前に準備します。

```cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    UObjectPoolManagerSubsystem* PoolManager = GetWorld()->GetSubsystem<UObjectPoolManagerSubsystem>();

    // ロード画面中にプールをウォーミング
    PoolManager->PrewarmPool(ABulletActor::StaticClass(), 100);
    PoolManager->PrewarmPool(AEnemyActor::StaticClass(), 20);
    PoolManager->PrewarmPool(AEffectActor::StaticClass(), 50);
}
```

### 4. 統計情報の監視

開発中は統計情報を定期的にチェックして、プールサイズを調整します。

```cpp
// 開発ビルドでのみ統計情報を出力
#if !UE_BUILD_SHIPPING
void AMyGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    static float Timer = 0.0f;
    Timer += DeltaTime;

    // 10秒ごとに統計情報を出力
    if (Timer >= 10.0f)
    {
        UObjectPoolManagerSubsystem* PoolManager = GetWorld()->GetSubsystem<UObjectPoolManagerSubsystem>();
        PoolManager->LogAllPoolStats();
        Timer = 0.0f;
    }
}
#endif
```

### 5. CanBePooled() の適切な実装

特定の条件下でのみプールに戻すようにします。

```cpp
bool AEnemyActor::CanBePooled_Implementation() const
{
    // 破壊アニメーション中はプールに戻さない
    if (bPlayingDeathAnimation)
    {
        return false;
    }

    // 異常な状態の場合はプールに戻さず破棄
    if (!IsValidLowLevel() || IsPendingKill())
    {
        return false;
    }

    return true;
}
```

## ベストプラクティス

### 1. OnPoolDeactivate で完全にリセット

プールに戻す際は、すべての状態を初期化します。

```cpp
void AEnemyActor::OnPoolDeactivate_Implementation()
{
    // ビジュアル
    SetActorHiddenInGame(true);

    // 物理
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    // ゲームロジック
    Health = 0;
    CurrentTarget = nullptr;

    // AI
    if (AIController)
    {
        AIController->StopMovement();
    }

    // タイマーのクリア
    GetWorldTimerManager().ClearAllTimersForObject(this);
}
```

### 2. OnPoolActivate で再初期化

プールから取り出す際は、必要な初期化を行います。

```cpp
void ABulletActor::OnPoolActivate_Implementation()
{
    // ビジュアル
    SetActorHiddenInGame(false);

    // 物理
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);

    // 弾丸の初期化
    Velocity = FVector::ZeroVector;
    bHasHitTarget = false;

    // 自動デスポーンタイマー
    GetWorldTimerManager().SetTimer(
        LifetimeTimerHandle,
        this,
        &ABulletActor::AutoDespawn,
        5.0f,
        false
    );
}
```

### 3. プール対象の選定

すべてのActorをプールする必要はありません。以下のような場合に有効です：

**プールに適している：**
- 頻繁にスポーン/デスポーンされる（弾丸、エフェクト）
- 同時に多数存在する（敵キャラクター、パーティクル）
- 生成コストが高い（複雑なメッシュ、多数のコンポーネント）

**プールに適していない：**
- めったにスポーンされない（ボスキャラクター）
- 一度しかスポーンされない（プレイヤーキャラクター）
- 状態が複雑すぎてリセットが困難

### 4. レベルクリーンアップ

レベル終了時は必要に応じてプールをクリアします。

```cpp
void AMyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UObjectPoolManagerSubsystem* PoolManager = GetWorld()->GetSubsystem<UObjectPoolManagerSubsystem>();
    if (PoolManager)
    {
        PoolManager->ClearAllPools();
    }

    Super::EndPlay(EndPlayReason);
}
```

## トラブルシューティング

### プールからスポーンされたActorが表示されない

- `OnPoolActivate()` で `SetActorHiddenInGame(false)` を呼んでいるか確認
- コリジョンとTickが有効になっているか確認

### メモリ使用量が増え続ける

- 最大プールサイズを設定しているか確認
- `ReturnToPool()` を適切に呼んでいるか確認
- `CanBePooled()` が常に false を返していないか確認

### パフォーマンスが改善されない

- 初期プールサイズが小さすぎないか確認（統計情報の `PoolExpansionCount` をチェック）
- プール対象のActorが頻繁にスポーン/デスポーンされているか確認
- Profiler でボトルネックを特定

## ライセンス

このプラグインはMITライセンスの下で提供されています。

## サポート

問題が発生した場合や機能リクエストがある場合は、プロジェクトのIssueトラッカーにて報告してください。
