# Object Pool System

頻繁に生成/破棄されるActorの再利用システムを提供するUnreal Engineプラグインです。

## 概要

このプラグインは、ゲーム内で頻繁に生成・破棄されるActor（敵キャラクター、弾丸、エフェクトなど）を効率的に管理するためのオブジェクトプールシステムを提供します。メモリアロケーションのオーバーヘッドを削減し、パフォーマンスを向上させることができます。

## 主な機能

- **頻繁に生成/破棄されるActorの再利用** - メモリアロケーションのオーバーヘッド削減
- **自動プール管理** - Actorクラスごとに自動でプール作成
- **自動拡張** - プール空時に自動的に新しいオブジェクトを生成
- **最大サイズ制限** - メモリ使用量のコントロール可能
- **使用状況モニタリング** - リアルタイムで統計情報を取得
- **Blueprint完全対応** - C++とBlueprint両方から使用可能
- **簡単な統合** - IPoolableインターフェースを実装するだけ

## インストール

1. このプラグインを `Plugins/ObjectPoolSystem/` ディレクトリにコピー
2. Unreal Editorでプロジェクトを開く
3. Edit > Plugins でプラグインを有効化
4. エディタを再起動

## 使用方法

### Blueprint での使用

#### 1. Actor Blueprintの設定

1. プールしたいActor Blueprintを開く
2. Class Settings > Interfaces から `Poolable` インターフェースを追加
3. Event Graph で以下のイベントを実装:
   - `OnPoolActivate`: プールから取り出された時の処理
   - `OnPoolDeactivate`: プールに返却される時の処理

#### 2. Actorのスポーンと返却

```
// プールからスポーン
Spawn Pooled Actor
  - Actor Class: MyEnemyActor
  - Location: (0, 0, 100)
  - Rotation: (0, 0, 0)
  - Initial Pool Size: 10
  - Max Pool Size: 50

// プールに返却
Despawn Pooled Actor
  - Actor: (スポーンしたActor)
```

#### 3. プールの事前生成（オプション）

ゲーム開始時にプールを事前に生成しておくことで、最初のスポーン時の遅延を防ぎます:

```
Event BeginPlay
  -> Prewarm Pool
       - Actor Class: MyEnemyActor
       - Count: 20
       - Max Pool Size: 50
```

### C++ での使用

#### 1. IPoolable インターフェースの実装

```cpp
// MyEnemy.h
#include "IPoolable.h"

UCLASS()
class AMyEnemy : public AActor, public IPoolable
{
    GENERATED_BODY()

public:
    // IPoolable interface
    virtual void OnPoolActivate_Implementation() override;
    virtual void OnPoolDeactivate_Implementation() override;
    virtual bool CanBePooled_Implementation() const override;
};

// MyEnemy.cpp
void AMyEnemy::OnPoolActivate_Implementation()
{
    // プールから取り出された時の初期化処理
    Health = MaxHealth;
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}

void AMyEnemy::OnPoolDeactivate_Implementation()
{
    // プールに返却される時のクリーンアップ処理
    // タイマーのクリアなど
    GetWorldTimerManager().ClearAllTimersForObject(this);
}

bool AMyEnemy::CanBePooled_Implementation() const
{
    // プールに返却可能かチェック
    return true;
}
```

#### 2. プールの使用

```cpp
// プールからスポーン
UObjectPoolManagerSubsystem* PoolManager = GetWorld()->GetSubsystem<UObjectPoolManagerSubsystem>();
if (PoolManager)
{
    FTransform SpawnTransform(FRotator::ZeroRotator, FVector(0, 0, 100));
    AActor* Enemy = PoolManager->SpawnFromPool(AMyEnemy::StaticClass(), SpawnTransform);
}

// プールに返却
if (PoolManager)
{
    PoolManager->ReturnToPool(Enemy);
}

// 事前生成
if (PoolManager)
{
    PoolManager->PrewarmPool(AMyEnemy::StaticClass(), 20, 50);
}
```

## API リファレンス

### IPoolable インターフェース

プール可能なActorが実装するインターフェース。

#### メソッド

- `OnPoolActivate()`: プールから取り出され、アクティブ化される時に呼ばれます
- `OnPoolDeactivate()`: プールに返却され、非アクティブ化される時に呼ばれます
- `CanBePooled()`: このActorがプールに返却可能かどうかを判定します

### UObjectPoolManagerSubsystem

ワールド内の全オブジェクトプールを管理するサブシステム。

#### メソッド

