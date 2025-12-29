# Wave Spawn System Plugin

ウェーブベースの敵スポーン管理システムを提供するUnreal Engine 5プラグインです。

## 概要

Wave Spawn Systemは、段階的な敵の出現、難易度調整、スポーンポイント管理、ウェーブクリア判定などの機能を提供する包括的なシステムです。

## 主な機能

### 1. ウェーブ管理
- 複数のウェーブを設定可能
- ウェーブごとの敵構成の定義
- ウェーブ間の遅延時間設定
- 自動ループ機能

### 2. 難易度調整
- ウェーブごとの体力倍率の自動増加
- ダメージ倍率の自動増加
- 敵の数の増加
- スポーン間隔の短縮
- 最大倍率の上限設定

### 3. スポーンポイント選択
以下の選択モードから選べます：
- **Sequential**: 順番に選択
- **Random**: ランダムに選択
- **FarthestFromPlayer**: プレイヤーから最も遠い地点を選択
- **OutOfSight**: プレイヤーの視界外を選択

### 4. ウェーブクリア判定
- 全ての敵を倒す必要があるかどうかの設定
- 制限時間の設定（オプション）
- クリア時の報酬識別子

## コンポーネント

### AWaveSpawnManager
ウェーブシステムのメインクラスです。レベルに配置して使用します。

**主要プロパティ:**
- `Waves`: ウェーブの設定配列
- `DifficultyScaling`: 難易度スケーリング設定
- `SpawnPointSelectionMode`: スポーンポイントの選択方法
- `bAutoStartFirstWave`: 自動開始するかどうか
- `bLoopWaves`: ウェーブを無限ループするかどうか

**主要メソッド:**
- `StartWaves()`: ウェーブシステムを開始
- `StartWave(int32 WaveIndex)`: 特定のウェーブを開始
- `StopCurrentWave()`: 現在のウェーブを停止
- `GetCurrentWaveNumber()`: 現在のウェーブ番号を取得
- `GetActiveEnemyCount()`: アクティブな敵の数を取得

**イベント:**
- `OnWaveStarted`: ウェーブ開始時
- `OnWaveCompleted`: ウェーブ完了時
- `OnAllWavesCompleted`: 全ウェーブ完了時
- `OnEnemySpawned`: 敵スポーン時
- `OnEnemyDefeated`: 敵撃破時

### AWaveSpawnPoint
敵のスポーン地点を示すアクターです。

**主要プロパティ:**
- `bIsEnabled`: このスポーンポイントが有効かどうか
- `Priority`: 優先度（高いほど選ばれやすい）
- `SpawnTag`: フィルタリング用のタグ
- `ExclusionRadius`: 他のスポーンを避ける半径

### FWaveConfiguration
ウェーブの設定を定義する構造体です。

**プロパティ:**
- `Enemies`: このウェーブでスポーンする敵の配列
- `PreWaveDelay`: ウェーブ開始前の遅延
- `TimeLimit`: 制限時間（0で無制限）
- `bRequireAllEnemiesDefeated`: 全ての敵を倒す必要があるか
- `RewardIdentifier`: 報酬の識別子

### FEnemySpawnInfo
個々の敵タイプの情報を定義する構造体です。

**プロパティ:**
- `EnemyClass`: スポーンする敵のクラス
- `Count`: スポーンする数
- `SpawnInterval`: スポーン間隔（秒）
- `HealthMultiplier`: 体力倍率
- `DamageMultiplier`: ダメージ倍率

## 使用方法

詳細な使用方法については、[Usage.md](Documentation/Usage.md)を参照してください。

基本的な手順：
1. プロジェクトでプラグインを有効化
2. レベルに`AWaveSpawnManager`を配置
3. レベルに`AWaveSpawnPoint`を配置
4. `AWaveSpawnManager`でウェーブを設定
5. ゲーム開始または`StartWaves()`の呼び出しでシステムを起動

## サンプル

サンプルについては、[Examples.md](Documentation/Examples.md)を参照してください。

## API リファレンス

詳細なAPIリファレンスについては、[API.md](Documentation/API.md)を参照してください。

## ライセンス

Copyright Epic Games, Inc. All Rights Reserved.
