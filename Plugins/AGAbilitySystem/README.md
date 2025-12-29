# AG Ability System Plugin

包括的なゲームプレイアビリティシステムプラグイン

## 概要

AG Ability Systemは、Unreal Engine 5向けの完全なスキル・アビリティシステムを提供するプラグインです。以下の機能を含みます：

- アビリティの基底クラス設計
- クールダウン管理
- リソース消費（MP、スタミナ等）
- コンボシステム
- バフ/デバフ管理

## 特徴

- ✅ **ネットワークレプリケーション対応** - マルチプレイヤーゲームで使用可能
- ✅ **Blueprint完全対応** - プログラマー以外でも調整可能
- ✅ **モジュール式設計** - 必要な機能だけを使用可能
- ✅ **拡張可能** - カスタムアビリティやエフェクトを簡単に追加

## インストール

1. プロジェクトの`Plugins`フォルダに`AGAbilitySystem`フォルダをコピー
2. プロジェクトを再ビルド
3. エディタを開き、プラグインが有効になっていることを確認

## 使用方法

### 1. AttributeSet（リソース管理）

キャラクターにHP、MP、スタミナなどのアトリビュートを追加します。

```cpp
// C++での使用例
UAGAttributeSet* AttributeSet = CreateDefaultSubobject<UAGAttributeSet>(TEXT("AttributeSet"));
```

**アトリビュート:**
- `Health` / `MaxHealth` - HP
- `Mana` / `MaxMana` - MP/マナ
- `Stamina` / `MaxStamina` - スタミナ
- `Damage` - ダメージ計算用メタアトリビュート

### 2. Ability System Component

アビリティシステムの中核となるコンポーネント。

```cpp
// C++での使用例
UAGAbilitySystemComponent* AbilityComp = CreateDefaultSubobject<UAGAbilitySystemComponent>(TEXT("AbilitySystem"));

// アビリティの付与
FGameplayAbilitySpecHandle Handle = AbilityComp->GrantAbility(UMyAbility::StaticClass(), 1, 0);

// アビリティの実行
AbilityComp->TryActivateAbilityByClass(UMyAbility::StaticClass());

// クールダウンチェック
bool bOnCooldown = AbilityComp->IsAbilityOnCooldown(UMyAbility::StaticClass());
float TimeRemaining = AbilityComp->GetAbilityCooldownTimeRemaining(UMyAbility::StaticClass());
```

### 3. Gameplay Ability（アビリティ基底クラス）

カスタムアビリティを作成するための基底クラス。

```cpp
// C++でのカスタムアビリティ作成例
UCLASS()
class UMyFireballAbility : public UAGGameplayAbility
{
    GENERATED_BODY()

public:
    UMyFireballAbility()
    {
        // クールダウン設定
        CooldownDuration = 5.0f;
        CooldownTag = FGameplayTag::RequestGameplayTag(FName("Ability.Cooldown.Fireball"));

        // リソースコスト設定
        ResourceType = EAGResourceType::Mana;
        ResourceCost = 30.0f;

        // コンボ設定
        ComboTag = FGameplayTag::RequestGameplayTag(FName("Combo.Magic"));
        ComboWindowDuration = 2.0f;
    }

    virtual void ActivateAbility(...) override
    {
        Super::ActivateAbility(...);

        // ファイアボールのロジック
        SpawnFireball();

        EndAbility(...);
    }
};
```

**Blueprint使用:**
- `BP_OnAbilityActivated` - アビリティ発動時
- `BP_OnAbilityEnded` - アビリティ終了時

**プロパティ:**
- `CooldownDuration` - クールダウン時間（秒）
- `CooldownTag` - クールダウン識別用GameplayTag
- `ResourceType` - 消費リソース種類（None/Mana/Stamina/Health）
- `ResourceCost` - 消費量
- `ComboTag` - コンボ識別用GameplayTag
- `ComboWindowDuration` - コンボ入力受付時間（秒）

### 4. Buff/Debuff System

バフとデバフを管理するシステム。

