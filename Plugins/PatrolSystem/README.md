# Patrol System Plugin

UE5用のパトロールシステムプラグイン

## 概要

このプラグインは、NPCやAIキャラクターのパトロール行動を簡単に実装するためのシステムです。StateTreeと連携して動作し、柔軟なパトロールルートの設定が可能です。

## 主な機能

### 1. ウェイポイント管理 (PatrolWaypoint)
- 各ウェイポイントでの待機時間設定
- 優先度設定（ランダムモード時の重み付け）
- 個別の有効/無効切り替え
- エディタでのビジュアル表示

### 2. パトロールルート (PatrolRouteComponent)
- 3つのパトロールモード：
  - **Sequential**: 順序通りに巡回
  - **Random**: ランダムに巡回（優先度による重み付け対応）
  - **PingPong**: 往復巡回
- ループ設定
- 移動速度のカスタマイズ
- 開始地点の選択（最初のウェイポイント/最寄りのウェイポイント）

### 3. StateTreeタスク (StateTreePatrolTask)
- StateTreeと統合されたパトロールタスク
- 自動的なウェイポイント間移動
- 待機時間の自動制御
- パスファインディング対応

## 使用方法

### 1. プラグインの有効化
1. プロジェクト設定を開く
2. Plugins > AI から "Patrol System" を探す
3. チェックボックスを有効にする
4. エディタを再起動

### 2. パトロールルートの設定

#### ウェイポイントの配置
1. レベルエディタで `PatrolWaypoint` アクターを配置
2. プロパティで待機時間や優先度を設定
3. 必要な数だけウェイポイントを配置

#### パトロールルートコンポーネントの追加
1. AIキャラクターのBlueprintを開く
2. `PatrolRouteComponent` を追加
3. Detailsパネルで以下を設定：
   - Waypoints: 配置したウェイポイントを順番に追加
   - Patrol Mode: Sequential/Random/PingPongから選択
   - Loop Route: ループするかどうか
   - Movement Speed: 移動速度（-1で既定値）

### 3. StateTreeでの使用

1. StateTreeアセットを作成または開く
2. 新しいタスクノードを追加
3. "Patrol Task" を選択
4. パラメータを設定：
   - Patrol Route: パトロールルートコンポーネントへの参照
   - AI Controller: AIコントローラーへの参照
   - Acceptance Radius: 到達判定の半径（既定: 100.0）

## プロパティ詳細

### PatrolWaypoint
| プロパティ | 型 | 説明 | 既定値 |
|-----------|-----|------|--------|
| Wait Time | float | ウェイポイント到達時の待機時間（秒） | 2.0 |
| Priority | float | ランダムモード時の優先度（重み） | 1.0 |
| Is Enabled | bool | ウェイポイントの有効/無効 | true |
| Debug Color | FColor | デバッグ表示色 | Green |
| Debug Display Radius | float | デバッグ表示サイズ | 50.0 |

### PatrolRouteComponent
| プロパティ | 型 | 説明 | 既定値 |
|-----------|-----|------|--------|
| Waypoints | Array | ウェイポイントのリスト | - |
| Patrol Mode | Enum | パトロールモード | Sequential |
| Loop Route | bool | ループするか（Sequential時） | true |
| Start From First Waypoint | bool | 最初のウェイポイントから開始 | true |
| Movement Speed | float | 移動速度（-1で既定値使用） | -1.0 |

### StateTreePatrolTask
| プロパティ | 型 | 説明 | 既定値 |
|-----------|-----|------|--------|
| Patrol Route | UPatrolRouteComponent* | パトロールルート | - |
| AI Controller | AAIController* | AIコントローラー | - |
| Acceptance Radius | float | 到達判定半径 | 100.0 |

## 技術仕様

- **対応エンジンバージョン**: Unreal Engine 5.7+
- **依存モジュール**:
  - AIModule
  - NavigationSystem
  - StateTreeModule
  - GameplayStateTreeModule
  - GameplayTags

## ライセンス

Copyright Epic Games, Inc. All Rights Reserved.
