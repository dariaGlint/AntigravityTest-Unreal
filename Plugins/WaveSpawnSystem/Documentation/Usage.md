# Wave Spawn System - 使用方法

## セットアップ

### 1. プラグインの有効化

1. Unreal Editorを開く
2. Edit > Plugins メニューを選択
3. "Wave Spawn System"を検索
4. チェックボックスをオンにして有効化
5. エディタを再起動

### 2. ウェーブマネージャーの配置

1. レベルエディタで、Place Actorsパネルを開く
2. "WaveSpawnManager"を検索
3. `AWaveSpawnManager`をレベルにドラッグ＆ドロップ

### 3. スポーンポイントの配置

1. Place Actorsパネルで"WaveSpawnPoint"を検索
2. `AWaveSpawnPoint`を敵をスポーンさせたい場所に複数配置
3. 各スポーンポイントの矢印コンポーネントで、敵の向きを調整

## ウェーブの設定

### 基本的な設定

1. レベルに配置した`AWaveSpawnManager`を選択
2. Detailsパネルで以下を設定：

#### ウェーブ設定
- **Waves**: ウェーブ配列の要素を追加
  - 各ウェーブで：
    - **Enemies**: 敵タイプの配列を追加
      - **Enemy Class**: スポーンする敵のBlueprintクラスを選択
      - **Count**: スポーンする数を設定
      - **Spawn Interval**: スポーン間隔（秒）を設定
      - **Health Multiplier**: 体力の倍率
      - **Damage Multiplier**: ダメージの倍率
    - **Pre Wave Delay**: ウェーブ開始前の待機時間
    - **Time Limit**: 制限時間（0で無制限）
    - **Require All Enemies Defeated**: 全ての敵を倒す必要があるか

#### 難易度スケーリング
- **Difficulty Scaling**:
  - **Health Scale Per Wave**: ウェーブごとの体力増加率（例: 0.1 = 10%増加）
  - **Damage Scale Per Wave**: ウェーブごとのダメージ増加率
  - **Count Scale Per Wave**: ウェーブごとの敵数増加率
  - **Spawn Interval Reduction Per Wave**: スポーン間隔の短縮（秒）
  - **Max Health Multiplier**: 体力倍率の上限
  - **Max Damage Multiplier**: ダメージ倍率の上限

#### スポーンポイント設定
- **Spawn Point Selection Mode**: スポーンポイントの選択方法
  - Sequential: 順番に選択
  - Random: ランダムに選択
  - Farthest From Player: プレイヤーから最も遠い地点
  - Out Of Sight: プレイヤーの視界外
- **Spawn Points**: 使用するスポーンポイントの配列（空の場合は自動検出）
- **Spawn Point Filter Tag**: スポーンポイントをフィルタリングするタグ

#### その他の設定
- **Auto Start First Wave**: ゲーム開始時に自動でウェーブを開始
- **Auto Start Delay**: 自動開始の遅延時間
- **Loop Waves**: 全ウェーブ完了後にループするか

## Blueprintでの使用

### ウェーブの開始

```cpp
// C++の場合
WaveSpawnManager->StartWaves();

// または特定のウェーブから開始
WaveSpawnManager->StartWave(2);  // 2番目のウェーブから開始
```

Blueprintの場合、`AWaveSpawnManager`への参照を取得し、`Start Waves`ノードを呼び出します。

### イベントのバインド

```cpp
// C++の場合
WaveSpawnManager->OnWaveStarted.AddDynamic(this, &AYourClass::OnWaveStartedHandler);
WaveSpawnManager->OnWaveCompleted.AddDynamic(this, &AYourClass::OnWaveCompletedHandler);
WaveSpawnManager->OnEnemySpawned.AddDynamic(this, &AYourClass::OnEnemySpawnedHandler);
```

Blueprintの場合、Event Dispatcherとして以下のイベントにバインド可能：
- **On Wave Started**: ウェーブ開始時
- **On Wave Completed**: ウェーブ完了時
- **On All Waves Completed**: 全ウェーブ完了時
- **On Enemy Spawned**: 敵スポーン時
- **On Enemy Defeated**: 敵撃破時

### 状態の取得

```cpp
// 現在のウェーブ番号を取得（1から始まる）
int32 CurrentWave = WaveSpawnManager->GetCurrentWaveNumber();

// アクティブな敵の数を取得
int32 ActiveEnemies = WaveSpawnManager->GetActiveEnemyCount();

// ウェーブがアクティブかチェック
bool bIsActive = WaveSpawnManager->IsWaveActive();
```

## 敵キャラクターの対応

スポーンされる敵は、`OnDestroyed`イベントでシステムに通知されます。特別な対応は不要ですが、敵が難易度スケーリングを反映するには、以下の対応が推奨されます：

1. 敵キャラクターに`HealthMultiplier`と`DamageMultiplier`パラメータを追加
2. スポーン時にこれらのパラメータを設定
3. ダメージ計算とヘルス初期化でこれらの倍率を適用

## トラブルシューティング

### 敵がスポーンされない
- スポーンポイントが正しく配置されているか確認
- 敵クラスが正しく設定されているか確認
- ウェーブマネージャーの`StartWaves()`が呼ばれているか確認

### スポーンポイントが見つからない
- `AWaveSpawnPoint`がレベルに配置されているか確認
- スポーンポイントフィルタタグが正しく設定されているか確認

### 難易度スケーリングが機能しない
- `DifficultyScaling`の値が設定されているか確認
- 敵キャラクターが倍率を正しく適用しているか確認