- `SpawnFromPool(ActorClass, SpawnTransform, InitialPoolSize, MaxPoolSize)`: プールからActorをスポーン
- `ReturnToPool(Actor)`: Actorをプールに返却
- `PrewarmPool(ActorClass, Count, MaxPoolSize)`: プールを事前に生成
- `GetPoolStats(ActorClass, OutStats)`: プール統計を取得
- `LogAllPoolStats()`: 全プールの統計をログ出力
- `ClearPool(ActorClass)`: 指定クラスのプールをクリア
- `ClearAllPools()`: すべてのプールをクリア

### UObjectPoolBlueprintLibrary

Blueprint用の便利な関数ライブラリ。

#### メソッド

- `SpawnPooledActor(WorldContextObject, ActorClass, Location, Rotation, InitialPoolSize, MaxPoolSize)`: プールからActorをスポーン
- `DespawnPooledActor(Actor)`: Actorをプールに返却
- `PrewarmPool(WorldContextObject, ActorClass, Count, MaxPoolSize)`: プールを事前に生成
- `GetPoolStats(WorldContextObject, ActorClass, OutStats)`: プール統計を取得
- `LogPoolStats(WorldContextObject)`: 全プールの統計をログ出力
- `ClearPool(WorldContextObject, ActorClass)`: 指定クラスのプールをクリア
- `ClearAllPools(WorldContextObject)`: すべてのプールをクリア

### FPoolStats 構造体

プールの統計情報を保持する構造体。

#### プロパティ

- `TotalObjects`: プール内の総オブジェクト数
- `ActiveObjects`: アクティブなオブジェクト数
- `InactiveObjects`: 非アクティブなオブジェクト数
- `PeakActiveObjects`: これまでのピークアクティブ数
- `TotalSpawnCount`: 総スポーン回数
- `ExpandCount`: プール拡張回数

## ベストプラクティス

### 1. 適切な初期プールサイズの設定

ゲームプレイ中の同時存在数を考慮して初期サイズを設定します。

```cpp
// 同時に最大30体の敵が存在する場合
PoolManager->PrewarmPool(AMyEnemy::StaticClass(), 30, 100);
```

### 2. 最大サイズの設定

メモリ使用量を制御するため、最大サイズを設定することをお勧めします。

```cpp
// 最大100体まで
PoolManager->SpawnFromPool(AMyEnemy::StaticClass(), SpawnTransform, 20, 100);
```

### 3. OnPoolDeactivateでの適切なクリーンアップ

プールに返却する前に、すべての状態をリセットします。

```cpp
void AMyEnemy::OnPoolDeactivate_Implementation()
{
    // タイマーのクリア
    GetWorldTimerManager().ClearAllTimersForObject(this);

    // デリゲートのクリア
    OnDeathDelegate.Clear();

    // 物理シミュレーションの停止
    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(GetRootComponent()))
    {
        Root->SetSimulatePhysics(false);
    }
}
```

### 4. 統計情報の監視

開発中は統計情報を定期的にチェックし、プールサイズを最適化します。

```cpp
// デバッグコマンドで統計を表示
void AMyGameMode::ShowPoolStats()
{
    UObjectPoolManagerSubsystem* PoolManager = GetWorld()->GetSubsystem<UObjectPoolManagerSubsystem>();
    if (PoolManager)
    {
        PoolManager->LogAllPoolStats();
    }
}
```

## パフォーマンス

オブジェクトプールを使用することで、以下のパフォーマンス向上が期待できます:

- **メモリアロケーション削減**: 頻繁な new/delete を回避
- **ガベージコレクション負荷軽減**: オブジェクトの再利用により GC の頻度を削減
- **スポーン時間の短縮**: すでに生成済みのオブジェクトを再利用

### 測定例

100体の敵を毎秒生成・破棄する場合:

- プールなし: 10-15ms/frame (スパイク発生)
- プールあり: 1-2ms/frame (安定)

## トラブルシューティング

### プールからスポーンできない

- Actorクラスが正しく設定されているか確認
- ワールドが有効か確認
- ObjectPoolManagerSubsystemが初期化されているか確認

### メモリリークの疑い

- OnPoolDeactivateで適切にクリーンアップしているか確認
- 最大プールサイズを設定しているか確認
- 統計情報でプールサイズの増加を監視

### Actorが正しく初期化されない

- OnPoolActivateで必要な初期化処理をすべて実装しているか確認
- SetActorTransformが正しく呼ばれているか確認

## ライセンス

Copyright Epic Games, Inc. All Rights Reserved.

## サポート

問題が発生した場合は、プロジェクトのIssueトラッカーで報告してください。
