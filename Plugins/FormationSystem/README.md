# Formation System Plugin

パーティメンバーの配置制御と陣形管理を行うUnreal Engine 5プラグインです。

## 機能概要

- **複数の陣形タイプ**: Line（一列）、Column（二列）、Wedge（楔形）、Circle（円形）、Combat Spread（戦闘展開）、Custom（カスタム）
- **リーダー追従ロジック**: パーティリーダーの移動に追従
- **障害物回避**: AIナビゲーションシステムとの統合
- **戦闘時の陣形変更**: 戦闘状態に応じた自動陣形切り替え
- **テレポート機能**: リーダーから離れすぎた場合の自動テレポート
- **ブループリント対応**: すべての主要機能がブループリントから利用可能

## コンポーネント構成

### FormationComponent

パーティリーダーにアタッチするコンポーネントです。陣形全体を管理します。

**主な機能:**
- 陣形タイプの設定と変更
- フォロワーの追加・削除
- 陣形位置の計算
- 陣形スロットの自動生成

**使用方法:**
```cpp
// C++
UFormationComponent* FormationComp = Leader->FindComponentByClass<UFormationComponent>();
FormationComp->SetFormationType(EFormationType::Wedge);
FormationComp->AddFollower(FollowerActor);
```

### FormationFollowerComponent

パーティメンバーにアタッチするコンポーネントです。リーダーへの追従を制御します。

**主な機能:**
- リーダー追従
- AIナビゲーションによる経路探索
- 障害物回避
- 自動テレポート

**使用方法:**
```cpp
// C++
UFormationFollowerComponent* FollowerComp = Member->FindComponentByClass<UFormationFollowerComponent>();
FollowerComp->SetFormationLeader(LeaderActor);
FollowerComp->SetFormationIndex(0); // 陣形内の位置インデックス
```

### FormationLibrary

陣形関連のユーティリティ関数を提供するブループリント関数ライブラリです。

**主な機能:**
- 陣形設定の作成
- 推奨陣形タイプの取得
- 陣形位置の検証
- 陣形設定の補間

## 陣形タイプ

### Line（一列）
リーダーの後ろに一列に並ぶ基本的な陣形です。

### Column（二列）
リーダーの後ろに二列で並ぶ陣形です。中規模パーティに適しています。

### Wedge（楔形）
V字型の陣形で、リーダーが先端に位置します。進軍時に有効です。

### Circle（円形）
リーダーを中心に円形に配置される陣形です。全方位からの攻撃に対応できます。

### Combat Spread（戦闘展開）
戦闘時用の広い間隔を持った陣形です。範囲攻撃への対策に有効です。

### Custom（カスタム）
独自の陣形パターンを定義できます。

## セットアップ方法

### 1. リーダーの設定

```cpp
// リーダーキャラクターにFormationComponentを追加
UFormationComponent* FormationComp = CreateDefaultSubobject<UFormationComponent>(TEXT("FormationComponent"));

// 陣形タイプを設定
FormationComp->SetFormationType(EFormationType::Column);
```

### 2. フォロワーの設定

```cpp
// フォロワーキャラクターにFormationFollowerComponentを追加
UFormationFollowerComponent* FollowerComp = CreateDefaultSubobject<UFormationFollowerComponent>(TEXT("FollowerComponent"));

// リーダーを設定
FollowerComp->SetFormationLeader(LeaderActor);

// 陣形内の位置を設定（0から始まるインデックス）
FollowerComp->SetFormationIndex(0);
```

### 3. 戦闘時の陣形変更

```cpp
// 戦闘開始時
FormationComp->SetFormationType(EFormationType::CombatSpread);

// 戦闘終了時
FormationComp->SetFormationType(EFormationType::Column);
```

## ブループリントでの使用

すべてのコンポーネントとライブラリ関数はブループリントから使用できます。

1. キャラクターブループリントを開く
2. Components パネルで「Add Component」をクリック
3. "Formation Component" または "Formation Follower Component" を検索して追加
4. Details パネルで設定を調整

## パラメータ調整

### FormationComponent

- **Formation Type**: 陣形のタイプ
- **Member Spacing**: メンバー間の距離（デフォルト: 200cm）
- **Enable Obstacle Avoidance**: 障害物回避の有効化
- **Max Follow Distance**: 最大追従距離（これを超えるとテレポート）

### FormationFollowerComponent

- **Formation Index**: 陣形内の位置インデックス
- **Movement Threshold**: 移動開始距離（デフォルト: 100cm）
- **Arrival Threshold**: 到着判定距離（デフォルト: 50cm）
- **Use AI Navigation**: AIナビゲーションの使用
- **Allow Teleport**: テレポートの許可
- **Teleport Distance**: テレポート発動距離（デフォルト: 3000cm）

## 依存関係

- Core
- CoreUObject
- Engine
- AIModule
- NavigationSystem
- GameplayTasks

## 注意事項

- AIナビゲーションを使用する場合、レベルにNav Mesh Bounds Volumeを配置してください
- フォロワーにはAIControllerが必要です
- テレポート機能を無効にする場合、Max Follow Distanceを大きな値に設定してください

## ライセンス

このプラグインはプロジェクトの一部として提供されています。
