# AntigravityTest プロジェクトアーキテクチャ

このドキュメントは、AntigravityTestプロジェクトの3つのゲームプレイバリアント（Platforming、Combat、SideScrolling）の構造と使い分けを説明します。

## 目次

- [プロジェクトアーキテクチャ概要](#プロジェクトアーキテクチャ概要)
- [バリアント別詳細](#バリアント別詳細)
  - [Variant_Platforming](#variant_platforming)
  - [Variant_Combat](#variant_combat)
  - [Variant_SideScrolling](#variant_sidescrolling)
- [モジュール構成](#モジュール構成)
- [バリアント間の連携](#バリアント間の連携)
- [新機能実装時のガイドライン](#新機能実装時のガイドライン)
- [使用システムとの統合](#使用システムとの統合)

---

## プロジェクトアーキテクチャ概要

### AntigravityTestプロジェクトの全体像

AntigravityTestは、Unreal Engine 5.7で開発された実験的なC++ゲームプロジェクトです。このプロジェクトは、**単一のUE5プロジェクト内に3つの異なるゲームプレイバリアント**を含む特殊な構造を採用しています。

各バリアントは独立したゲームプレイ体験を提供しながら、共通のエンジン機能とプロジェクトインフラストラクチャを共有します。

```
AntigravityTest (プロジェクトルート)
│
├── Source/AntigravityTest/          # C++ソースコード
│   ├── [共通コンポーネント]          # 全バリアントで共有
│   ├── Variant_Platforming/         # プラットフォーマー専用
│   ├── Variant_Combat/              # 戦闘アクション専用
│   └── Variant_SideScrolling/       # 横スクロール専用
│
└── Content/                         # Blueprintとアセット
    ├── [共通アセット]                # 全バリアントで共有
    ├── Variant_Platforming/         # プラットフォーマー専用
    ├── Variant_Combat/              # 戦闘アクション専用
    └── Variant_SideScrolling/       # 横スクロール専用
```

### なぜ3つのバリアントに分かれているか

このプロジェクトがバリアント構造を採用している理由：

1. **ゲームプレイの多様性**: 各バリアントは異なるゲームジャンルとメカニクスを実装
   - Platforming: 3Dプラットフォーマー（ジャンプ、ダッシュ、壁ジャンプ）
   - Combat: 戦闘アクション（コンボ攻撃、AI戦闘、ダメージシステム）
   - SideScrolling: 2D横スクロール（物理パズル、インタラクション）

2. **実験とプロトタイピング**: 異なるゲームメカニクスを同一プロジェクト内で試験可能

3. **コードの再利用**: 共通システム（Enhanced Input、Gameplay Abilities等）を全バリアントで活用

4. **学習とリファレンス**: UE5の様々な機能の実装例を提供

### 共通部分と個別部分の区別

#### 共通部分（全バリアントで共有）

**C++クラス**:
- `AAntigravityTestGameMode` - 基底GameModeクラス
- `AAntigravityGoalGameMode` - ゴール/クリア条件を持つGameMode基底クラス
- `UInventoryComponent` - インベントリシステム
- `UAntigravityHUD` - HUD基底クラス
- `GA_Jump`, `GA_JumpWait` - Gameplay Ability System (GAS) 関連

**モジュール/システム**:
- Enhanced Input System
- Gameplay Ability System (GAS)
- State Tree AI
- UMG UI システム
- AIModule (Behavior Tree, EQS)

**Content**:
- `Content/ThirdPerson/` - 共通のサードパーソンアセット
- `Content/Input/` - 共通入力アクション
- 共通のキャラクターメッシュとアニメーション

#### 個別部分（バリアント専用）

各バリアントは以下を独自に実装：

- **Character クラス**: 各バリアント専用の移動とアクション
- **GameMode**: バリアント固有のゲームルール
- **PlayerController**: 入力処理とカメラ制御
- **AI実装**: 各バリアントのゲームプレイに特化したAI
- **Input Mapping Context**: バリアント固有の入力設定
- **Animation Blueprint**: バリアント専用のアニメーションロジック
- **UI Widget**: バリアント固有のUI

---

## バリアント別詳細

## Variant_Platforming

### 目的と用途

**Platforming バリアント**は、3Dプラットフォーマーゲームのメカニクスを実装した実験的なバリアントです。ジャンプ、ダッシュ、壁ジャンプなどの高速移動に重点を置いています。

**主な特徴**:
- Press and Hold Jump（ジャンプボタン長押しで高度調整）
- Double Jump（二段ジャンプ）
- Wall Jump（壁ジャンプ）
- Dash（ダッシュ動作）
- Coyote Time（崖から落ちた直後でもジャンプ可能）

### ディレクトリ構造

```
Source/AntigravityTest/Variant_Platforming/
├── Animation/
│   ├── AnimNotify_EndDash.h/.cpp        # ダッシュ終了通知
│
├── PlatformingCharacter.h/.cpp          # プレイヤーキャラクター
├── PlatformingGameMode.h/.cpp           # ゲームモード
└── PlatformingPlayerController.h/.cpp   # プレイヤーコントローラー

Content/Variant_Platforming/
├── Input/
│   ├── Actions/
│   │   └── IA_Dash.uasset              # ダッシュ入力アクション
│   ├── IMC_Platforming.uasset          # Input Mapping Context
│   ├── UI_TouchInterface_Platforming.uasset
│   └── BPI_TouchInterface_Platforming.uasset
│
├── Blueprints/
│   ├── BP_PlatformingCharacter.uasset  # キャラクターBlueprint
│   ├── BP_PlatformingGameMode.uasset
│   └── BP_PlatformingPlayerController.uasset
│
├── Anims/
│   ├── ABP_Manny_Platforming.uasset    # Animation Blueprint
│   └── AM_Dash.uasset                  # ダッシュAnimMontage
│
├── VFX/
│   └── NS_Jump_Trail.uasset            # ジャンプトレイルエフェクト
│
└── Lvl_Platforming.umap                # プラットフォーミングレベル
```

### 主要クラス

#### APlatformingCharacter

**ファイル**: `Source/AntigravityTest/Variant_Platforming/PlatformingCharacter.h`

**継承関係**: `ACharacter` (UE標準)

**責務**:
- 高度なジャンプメカニクス（二段ジャンプ、壁ジャンプ、Coyote Time）
- ダッシュ動作
- カメラ制御（Spring Arm + Follow Camera）
- Enhanced Input System連携

**主要プロパティ**:
```cpp
// 入力アクション
UPROPERTY(EditAnywhere, Category="Input")
UInputAction* JumpAction;
UInputAction* MoveAction;
UInputAction* LookAction;
UInputAction* DashAction;

// 壁ジャンプ設定
UPROPERTY(EditAnywhere, Category="Wall Jump")
float WallJumpTraceDistance = 50.0f;
float WallJumpBounceImpulse = 800.0f;
float WallJumpVerticalImpulse = 900.0f;

// Coyote Time設定
UPROPERTY(EditAnywhere, Category="Coyote Time")
float MaxCoyoteTime = 0.16f;

// ダッシュ設定
UPROPERTY(EditAnywhere, Category="Dash")
UAnimMontage* DashMontage;
```

**主要メソッド**:
- `DoMove(float Right, float Forward)` - 移動処理（Blueprint呼び出し可能）
- `DoLook(float Yaw, float Pitch)` - 視点移動
- `DoDash()` - ダッシュ実行
- `DoJumpStart() / DoJumpEnd()` - ジャンプ開始/終了
- `MultiJump()` - 高度なジャンプ判定（二段ジャンプ、壁ジャンプ）
- `EndDash()` - ダッシュ終了（AnimNotifyから呼び出し）

#### APlatformingGameMode

**継承関係**: `AAntigravityGoalGameMode` → `AAntigravityTestGameMode` → `AGameModeBase`

プラットフォーミング専用のゲームルールを管理します。

#### APlatformingPlayerController

プレイヤー入力とカメラ制御を管理します。

### 使用している機能

- **Enhanced Input System**: すべての入力処理
- **Character Movement Component**: UEの標準移動システムを拡張
- **Animation Montage**: ダッシュアニメーション制御
- **AnimNotify**: ダッシュ終了タイミングの通知
- **Blueprint Implementable Event**: ジャンプトレイルエフェクトの制御

### Content/内の対応Blueprint

- **BP_PlatformingCharacter**: `APlatformingCharacter`の具体的な実装
- **BP_PlatformingGameMode**: ゲームモード設定
- **ABP_Manny_Platforming**: キャラクターアニメーションロジック
- **Lvl_Platforming**: プラットフォーミング専用レベル

---

## Variant_Combat

### 目的と用途

**Combat バリアント**は、近接戦闘システムと敵AIを実装した戦闘アクションゲームのバリアントです。コンボ攻撃、チャージ攻撃、ダメージシステム、AI戦闘などを特徴としています。

**主な特徴**:
- コンボ攻撃システム（多段攻撃の連鎖）
- チャージ攻撃（ボタン長押しで強力な攻撃）
- ダメージシステム（HP、ノックバック、死亡、リスポーン）
- 敵AI（State Tree、Behavior Tree、EQS）
- UI（ライフバー表示）

### ディレクトリ構造

```
Source/AntigravityTest/Variant_Combat/
├── AI/
│   ├── CombatAIController.h/.cpp           # AI制御
│   ├── CombatEnemy.h/.cpp                  # 敵キャラクター
│   ├── CombatEnemySpawner.h/.cpp          # 敵スポーナー
│   ├── CombatStateTreeUtility.h/.cpp      # State Tree ユーティリティ
│   ├── EnvQueryContext_Danger.h/.cpp      # EQS: 危険検知
│   └── EnvQueryContext_Player.h/.cpp      # EQS: プレイヤー検知
│
├── Animation/
│   ├── AnimNotify_CheckChargedAttack.h/.cpp  # チャージ攻撃チェック
│   ├── AnimNotify_CheckCombo.h/.cpp          # コンボチェック
│   └── AnimNotify_DoAttackTrace.h/.cpp       # 攻撃判定トレース
│
├── Gameplay/
│   ├── CombatActivationVolume.h/.cpp       # 戦闘エリア起動
│   ├── CombatCheckpointVolume.h/.cpp       # チェックポイント
│   ├── CombatDamageableBox.h/.cpp          # 破壊可能オブジェクト
│   ├── CombatDummy.h/.cpp                  # 練習用ダミー
│   └── CombatLavaFloor.h/.cpp              # 溶岩床（継続ダメージ）
│
├── Interfaces/
│   ├── CombatAttacker.h/.cpp               # 攻撃者インターフェース
│   ├── CombatDamageable.h/.cpp             # ダメージ可能インターフェース
│   └── CombatActivatable.h/.cpp            # 起動可能インターフェース
│
├── UI/
│   └── CombatLifeBar.h/.cpp                # ライフバーウィジェット
│
├── CombatCharacter.h/.cpp                  # 戦闘キャラクター
├── CombatGameMode.h/.cpp                   # ゲームモード
└── CombatPlayerController.h/.cpp           # プレイヤーコントローラー

Content/Variant_Combat/
├── Input/
│   ├── Actions/
│   │   ├── IA_ComboAttack.uasset          # コンボ攻撃入力
│   │   ├── IA_ChargedAttack.uasset        # チャージ攻撃入力
│   │   └── IA_ToggleCameraSide.uasset     # カメラ切替
│   └── IMC_Combat.uasset                  # Input Mapping Context
│
├── Blueprints/
│   ├── AI/
│   │   ├── BP_CombatEnemy.uasset          # 敵Blueprint
│   │   ├── BP_CombatAIController.uasset   # AI Controller
│   │   ├── BP_CombatEnemySpawner.uasset   # スポーナー
│   │   ├── ST_CombatEnemy.uasset          # State Tree
│   │   ├── EnvQuery_Evade.uasset          # EQS: 回避
│   │   ├── EnvQuery_Flank.uasset          # EQS: 側面攻撃
│   │   └── EnvQuery_Fallback.uasset       # EQS: 退却
│   │
│   ├── Interactables/
│   │   ├── BP_CombatDamageableBox.uasset
│   │   ├── BP_CombatLavaFloor.uasset
│   │   ├── BP_CombatActivationVolume.uasset
│   │   ├── BP_CombatDummy.uasset
│   │   └── BP_CombatCheckpointVolume.uasset
│   │
│   ├── BP_CombatCharacter.uasset
│   ├── BP_CombatGameMode.uasset
│   ├── BP_CombatPlayerController.uasset
│   ├── BP_CameraShake_Hit_Player.uasset   # カメラシェイク
│   └── BP_CameraShake_Hit_Enemy.uasset
│
├── Anims/
│   ├── ABP_Manny_Combat.uasset            # Animation Blueprint
│   ├── AM_ComboAttack.uasset              # コンボアニメーション
│   └── AM_ChargedAttack.uasset            # チャージアニメーション
│
├── Materials/
│   ├── M_Lava.uasset                      # 溶岩マテリアル
│   └── MI_Box_Destroyed.uasset            # 破壊済みボックス
│
├── VFX/
│   └── NS_Damage.uasset                   # ダメージエフェクト
│
├── UI/
│   └── WBP_LifeBar.uasset                 # ライフバーWidget
│
└── Lvl_Combat.umap                        # 戦闘レベル
```

### 主要クラス

#### ACombatCharacter

**ファイル**: `Source/AntigravityTest/Variant_Combat/CombatCharacter.h`

**継承関係**: `ACharacter` (UE標準)

**実装インターフェース**: `ICombatAttacker`, `ICombatDamageable`

**責務**:
- コンボ攻撃システム
- チャージ攻撃システム
- ダメージ処理とHP管理
- ノックバック/ラグドール
- 死亡とリスポーン
- ライフバー表示

**主要プロパティ**:
```cpp
// コンポーネント
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
USpringArmComponent* CameraBoom;
UCameraComponent* FollowCamera;
UWidgetComponent* LifeBar;

// ダメージ設定
UPROPERTY(EditAnywhere, Category="Damage")
float MaxHP = 5.0f;
float CurrentHP = 0.0f;
FLinearColor LifeBarColor;

// コンボ攻撃設定
UPROPERTY(EditAnywhere, Category="Melee Attack|Combo")
UAnimMontage* ComboAttackMontage;
TArray<FName> ComboSectionNames;
float ComboInputCacheTimeTolerance = 0.45f;

// チャージ攻撃設定
UPROPERTY(EditAnywhere, Category="Melee Attack|Charged")
UAnimMontage* ChargedAttackMontage;
FName ChargeLoopSection;
FName ChargeAttackSection;

// 攻撃トレース設定
UPROPERTY(EditAnywhere, Category="Melee Attack|Trace")
float MeleeTraceDistance = 75.0f;
float MeleeTraceRadius = 75.0f;
float MeleeDamage = 1.0f;
float MeleeKnockbackImpulse = 250.0f;
```

**主要メソッド**:

**入力処理**:
- `DoMove(float Right, float Forward)` - 移動
- `DoLook(float Yaw, float Pitch)` - 視点移動
- `DoComboAttackStart() / DoComboAttackEnd()` - コンボ攻撃
- `DoChargedAttackStart() / DoChargedAttackEnd()` - チャージ攻撃

**ICombatAttacker インターフェース**:
- `DoAttackTrace(FName DamageSourceBone)` - 攻撃判定（AnimNotifyから呼び出し）
- `CheckCombo()` - コンボ継続チェック（AnimNotifyから呼び出し）
- `CheckChargedAttack()` - チャージループチェック（AnimNotifyから呼び出し）

**ICombatDamageable インターフェース**:
- `ApplyDamage(float Damage, AActor* DamageCauser, ...)` - ダメージ適用
- `HandleDeath()` - 死亡処理
- `ApplyHealing(float Healing, AActor* Healer)` - 回復処理
- `NotifyDanger(const FVector& DangerLocation, ...)` - 危険通知

**Blueprint イベント**:
- `BP_ToggleCamera()` - カメラ切替（Blueprint実装）
- `DealtDamage(float Damage, const FVector& ImpactPoint)` - ダメージエフェクト
- `ReceivedDamage(...)` - 被ダメージエフェクト

#### ACombatEnemy

**ファイル**: `Source/AntigravityTest/Variant_Combat/AI/CombatEnemy.h`

**継承関係**: `ACombatCharacter`

**責務**:
- 敵AI制御
- 攻撃パターン
- ダメージ反応
- 死亡時のドロップ処理

#### ACombatAIController

**ファイル**: `Source/AntigravityTest/Variant_Combat/AI/CombatAIController.h`

**責務**:
- State Tree制御
- Behavior Tree制御
- EQS（Environment Query System）連携
- 敵の意思決定

### AIシステムの実装

Combat バリアントは高度なAIシステムを実装しています：

#### State Tree

**ファイル**: `Content/Variant_Combat/Blueprints/AI/ST_CombatEnemy.uasset`

State Treeは敵AIの状態管理を行います：
- Idle（待機）
- Patrol（巡回）
- Chase（追跡）
- Attack（攻撃）
- Evade（回避）
- Flank（側面攻撃）

#### Environment Query System (EQS)

**EnvQueryContext_Player**: プレイヤー位置のコンテキスト
**EnvQueryContext_Danger**: 危険エリアのコンテキスト

**クエリ**:
- `EnvQuery_Evade.uasset` - 安全な退避位置を検索
- `EnvQuery_Flank.uasset` - 側面攻撃の最適位置を検索
- `EnvQuery_Fallback.uasset` - 退却位置を検索

### Combat固有の機能

#### インターフェースシステム

Combat バリアントは、柔軟な相互作用を実現するためにC++インターフェースを活用しています：

**ICombatAttacker** (`CombatAttacker.h`):
- 攻撃アニメーション中のイベント処理
- AnimNotifyとの連携

**ICombatDamageable** (`CombatDamageable.h`):
- ダメージ、回復、死亡の統一インターフェース
- 危険通知（攻撃が来ることをAIに伝える）

**ICombatActivatable** (`CombatActivatable.h`):
- 戦闘エリアの起動/非起動
- ボスバトルなどのトリガー管理

#### ゲームプレイ要素

**CombatActivationVolume**: 戦闘エリアへの侵入で敵をスポーン
**CombatCheckpointVolume**: 死亡時のリスポーン位置
**CombatDamageableBox**: 破壊可能なオブジェクト
**CombatLavaFloor**: 継続ダメージ床
**CombatDummy**: 攻撃練習用ダミー

### GameplayAbilities連携

Combat バリアントはGameplay Ability System (GAS) と統合可能に設計されています。現在は直接的な実装はありませんが、共通プロジェクトにGASモジュールが含まれており、将来的に攻撃やスキルをGameplayAbilityとして実装可能です。

**拡張可能性**:
- コンボ攻撃 → `UGameplayAbility_ComboAttack`
- チャージ攻撃 → `UGameplayAbility_ChargedAttack`
- 回避 → `UGameplayAbility_Evade`

---

## Variant_SideScrolling

### 目的と用途

**SideScrolling バリアント**は、2D横スクロールゲームのメカニクスを実装したバリアントです。物理パズル、プラットフォームギミック、NPCインタラクションに重点を置いています。

**主な特徴**:
- 固定カメラの2Dサイドスクロール
- 壁ジャンプ、二段ジャンプ
- Soft Platform（すり抜け床）
- 移動プラットフォーム
- ジャンプパッド
- NPCインタラクション
- Coyote Time

### ディレクトリ構造

```
Source/AntigravityTest/Variant_SideScrolling/
├── AI/
│   ├── SideScrollingAIController.h/.cpp      # AI制御
│   ├── SideScrollingNPC.h/.cpp               # NPCキャラクター
│   └── SideScrollingStateTreeUtility.h/.cpp  # State Tree ユーティリティ
│
├── Gameplay/
│   ├── SideScrollingJumpPad.h/.cpp           # ジャンプパッド
│   ├── SideScrollingMovingPlatform.h/.cpp    # 移動プラットフォーム
│   ├── SideScrollingPickup.h/.cpp            # アイテム取得
│   └── SideScrollingSoftPlatform.h/.cpp      # すり抜け床
│
├── Interfaces/
│   └── SideScrollingInteractable.h/.cpp      # インタラクション可能インターフェース
│
├── UI/
│   └── SideScrollingUI.h/.cpp                # UI表示
│
├── SideScrollingCharacter.h/.cpp             # プレイヤーキャラクター
├── SideScrollingGameMode.h/.cpp              # ゲームモード
├── SideScrollingPlayerController.h/.cpp      # プレイヤーコントローラー
└── SideScrollingCameraManager.h/.cpp         # カメラマネージャー

Content/Variant_SideScrolling/
├── Input/
│   ├── Actions/
│   │   ├── IA_Drop.uasset                   # すり抜け降下
│   │   ├── IA_Interact.uasset               # インタラクション
│   │   └── IA_Mover.uasset                  # 移動
│   └── IMC_SideScroller.uasset              # Input Mapping Context
│
├── Blueprints/
│   ├── AI/
│   │   ├── BP_SideScrollingNPC.uasset       # NPC Blueprint
│   │   ├── BP_SideScrollingAIController.uasset
│   │   └── ST_SideScrollingNPC.uasset       # State Tree
│   │
│   ├── Items/
│   │   ├── BP_SideScrollingPickup.uasset
│   │   ├── BP_SideScrollingSoftPlatform.uasset
│   │   └── BP_SideScrollingMovingPlatform.uasset
│   │
│   ├── BP_SideScrollingCharacter.uasset
│   ├── BP_SideScrollingGameMode.uasset
│   ├── BP_SideScrollingPlayerController.uasset
│   └── BP_SideScrollingCameraManager.uasset
│
├── Anims/
│   └── ABP_Manny_SideScroller.uasset        # Animation Blueprint
│
├── UI/
│   └── UI_SideScrolling.uasset              # UIウィジェット
│
└── Lvl_SideScrolling.umap                   # 横スクロールレベル
```

### 主要クラス

#### ASideScrollingCharacter

**ファイル**: `Source/AntigravityTest/Variant_SideScrolling/SideScrollingCharacter.h`

**継承関係**: `ACharacter` (UE標準)

**責務**:
- 2D横スクロール移動（Y軸固定）
- 壁ジャンプ、二段ジャンプ
- すり抜け床の制御
- インタラクション
- Coyote Time

**主要プロパティ**:
```cpp
// コンポーネント
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UCameraComponent* Camera;  // Spring Armなし、直接カメラ

// ジャンプ設定
UPROPERTY(EditAnywhere, Category="Side Scrolling|Jump")
float JumpPushImpulse = 600.0f;

// インタラクション設定
UPROPERTY(EditAnywhere, Category="Side Scrolling|Interaction")
float InteractionRadius = 200.0f;

// 壁ジャンプ設定
UPROPERTY(EditAnywhere, Category="Side Scrolling|Wall Jump")
float DelayBetweenWallJumps = 0.3f;
float WallJumpTraceDistance = 50.0f;
float WallJumpHorizontalImpulse = 500.0f;
float WallJumpVerticalMultiplier = 1.4f;

// すり抜け床設定
UPROPERTY(EditAnywhere, Category="Side Scrolling|Soft Platforms")
TEnumAsByte<ECollisionChannel> SoftCollisionObjectType;
float SoftCollisionTraceDistance = 1000.0f;

// Coyote Time設定
UPROPERTY(EditAnywhere, Category="Side Scrolling|Coyote Time")
float MaxCoyoteTime = 0.16f;
```

**主要メソッド**:
- `DoMove(float Forward)` - 横方向移動（1軸のみ）
- `DoDrop(float Value)` - すり抜け床降下
- `DoJumpStart() / DoJumpEnd()` - ジャンプ
- `DoInteract()` - インタラクション実行
- `MultiJump()` - 壁ジャンプ/二段ジャンプ判定
- `SetSoftCollision(bool bEnabled)` - すり抜け床の当たり判定切替
- `CheckForSoftCollision()` - すり抜け床の検出

#### ASideScrollingCameraManager

**ファイル**: `Source/AntigravityTest/Variant_SideScrolling/SideScrollingCameraManager.h`

**責務**:
- 固定カメラ角度の管理
- 2Dサイドビューの維持
- カメラのスムーズ追従

#### ASideScrollingNPC

**ファイル**: `Source/AntigravityTest/Variant_SideScrolling/AI/SideScrollingNPC.h`

**責務**:
- NPC AI制御
- State Tree連携
- インタラクション応答

### 2Dサイドスクロール固有の実装

#### 移動制限

ASideScrollingCharacterは、CharacterMovementComponentを使用しながら、Y軸（横軸）への移動のみを許可するように設計されています。

**実装手法**:
- カメラを固定角度（真横）に配置
- 移動入力をY軸のみに制限
- プレイヤーの回転を制限

#### すり抜け床システム

**SoftPlatform** は、プレイヤーが上から通過でき、下から着地できる特殊なプラットフォームです。

**実装**:
1. 専用のCollisionChannel (`SoftCollisionObjectType`) を使用
2. プレイヤーがDown+Jumpを入力すると、一時的にすり抜け床との衝突を無効化
3. 落下後、プレイヤーが床の上に戻ると再び衝突を有効化

#### インタラクションシステム

**ISideScrollingInteractable インターフェース**:
```cpp
class ISideScrollingInteractable
{
public:
    virtual void Interaction(AActor* Interactor) = 0;
};
```

このインターフェースを実装したActorは、プレイヤーが近づいてInteractボタンを押すと反応します。

**実装例**:
- Pickup（アイテム取得）
- NPC（会話）
- スイッチ（ドア開閉）

#### 物理ギミック

**SideScrollingJumpPad**: プレイヤーに垂直/水平の推進力を与える
**SideScrollingMovingPlatform**: 定期的に移動するプラットフォーム
**SideScrollingSoftPlatform**: すり抜け可能な床

---

## モジュール構成

### AntigravityTest.Build.cs の解説

**ファイル**: `Source/AntigravityTest/AntigravityTest.Build.cs`

このファイルは、UnrealBuildTool (UBT) がプロジェクトをコンパイルする際の設定を定義します。

```csharp
public class AntigravityTest : ModuleRules
{
    public AntigravityTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 依存モジュールの指定
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "StateTreeModule",
            "GameplayStateTreeModule",
            "UMG",
            "Slate",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "SlateCore"
        });

        // 公開インクルードパスの指定
        PublicIncludePaths.AddRange(new string[] {
            "AntigravityTest",
            "AntigravityTest/Variant_Platforming",
            "AntigravityTest/Variant_Platforming/Animation",
            "AntigravityTest/Variant_Combat",
            "AntigravityTest/Variant_Combat/AI",
            "AntigravityTest/Variant_Combat/Animation",
            "AntigravityTest/Variant_Combat/Gameplay",
            "AntigravityTest/Variant_Combat/Interfaces",
            "AntigravityTest/Variant_Combat/UI",
            "AntigravityTest/Variant_SideScrolling",
            "AntigravityTest/Variant_SideScrolling/AI",
            "AntigravityTest/Variant_SideScrolling/Gameplay",
            "AntigravityTest/Variant_SideScrolling/Interfaces",
            "AntigravityTest/Variant_SideScrolling/UI"
        });
    }
}
```

### PublicIncludePathsの役割

`PublicIncludePaths` は、このモジュールを使用する際にインクルードパスとして自動的に追加されるディレクトリを指定します。

**重要性**:
1. **簡潔なインクルード**: 長いパスを書かなくて済む
   ```cpp
   // PublicIncludePathsありの場合
   #include "CombatCharacter.h"

   // PublicIncludePathsなしの場合
   #include "AntigravityTest/Variant_Combat/CombatCharacter.h"
   ```

2. **バリアント間のクロスリファレンス**: 異なるバリアントのクラスを参照可能
   ```cpp
   // Variant_SideScrollingから Variant_Combatのインターフェースを利用
   #include "CombatDamageable.h"
   ```

3. **Blueprintとの統合**: Blueprintがc++クラスを認識するために必要

### 各バリアントへのアクセス方法

#### 同一バリアント内のクラスをインクルード

```cpp
// Variant_Combat/CombatCharacter.cpp
#include "CombatCharacter.h"
#include "CombatEnemy.h"         // 同じバリアント内
#include "CombatAttacker.h"      // Interfacesフォルダ
```

#### 別バリアントのクラスをインクルード

```cpp
// Variant_SideScrolling/SideScrollingCharacter.cpp
#include "SideScrollingCharacter.h"
#include "CombatDamageable.h"    // Variant_Combatのインターフェース利用可能
```

#### 共通クラスをインクルード

```cpp
// 任意のバリアント
#include "AntigravityGoalGameMode.h"  // 共通基底クラス
#include "InventoryComponent.h"       // 共通コンポーネント
```

---

## バリアント間の連携

### 共通インターフェースの使い方

各バリアントは、特定の機能を共有するためにC++インターフェースを活用しています。

**例: ダメージシステムの共有**

`ICombatDamageable` インターフェースは、Variant_Combat で定義されていますが、他のバリアントでも使用可能です。

```cpp
// Variant_Combat/Interfaces/CombatDamageable.h
class ICombatDamageable
{
public:
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
                            const FVector& DamageLocation,
                            const FVector& DamageImpulse) = 0;
    virtual void HandleDeath() = 0;
    virtual void ApplyHealing(float Healing, AActor* Healer) = 0;
    virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) = 0;
};
```

**Variant_SideScrolling でダメージシステムを実装する場合**:
```cpp
// Variant_SideScrolling/SideScrollingCharacter.h
#include "CombatDamageable.h"  // Variant_Combatのインターフェース

class ASideScrollingCharacter : public ACharacter, public ICombatDamageable
{
    GENERATED_BODY()

public:
    // ICombatDamageable インターフェースの実装
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
                            const FVector& DamageLocation,
                            const FVector& DamageImpulse) override;
    virtual void HandleDeath() override;
    virtual void ApplyHealing(float Healing, AActor* Healer) override;
    virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) override;
};
```

### コードの再利用パターン

#### パターン1: 共通基底クラスの継承

全バリアントのGameModeは `AAntigravityGoalGameMode` を継承しています。

```cpp
// 共通基底
class AAntigravityGoalGameMode : public AAntigravityTestGameMode
{
    UFUNCTION(BlueprintCallable, Category = "Game")
    void CompleteStage();
};

// 各バリアント
class APlatformingGameMode : public AAntigravityGoalGameMode { };
class ACombatGameMode : public AAntigravityGoalGameMode { };
class ASideScrollingGameMode : public AAntigravityGoalGameMode { };
```

#### パターン2: インターフェースの活用

インターフェースを使用して、実装の詳細を隠蔽しながら機能を共有します。

```cpp
// Variant_SideScrolling/Interfaces/SideScrollingInteractable.h
class ISideScrollingInteractable
{
public:
    virtual void Interaction(AActor* Interactor) = 0;
};

// 複数のActorが実装可能
class ASideScrollingPickup : public AActor, public ISideScrollingInteractable { };
class ASideScrollingNPC : public ACharacter, public ISideScrollingInteractable { };
```

#### パターン3: コンポーネントの共有

`UInventoryComponent` は全バリアントで使用可能な共通コンポーネントです。

```cpp
// Source/AntigravityTest/Component/InventoryComponent.h
UCLASS()
class UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
    // インベントリロジック
};

// どのバリアントでも利用可能
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UInventoryComponent* Inventory;
```

### バリアント切り替えの方法

#### 方法1: エディタでレベルを開く

1. Unreal Editorを起動
2. `Content/Variant_Platforming/Lvl_Platforming.umap` を開く
3. Playボタンを押す

#### 方法2: プロジェクト設定でデフォルトマップを変更

`Config/DefaultEngine.ini` を編集:

```ini
[/Script/EngineSettings.GameMapsSettings]
EditorStartupMap=/Game/Variant_Combat/Lvl_Combat.Lvl_Combat
GameDefaultMap=/Game/Variant_Combat/Lvl_Combat.Lvl_Combat
```

#### 方法3: コマンドライン引数

```bash
UnrealEditor.exe "C:/Path/To/AntigravityTest.uproject" /Game/Variant_SideScrolling/Lvl_SideScrolling
```

---

## 新機能実装時のガイドライン

### どのバリアントに実装すべきか判断基準

以下の質問に答えて、最適なバリアントを選択してください：

| 質問 | Yes → | No → |
|------|-------|------|
| この機能は近接戦闘やAI戦闘に関連しますか？ | **Variant_Combat** | 次へ |
| この機能は3Dプラットフォーム移動（ダッシュ、壁ジャンプ）に関連しますか？ | **Variant_Platforming** | 次へ |
| この機能は2D横スクロールやインタラクションに関連しますか？ | **Variant_SideScrolling** | 次へ |
| この機能はすべてのバリアントで使用されますか？ | **共通部分** | バリアントを選択 |

### 複数バリアントで共有する機能の実装方法

#### ステップ1: インターフェースを定義

共有したい機能をインターフェースとして定義します。

**推奨配置先**:
- すでにあるバリアントのInterfacesフォルダ（例: `Variant_Combat/Interfaces/`）
- または新しく共通インターフェースフォルダを作成（例: `Source/AntigravityTest/Interfaces/`）

**例: テレポート機能を共有する場合**

```cpp
// Source/AntigravityTest/Interfaces/Teleportable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Teleportable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UTeleportable : public UInterface
{
    GENERATED_BODY()
};

class ITeleportable
{
    GENERATED_BODY()

public:
    virtual void TeleportTo(const FVector& Location) = 0;
};
```

#### ステップ2: Build.csにインクルードパスを追加

```csharp
PublicIncludePaths.AddRange(new string[] {
    "AntigravityTest/Interfaces"  // 追加
});
```

#### ステップ3: 各バリアントで実装

```cpp
// Variant_Platforming/PlatformingCharacter.h
#include "Teleportable.h"

class APlatformingCharacter : public ACharacter, public ITeleportable
{
    virtual void TeleportTo(const FVector& Location) override;
};

// Variant_Combat/CombatCharacter.h
#include "Teleportable.h"

class ACombatCharacter : public ACharacter, public ITeleportable
{
    virtual void TeleportTo(const FVector& Location) override;
};
```

### バリアント固有の機能の実装方法

バリアント固有の機能は、そのバリアントのディレクトリ内に実装します。

**例: Variant_Combatに必殺技を追加**

#### ステップ1: クラスを作成

```cpp
// Variant_Combat/Gameplay/CombatUltimateAttack.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatUltimateAttack.generated.h"

UCLASS()
class ACombatUltimateAttack : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Combat")
    void ExecuteUltimate(AActor* Target);
};
```

#### ステップ2: Build.csにパスを追加（必要に応じて）

すでに `Variant_Combat/Gameplay` はPublicIncludePathsに含まれているため、追加不要です。

#### ステップ3: CombatCharacterに統合

```cpp
// Variant_Combat/CombatCharacter.h
#include "CombatUltimateAttack.h"

class ACombatCharacter : public ACharacter
{
    UPROPERTY(EditAnywhere, Category="Combat|Ultimate")
    TSubclassOf<ACombatUltimateAttack> UltimateAttackClass;

    UFUNCTION(BlueprintCallable, Category="Combat")
    void DoUltimateAttack();
};
```

---

## 使用システムとの統合

### Enhanced Input System

すべてのバリアントは Enhanced Input System を使用しています。

**基本構造**:
1. **Input Actions** (`IA_*`): 抽象的な入力（ジャンプ、移動等）
2. **Input Mapping Context** (`IMC_*`): 入力デバイスとInput Actionのマッピング
3. **C++/Blueprint**: Input Actionのイベントハンドラ

**実装例**:

```cpp
// PlatformingCharacter.h
UPROPERTY(EditAnywhere, Category="Input")
UInputAction* JumpAction;

UPROPERTY(EditAnywhere, Category="Input")
UInputAction* MoveAction;

// PlatformingCharacter.cpp
void APlatformingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlatformingCharacter::DoJumpStart);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformingCharacter::Move);
    }
}
```

**バリアント別Input Mapping Context**:
- `Content/Variant_Platforming/Input/IMC_Platforming.uasset`
- `Content/Variant_Combat/Input/IMC_Combat.uasset`
- `Content/Variant_SideScrolling/Input/IMC_SideScroller.uasset`

### Gameplay Ability System (GAS)

プロジェクトにはGASモジュールが含まれていますが、現時点では限定的な使用にとどまっています。

**現在の実装**:
- `GA_Jump` - ジャンプアビリティ
- `GA_JumpWait` - ジャンプ待機アビリティ

**将来的な拡張**:
- Variant_Combatの攻撃をGameplayAbilityとして実装
- Variant_PlatformingのダッシュをGameplayAbilityとして実装
- バフ/デバフシステムの追加

**GASを新しいバリアントに統合する手順**:

1. **AbilitySystemComponentを追加**:
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
UAbilitySystemComponent* AbilitySystemComponent;
```

2. **GameplayAbilityを作成**:
```cpp
UCLASS()
class UGA_Dash : public UGameplayAbility
{
    GENERATED_BODY()

    virtual void ActivateAbility(...) override;
};
```

3. **アビリティを付与**:
```cpp
AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UGA_Dash::StaticClass(), 1, INDEX_NONE, this));
```

### State Tree

State TreeはVariant_CombatとVariant_SideScrollingのAI制御に使用されています。

**Variant_Combat**:
- `Content/Variant_Combat/Blueprints/AI/ST_CombatEnemy.uasset`

**Variant_SideScrolling**:
- `Content/Variant_SideScrolling/Blueprints/AI/ST_SideScrollingNPC.uasset`

**State Treeの利点**:
- ビジュアルベースの状態管理
- Behavior Treeよりも軽量
- GameplayStateTreeModuleとの統合

**新しいState Treeの作成手順**:

1. Content Browserで右クリック → AI → State Tree
2. State Treeを開き、状態を追加
3. AIControllerでState Treeを実行:

```cpp
UPROPERTY(EditAnywhere, Category="AI")
UStateTree* StateTreeAsset;

// BeginPlayで実行
void ACombatAIController::BeginPlay()
{
    Super::BeginPlay();
    RunStateTree(StateTreeAsset);
}
```

### AI Module (Behavior Tree, EQS)

#### Behavior Tree

現在のプロジェクトでは主にState Treeが使用されていますが、Behavior Treeも利用可能です。

#### Environment Query System (EQS)

Variant_Combatで使用されています。

**実装例**:

```cpp
// EnvQueryContext_Player.h
UCLASS()
class UEnvQueryContext_Player : public UEnvQueryContext
{
    GENERATED_BODY()

    virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
                                FEnvQueryContextData& ContextData) const override;
};
```

**使用方法**:
1. Content Browserで右クリック → AI → Environment Query
2. Generatorを追加（例: Points: Grid）
3. Testsを追加（例: Distance to Player）
4. Contextを設定（例: `EnvQueryContext_Player`）

### プラグインとの連携

プロジェクトには以下のプラグインが含まれています：

#### ScreenTransitionSystem

**場所**: `Plugins/ScreenTransitionSystem/`

**機能**: UI画面遷移、非同期ロード、トランジションエフェクト

**バリアントとの統合例**:
```cpp
#include "ScreenTransitionSubsystem.h"

void ACombatGameMode::LoadNextLevel()
{
    UScreenTransitionSubsystem* TransitionSystem = GetGameInstance()->GetSubsystem<UScreenTransitionSubsystem>();
    TransitionSystem->TransitionToLevel("/Game/Variant_Combat/Lvl_Combat_Stage2");
}
```

#### MermaidMaterial

**場所**: `Plugins/MermaidMaterial/`

**機能**: Pythonスクリプトによるマテリアル生成

**使用方法**: ドキュメント `Plugins/MermaidMaterial/Documentation/` を参照

---

## まとめ

AntigravityTestプロジェクトは、3つの独立したゲームプレイバリアントを単一のUE5プロジェクト内に統合した実験的な構造を採用しています。

**バリアント選択の指針**:
- **Platforming**: 3Dプラットフォーマー、高速移動
- **Combat**: 近接戦闘、AI戦闘、ダメージシステム
- **SideScrolling**: 2D横スクロール、物理パズル、インタラクション

**共有機能の実装**:
- インターフェースを使用
- 共通基底クラスを継承
- コンポーネントを活用

**新機能追加時のチェックポイント**:
1. どのバリアントに属するか判断
2. 適切なディレクトリに配置
3. `Build.cs` にパスを追加（必要に応じて）
4. インターフェースで抽象化（複数バリアントで使用する場合）
5. Enhanced Input、GAS、State Tree等のシステムと統合

このアーキテクチャにより、柔軟な実験とプロトタイピングが可能になっています。
