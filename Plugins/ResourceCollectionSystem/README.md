# Resource Collection System

リソース収集システムプラグイン - UE5用の包括的な採掘・採取システム

## 概要

このプラグインは、採掘ノード管理、リスポーン制御、ドロップテーブル、スキルベースの収集量変化を含む、完全なリソース収集システムを提供します。

## 主な機能

- **リソースノード管理**: 採掘可能なノードの作成と管理
- **リスポーン制御**: カスタマイズ可能な自動リスポーンシステム
- **ドロップテーブル**: 柔軟なアイテムドロップ設定
- **スキルシステム**: 収集量がスキルレベルに応じて変化
- **視覚的フィードバック**: パーティクルエフェクトとサウンド対応
- **Blueprint対応**: 完全なBlueprint統合

## コンポーネント

### ResourceNodeComponent

任意のActorに追加してリソースノードにできるコンポーネント。

**主要プロパティ:**
- `DropTable`: ドロップテーブルアセット
- `MaxHarvestCount`: 採掘可能回数 (0 = 無限)
- `RespawnTime`: リスポーン時間（秒）
- `RespawnTimeVariance`: リスポーン時間のランダム変動
- `RequiredHarvesterTags`: 採掘に必要なゲームプレイタグ

**主要関数:**
- `TryHarvest()`: リソースの採掘を試みる
- `CanHarvest()`: 採掘可能かチェック
- `ForceRespawn()`: 強制的にリスポーンさせる

**イベント:**
- `OnHarvested`: 採掘時に発火
- `OnDepleted`: 枯渇時に発火
- `OnRespawned`: リスポーン時に発火

### ResourceNodeActor

ワールドに配置可能なリソースノードの基底クラス。

**特徴:**
- 自動的にメッシュの切り替え
- 枯渇時の非表示オプション
- マテリアルの自動切り替え

### HarvestingSkillComponent

キャラクターに追加して採掘スキルを管理するコンポーネント。

**機能:**
- スキルレベルと経験値の管理
- スキルレベルに基づく収集量ボーナス
- 自動レベルアップ処理

### ResourceDropTable

ドロップアイテムを定義するデータアセット。

**設定:**
- アイテムと数量範囲
- ドロップ確率
- 全アイテムドロップ or ランダム1個選択

## 使い方

### 1. リソースノードの作成

**方法A: Blueprintで新規作成**
```
1. ResourceNodeActorを親クラスとしてBlueprintを作成
2. AvailableMesh/DepletedMeshを設定
3. ResourceNodeComponentの設定を調整
```

**方法B: 既存Actorに追加**
```
1. 既存ActorにResourceNodeComponentを追加
2. DropTableを設定
3. 必要に応じてイベントをバインド
```

### 2. ドロップテーブルの作成

```
1. Content Browserで右クリック
2. Miscellaneous > Data Asset > ResourceDropTable
3. Dropsに各アイテムを追加し、ドロップ確率と数量を設定
```

### 3. キャラクターに採掘機能を追加

**方法A: HarvestingSkillComponentを使用**
```cpp
// C++
UHarvestingSkillComponent* SkillComp = CreateDefaultSubobject<UHarvestingSkillComponent>(TEXT("HarvestingSkills"));

// Blueprint
1. HarvestingSkillComponentを追加
2. Skillsにスキルを追加（Mining, Woodcutting等）
```

**方法B: インターフェースを実装**
```cpp
// C++で IResourceHarvesterInterface を実装
class AMyCharacter : public ACharacter, public IResourceHarvesterInterface
{
    virtual float GetSkillModifier_Implementation(const FGameplayTag& ResourceTag) const override;
};
```

### 4. 採掘の実行

**Blueprintから:**
```
Use HarvestResourceNode function from ResourceCollectionLibrary
```

**C++から:**
```cpp
UResourceCollectionLibrary::HarvestResourceNode(
    Harvester,
    ResourceNode,
    InventoryComponent,
    SkillTag
);
```

## ゲームプレイタグの使用例

推奨されるタグ構造:

```
Skill.Mining          // 採掘スキル
Skill.Woodcutting     // 伐採スキル
Skill.Herbalism       // 採取スキル
Skill.Fishing         // 釣りスキル

Resource.Ore          // 鉱石リソース
Resource.Wood         // 木材リソース
Resource.Herb         // 薬草リソース

Tool.Pickaxe          // ツルハシ装備
Tool.Axe              // 斧装備
```

## カスタマイズ例

### スキルボーナスの調整

```cpp
// FHarvestingSkillのBonusPerLevelを変更
Skill.BonusPerLevel = 0.1f; // レベルごとに10%ボーナス
```

### カスタムレベルアップ曲線

```cpp
// HarvestingSkillComponent.cppのCheckLevelUp()を修正
ExperienceToNextLevel = CustomFunction(Level);
```

### ビジュアルエフェクトの追加

```cpp
// ResourceNodeActorのUpdateVisualState()をオーバーライド
void AMyResourceNode::UpdateVisualState_Implementation()
{
    Super::UpdateVisualState_Implementation();
    // カスタムエフェクトを追加
}
```

## 統合

既存のInventoryComponentと自動的に統合します。採掘されたアイテムは自動的にインベントリに追加されます。

## ライセンス

プロジェクトのライセンスに準じます。
