# スキル・アビリティシステム

## 概要
Unreal Engine 5のGameplay Ability System (GAS)を拡張した包括的なスキル・アビリティシステムです。

## 実装されたコンポーネント

### 1. AGAttributeSet (アトリビュートセット)
キャラクターのリソースとステータスを管理します。

**属性:**
- Health (HP) / MaxHealth
- Mana (MP) / MaxMana
- Stamina (スタミナ) / MaxStamina
- Damage (メタ属性)

**機能:**
- ネットワークレプリケーション対応
- 属性の自動クランプ（最小値・最大値）
- ダメージ計算のサポート

### 2. AGAbilitySystemComponent (アビリティシステムコンポーネント)
アビリティの管理とアクティベーションを行います。

**主な機能:**
- アビリティの付与・削除
- クールダウンの確認
- 入力バインディング
- デフォルトアビリティとエフェクトの自動付与

**使用例（Blueprint）:**
```cpp
// アビリティを付与
GrantAbility(MyAbilityClass, 1);

// アビリティをアクティベート
ActivateAbilityByClass(MyAbilityClass);

// クールダウンを確認
bool bOnCooldown = IsAbilityOnCooldown(MyAbilityClass);
float TimeRemaining = GetAbilityCooldownTimeRemaining(MyAbilityClass);
```

### 3. AGGameplayAbility (ゲームプレイアビリティ基底クラス)
すべてのアビリティの基底クラスで、以下の機能を提供します。

**クールダウン管理:**
- `CooldownDuration`: クールダウン時間
- `CooldownTags`: クールダウン用のゲームプレイタグ
- `ApplyCooldown()`: クールダウンを適用

**リソース消費:**
- `ResourceType`: 消費するリソースタイプ（None/Health/Mana/Stamina）
- `ResourceCost`: 消費量
- `HasEnoughResources()`: リソースが十分かチェック
- `ApplyResourceCost()`: リソースを消費

**コンボシステム:**
- `ComboIndex`: コンボチェーン内のインデックス
- `NextComboAbility`: 次のコンボアビリティ
- `ComboWindowDuration`: コンボ入力受付時間
- `CanComboInto()`: 特定のアビリティへコンボ可能かチェック

**Blueprintイベント:**
- `OnAbilityActivated`: アビリティ発動時
- `OnAbilityEnded`: アビリティ終了時

### 4. AGBuffDebuffEffect (バフ/デバフエフェクト)
バフとデバフを管理するシステムです。

**UAGBuffDebuffEffect:**
- `EffectType`: Buff/Debuff/Neutral
- `DisplayName`: 表示名
- `Description`: 説明
- `Icon`: アイコン
- `bCanStack`: スタック可能か
- `MaxStacks`: 最大スタック数

**UAGBuffDebuffManager コンポーネント:**
- `ApplyBuffDebuff()`: バフ/デバフを適用
- `RemoveBuffDebuff()`: バフ/デバフを削除
- `GetActiveBuffs()`: アクティブなバフを取得
- `GetActiveDebuffs()`: アクティブなデバフを取得
- `HasBuffDebuff()`: 特定のエフェクトがアクティブかチェック

**イベント:**
- `OnBuffApplied`: バフ適用時
- `OnDebuffApplied`: デバフ適用時
- `OnBuffRemoved`: バフ削除時
- `OnDebuffRemoved`: デバフ削除時

### 5. AGComboSystem (コンボシステム)
複雑なコンボチェーンを管理します。

**FComboChain 構造体:**
- `Abilities`: コンボ内のアビリティ配列
- `ComboWindow`: コンボ入力受付時間
- `bCanLoop`: ループ可能か

**主な機能:**
- `RegisterComboChain()`: コンボチェーンを登録
- `StartCombo()`: コンボを開始
- `ContinueCombo()`: コンボを継続
- `ResetCombo()`: コンボをリセット
- `GetNextComboAbility()`: 次のコンボアビリティを取得
- `GetComboCount()`: 現在のコンボ数を取得

**イベント:**
- `OnComboCountChanged`: コンボ数変更時
- `OnComboReset`: コンボリセット時

## 使用方法

### キャラクターへの追加

1. キャラクターに `AGAbilitySystemComponent` を追加
2. `AGAttributeSet` を作成して初期化
3. 必要に応じて `AGBuffDebuffManager` と `AGComboSystem` コンポーネントを追加

```cpp
// C++の例
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter()
    {
        // アビリティシステムコンポーネントを作成
        AbilitySystemComponent = CreateDefaultSubobject<UAGAbilitySystemComponent>(TEXT("AbilitySystemComp"));

        // アトリビュートセットを作成
        AttributeSet = CreateDefaultSubobject<UAGAttributeSet>(TEXT("AttributeSet"));

        // バフ/デバフマネージャーを作成
        BuffDebuffManager = CreateDefaultSubobject<UAGBuffDebuffManager>(TEXT("BuffDebuffManager"));

        // コンボシステムを作成
        ComboSystem = CreateDefaultSubobject<UAGComboSystem>(TEXT("ComboSystem"));
    }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UAGAttributeSet* AttributeSet;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGBuffDebuffManager* BuffDebuffManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGComboSystem* ComboSystem;
};
```

### カスタムアビリティの作成

`UAGGameplayAbility` を継承してカスタムアビリティを作成：

```cpp
UCLASS()
class UMyCustomAbility : public UAGGameplayAbility
{
    GENERATED_BODY()

public:
    UMyCustomAbility()
    {
        // クールダウンを設定
        CooldownDuration = 5.0f;

        // リソース消費を設定
        ResourceType = EAbilityResourceType::Mana;
        ResourceCost = 20.0f;

        // コンボ設定
        ComboIndex = 0;
        ComboWindowDuration = 1.5f;
    }

    virtual void ActivateAbility(...) override
    {
        Super::ActivateAbility(...);

        // カスタムロジックをここに実装

        EndAbility(...);
    }
};
```

### Blueprintでの使用

すべてのコンポーネントと関数はBlueprintから使用可能です：

1. **アビリティの付与:**
   - `Grant Ability` ノードを使用

2. **リソースの確認:**
   - `Get Health/Mana/Stamina` でAttributeSetから値を取得

3. **バフ/デバフの適用:**
   - `Apply Buff Debuff` ノードを使用

4. **コンボの管理:**
   - `Start Combo` でコンボ開始
   - `Continue Combo` でコンボ継続
   - `On Combo Count Changed` イベントで通知を受け取る

## アーキテクチャ

このシステムはUnreal Engine 5のGameplay Ability Systemを基盤としており、以下の利点があります：

- **ネットワーク対応:** すべてのアビリティとエフェクトはマルチプレイヤーに対応
- **予測実行:** クライアント側の予測実行でレスポンシブな操作感
- **モジュラー設計:** コンポーネントベースで拡張が容易
- **Blueprint対応:** プログラマーでない人でも設定・調整が可能

## 今後の拡張案

- アビリティのレベルアップシステム
- パッシブアビリティのサポート
- チャージアビリティ（溜め攻撃）
- ターゲティングシステムの統合
- エフェクトのビジュアル表示