```cpp
// C++での使用例
UAGBuffDebuffManager* BuffManager = CreateDefaultSubobject<UAGBuffDebuffManager>(TEXT("BuffManager"));

// バフの適用
FAGBuffDebuffData BuffData;
BuffData.EffectTag = FGameplayTag::RequestGameplayTag(FName("Buff.AttackUp"));
BuffData.EffectType = EAGEffectType::Buff;
BuffData.Duration = 10.0f;
BuffData.MaxStacks = 3;
BuffData.GameplayEffect = UGE_AttackBuff::StaticClass();

BuffManager->ApplyEffect(BuffData);

// バフのチェック
bool bHasBuff = BuffManager->HasEffect(BuffData.EffectTag);
int32 Stacks = BuffManager->GetEffectStackCount(BuffData.EffectTag);
float TimeLeft = BuffManager->GetEffectTimeRemaining(BuffData.EffectTag);

// バフの削除
BuffManager->RemoveEffect(BuffData.EffectTag);
```

**Blueprint使用:**
- `OnBuffDebuffApplied` - エフェクト適用時のイベント
- `OnBuffDebuffRemoved` - エフェクト削除時のイベント

### 5. Combo System

コンボチェーンを管理するシステム。

```cpp
// C++での使用例
UAGComboSystem* ComboSystem = CreateDefaultSubobject<UAGComboSystem>(TEXT("ComboSystem"));

// コンボチェーンの登録
FAGComboChain ComboChain;
ComboChain.ComboTag = FGameplayTag::RequestGameplayTag(FName("Combo.MeleeAttack"));
ComboChain.AbilityChain.Add(UAttack1::StaticClass());
ComboChain.AbilityChain.Add(UAttack2::StaticClass());
ComboChain.AbilityChain.Add(UAttack3::StaticClass());
ComboChain.ComboWindowDuration = 1.5f;

ComboSystem->RegisterComboChain(ComboChain);

// コンボの開始
ComboSystem->StartCombo(ComboChain.ComboTag);

// コンボの進行
ComboSystem->ProgressCombo();

// コンボ状態の確認
int32 CurrentCount = ComboSystem->GetCurrentComboCount();
bool bActive = ComboSystem->IsComboActive();
```

**Blueprint使用:**
- `OnComboProgressed` - コンボ進行時のイベント
- `OnComboEnded` - コンボ終了時のイベント

## ファイル構成

```
Plugins/AGAbilitySystem/
├── AGAbilitySystem.uplugin          # プラグイン定義ファイル
├── README.md                         # このファイル
└── Source/
    └── AGAbilitySystem/
        ├── AGAbilitySystem.Build.cs  # ビルド設定
        ├── Public/
        │   ├── AGAbilitySystem.h            # モジュールヘッダー
        │   ├── AGAttributeSet.h             # アトリビュートセット
        │   ├── AGAbilitySystemComponent.h   # アビリティシステムコンポーネント
        │   ├── AGGameplayAbility.h          # アビリティ基底クラス
        │   ├── AGBuffDebuffEffect.h         # バフ/デバフシステム
        │   └── AGComboSystem.h              # コンボシステム
        └── Private/
            ├── AGAbilitySystem.cpp
            ├── AGAttributeSet.cpp
            ├── AGAbilitySystemComponent.cpp
            ├── AGGameplayAbility.cpp
            ├── AGBuffDebuffEffect.cpp
            └── AGComboSystem.cpp
```

## 依存関係

このプラグインは以下のUnreal Engineモジュールに依存します：

- Core
- CoreUObject
- Engine
- GameplayTags
- GameplayTasks
- NetCore

## サンプルプロジェクトへの統合

### Character Classへの追加

```cpp
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter()
    {
        // Ability System Component
        AbilitySystemComponent = CreateDefaultSubobject<UAGAbilitySystemComponent>(TEXT("AbilitySystem"));

        // Attribute Set
        AttributeSet = CreateDefaultSubobject<UAGAttributeSet>(TEXT("AttributeSet"));

        // Buff/Debuff Manager
        BuffManager = CreateDefaultSubobject<UAGBuffDebuffManager>(TEXT("BuffManager"));

        // Combo System
        ComboSystem = CreateDefaultSubobject<UAGComboSystem>(TEXT("ComboSystem"));
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();

        // アビリティの付与
        if (AbilitySystemComponent)
        {
            AbilitySystemComponent->GrantAbility(UMyAbility::StaticClass(), 1);
        }
    }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGAttributeSet* AttributeSet;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGBuffDebuffManager* BuffManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAGComboSystem* ComboSystem;
};
```

## ライセンス

Copyright Epic Games, Inc. All Rights Reserved.

## サポート

問題や質問がある場合は、プロジェクトのIssueトラッカーを使用してください。
