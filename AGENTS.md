# AGENTS.md - Claude Code 活用ガイド

このドキュメントは、Claude Code に効果的に依頼するためのガイドラインです。

## 目次

1. [依頼方法テンプレート](#依頼方法テンプレート)
   - [新機能実装](#新機能実装)
   - [プラグイン開発](#プラグイン開発)
   - [デバッグ・修正](#デバッグ修正)
2. [ワークフロー自動化](#ワークフロー自動化)
3. [カスタムコマンド](#カスタムコマンド)
4. [FAQ・チェックリスト](#faqチェックリスト)

---

## 依頼方法テンプレート

### 新機能実装

#### 基本テンプレート

```
[機能名]を実装してください。

【要件】
- [要件1]
- [要件2]
- [要件3]

【実装場所】
- C++クラス: Source/AntigravityTest/[カテゴリ]/
- Blueprint: Content/[カテゴリ]/Blueprints/

【参考にする既存コード】
- [既存クラス名や機能名]

【実装後の確認】
- [ ] Build.csに必要なモジュールを追加
- [ ] BlueprintCallableを設定
- [ ] コンパイルエラーがないこと
- [ ] エディタで動作確認
```

#### 良い依頼例1: キャラクター機能追加

```
キャラクターにダッシュ機能を追加してください。

【要件】
- Shift+Wでダッシュ（移動速度2倍、スタミナ消費）
- スタミナが0になるとダッシュ不可
- ダッシュ中は専用アニメーションを再生
- Enhanced Input Systemを使用

【実装場所】
- C++クラス: Source/AntigravityTest/Variant_Combat/Gameplay/CombatCharacter.h/cpp
- Input Action: Content/Variant_Combat/Input/IA_Dash.uasset

【参考にする既存コード】
- CombatCharacter::Move() の実装
- 既存のInput Actionの設定方法

【実装後の確認】
- [ ] Build.csに必要なモジュールを追加（EnhancedInput）
- [ ] DoDash()をBlueprintCallableに設定
- [ ] Input Actionを作成してマッピング
- [ ] コンパイル確認
- [ ] エディタで動作確認
```

**Claudeの実装フロー:**
1. CombatCharacter.hにDoDash()メソッド追加
2. Stamina管理用のUPROPERTYを追加
3. Enhanced Input用のIA_Dashアクションをバインド
4. DoDash()の実装（移動速度変更、スタミナ消費）
5. BlueprintImplementableEventでアニメーション制御を委譲

#### 良い依頼例2: UI実装

```
体力バーとスタミナバーを表示するUIウィジェットを作成してください。

【要件】
- 体力バー: 緑色のプログレスバー、現在HP/最大HPを表示
- スタミナバー: 青色のプログレスバー、現在値/最大値を表示
- キャラクターの頭上に表示（World Space Widget）
- ダメージを受けると体力バーが赤く点滅

【実装場所】
- C++クラス: Source/AntigravityTest/UI/CharacterStatusWidget.h/cpp
- Widget Blueprint: Content/Variant_Combat/UI/WBP_CharacterStatus.uasset

【参考にする既存コード】
- UCombatLifeBar（既存の体力バー実装）

【実装後の確認】
- [ ] Build.csにUMGモジュールを追加（既存）
- [ ] UpdateHealthBar()とUpdateStaminaBar()をBlueprintCallableに設定
- [ ] Widget Blueprintで具体的なデザインを実装
- [ ] コンパイル確認
- [ ] エディタで表示確認
```

**Claudeの実装フロー:**
1. UUserWidgetを継承したCharacterStatusWidgetクラスを作成
2. Health/Stamina用のUPROPERTYを追加
3. UpdateHealthBar()とUpdateStaminaBar()をBlueprintNativeEventで実装
4. Blueprint側でアニメーション（点滅）を実装

---

### プラグイン開発

#### 新規プラグイン作成テンプレート

```
[プラグイン名]プラグインを作成してください。

【概要】
[プラグインの目的と機能の説明]

【機能要件】
- [機能1]
- [機能2]
- [機能3]

【参考にする既存プラグイン】
- [既存プラグイン名]（類似機能）

【モジュール構成】
- Runtime: [機能説明]
- Editor (必要な場合): [機能説明]

【実装後の確認】
- [ ] .upluginファイルを作成
- [ ] Build.csを作成
- [ ] モジュールの依存関係を設定
- [ ] サンプルコードまたはテストコードを追加
- [ ] プラグインがエディタで認識されることを確認
```

#### 良い依頼例: QuestSystemプラグイン

```
QuestSystemプラグインを作成してください。

【概要】
ゲーム内のクエスト管理システム。クエストの作成、進行状況の追跡、報酬の付与を行う。

【機能要件】
- クエストデータの定義（タイトル、説明、目標、報酬）
- クエストの進行状況管理（未受注、進行中、完了、失敗）
- 目標の達成確認（複数目標、順序指定、並行可能）
- クエスト報酬の付与（経験値、アイテム、通貨）
- Blueprintから簡単に利用可能

【参考にする既存プラグイン】
- ScreenTransitionSystem（プラグイン構造とAPI設計）

【モジュール構成】
- Runtime: クエストロジック、データ管理
- Editor: クエストエディタ、デバッグツール

【実装後の確認】
- [ ] .upluginファイルを作成（QuestSystem.uplugin）
- [ ] Build.csを作成（依存: Engine, CoreUObject, UMG）
- [ ] UQuestクラスとUQuestManagerクラスを実装
- [ ] BlueprintCallable関数を実装
- [ ] サンプルクエストBlueprintを作成
- [ ] エディタでプラグインが有効化されることを確認
```

**Claudeの実装フロー:**
1. プラグインディレクトリ構造を作成
2. .upluginファイルを生成（メタデータ、モジュール定義）
3. Build.csを作成（依存関係設定）
4. Runtimeモジュール: UQuest, UQuestManager, UQuestObjectiveクラスを実装
5. Editorモジュール: クエストエディタのUI（可能な範囲で）
6. ドキュメント作成（README.md）

---

### デバッグ・修正

#### バグ修正テンプレート

```
[バグの簡潔な説明]を修正してください。

【現象】
- [何が起きているか]
- [再現手順]

【期待される動作】
- [正しい動作の説明]

【エラーメッセージ・ログ】
```
[エラーメッセージやログをここに貼り付け]
```

【疑わしいコード箇所】
- [ファイル名:行番号] または [クラス名::メソッド名]

【実装後の確認】
- [ ] バグが修正されたことを確認
- [ ] 修正が他の機能に影響しないことを確認
- [ ] コンパイルエラーがないこと
```

#### 良い依頼例: クラッシュ修正

```
CombatCharacter::ApplyDamage()でクラッシュする問題を修正してください。

【現象】
- 敵からダメージを受けた際にクラッシュ
- 再現手順:
  1. ゲームを開始
  2. 敵に近づく
  3. 敵の攻撃を受ける
  4. クラッシュ

【期待される動作】
- ダメージを受けて体力が減少する
- 体力バーが更新される
- クラッシュしない

【エラーメッセージ・ログ】
```
Assertion failed: LifeBarWidget != nullptr
File: CombatCharacter.cpp
Line: 245
```

【疑わしいコード箇所】
- CombatCharacter.cpp:245
- ApplyDamage()内でLifeBarWidgetを使用している箇所

【実装後の確認】
- [ ] LifeBarWidgetがnullptrの場合のチェックを追加
- [ ] クラッシュが修正されたことを確認
- [ ] 他のキャラクターでも動作することを確認
- [ ] コンパイル確認
```

**Claudeの実装フロー:**
1. CombatCharacter.cppを読む
2. ApplyDamage()内のLifeBarWidget使用箇所を確認
3. nullptrチェックを追加
4. エラーハンドリングを追加（警告ログ等）
5. 修正内容を説明

#### リファクタリングテンプレート

```
[リファクタリング対象]をリファクタリングしてください。

【目的】
- [なぜリファクタリングが必要か]

【リファクタリング内容】
- [変更内容1]
- [変更内容2]

【制約】
- [変更してはいけない部分]
- [互換性を保つべき部分]

【実装後の確認】
- [ ] 既存の機能が動作すること
- [ ] Blueprintからの呼び出しが動作すること
- [ ] コンパイルエラーがないこと
```

#### 良い依頼例: コードの整理

```
CombatCharacterクラスの攻撃関連メソッドをリファクタリングしてください。

【目的】
- 攻撃ロジックが複雑化しており、可読性が低い
- コンボ攻撃とチャージ攻撃のロジックを分離したい

【リファクタリング内容】
- DoAttack()を分割:
  - DoComboAttack(): コンボ攻撃専用
  - DoChargeAttack(): チャージ攻撃専用
- 攻撃状態管理をenum classに変更（int→EAttackState）
- 攻撃判定ロジックを別メソッドに抽出（CheckAttackHit()）

【制約】
- Blueprint側の実装に影響を与えない（既存のBlueprintイベントは維持）
- 既存のDoAttack()は残す（互換性のためDeprecated扱い）

【実装後の確認】
- [ ] 既存のBlueprintが動作すること
- [ ] 新しいメソッドがBlueprintから呼び出せること
- [ ] コンパイル確認
- [ ] 攻撃機能が正常に動作すること
```

**Claudeの実装フロー:**
1. CombatCharacter.h/cppを読む
2. 攻撃ロジックを分析
3. EAttackState enumを定義
4. DoComboAttack()とDoChargeAttack()を実装
5. CheckAttackHit()を抽出
6. 既存のDoAttack()を残し、内部で新メソッドを呼び出す
7. DEPRECATED マクロで警告を表示

---

## ワークフロー自動化

現在、このプロジェクトでは`.agent/workflows/`ディレクトリを使用していません。
将来的に以下のようなワークフローが追加される可能性があります:

### 想定される将来のワークフロー

#### build_and_launch_ue5.md
UE5プロジェクトのビルドとエディタ起動を自動化

**使用例:**
```
「ビルドしてエディタを起動して」
```

**実行内容:**
1. Build.batを実行してコンパイル
2. コンパイルエラーがあれば修正を試みる
3. エディタを起動（LaunchEditor.bat）

#### create_gameplay_ability.md
Gameplay Ability Systemの新しいアビリティを作成

**使用例:**
```
「FireballAbilityを作成して」
```

**実行内容:**
1. UGameplayAbilityを継承したクラスを作成
2. ActivateAbility()とEndAbility()を実装
3. Blueprint用の設定をセットアップ
4. サンプルBlueprintを生成

#### create_ai_decorator.md
Behavior Treeの新しいDecoratorを作成

**使用例:**
```
「IsPlayerInRangeDecoratorを作成して」
```

**実行内容:**
1. UBTDecorator_*クラスを作成
2. CalculateRawConditionValue()を実装
3. エディタ用のプロパティを設定
4. テスト用のBehavior Treeを生成

#### create_3d_game_cycle.md
完全なゲームサイクルの実装（GameMode、Character、UI等）

**使用例:**
```
「新しいゲームモード"Survival"を作成して」
```

**実行内容:**
1. GameModeクラスを作成
2. プレイヤーCharacterクラスを作成
3. 基本的なUIを作成
4. Blueprintで具体的な実装を作成

#### check_gameplay_abilities.md
Gameplay Ability Systemの設定を確認・修正

**使用例:**
```
「GASの設定を確認して」
```

**実行内容:**
1. Build.csにGameplayAbilitiesモジュールが含まれているか確認
2. Ability System Componentが正しく設定されているか確認
3. 問題があれば修正提案

#### ensure_property_access.md
UPROPERTYのアクセス設定を確認・修正

**使用例:**
```
「CombatCharacterのプロパティアクセスを確認して」
```

**実行内容:**
1. UPROPERTYマクロが適切に設定されているか確認
2. BlueprintCallable/BlueprintReadWrite等のマクロをチェック
3. カテゴリ設定を確認
4. 問題があれば修正提案

---

## カスタムコマンド

### /worktree-feature

Gitのworktree機能を使用して、新しいブランチを作成し、作業ディレクトリを切り替えます。

**使用例:**
```
「/worktree-feature new-quest-system で新しいworktreeを作成して」
```

**実行内容:**
```bash
git worktree add ../AntigravityTest-new-quest-system -b feature/new-quest-system
cd ../AntigravityTest-new-quest-system
```

**メリット:**
- 複数のブランチを並行して作業できる
- ブランチ切り替えによるコンパイル待ち時間を削減
- mainブランチを常にクリーンな状態に保つ

### /link-plugin

プラグインをUE5プロジェクトにシンボリックリンクで接続します。

**使用例:**
```
「ScreenTransitionSystemをC:/MyUE5Projectにリンクして」
```

**実行内容:**
```bash
./scripts/quick-link.sh ScreenTransitionSystem C:/MyUE5Project
```

**メリット:**
- Worktreeでの変更が即座にUE5プロジェクトに反映
- ファイルをコピーする必要がなく、一箇所で管理
- 複数のプロジェクトで同じプラグインをテスト可能

**詳細:** `scripts/README.md` を参照

---

## FAQ・チェックリスト

### よくある質問（FAQ）

#### Q1: Blueprint用のクラスを作成したい

**依頼例:**
```
「CombatCharacterを継承したBoss用のクラスを作成して。Blueprint側で具体的な実装をします」
```

**Claudeの実装:**
```cpp
UCLASS(abstract, Blueprintable)
class ANTIGRAVITYTEST_API ABossCharacter : public ACombatCharacter
{
    GENERATED_BODY()

public:
    // Blueprintで実装可能なイベント
    UFUNCTION(BlueprintImplementableEvent, Category="Boss")
    void BP_OnPhaseChange(int32 NewPhase);

    // C++でデフォルト実装、Blueprintでオーバーライド可能
    UFUNCTION(BlueprintNativeEvent, Category="Boss")
    void OnBossDefeat();
};
```

**ポイント:**
- `abstract`: C++側ではインスタンス化不可、Blueprint側で具体化
- `Blueprintable`: Blueprintで派生可能
- `BlueprintImplementableEvent`: Blueprint専用イベント
- `BlueprintNativeEvent`: C++デフォルト実装 + Blueprintオーバーライド可能

#### Q2: コードスタイルを統一したい

**依頼例:**
```
「Source/AntigravityTest/以下の全てのヘッダーファイルで、UPROPERTY()にTObjectPtr<>を使用するように修正して」
```

**Claudeの実装フロー:**
1. `**/*.h`でヘッダーファイルを検索
2. `UPROPERTY()`を含む行をGrep
3. `U*` や `A*` ポインタを検出
4. `TObjectPtr<>`に置き換え
5. コンパイル確認

**注意:**
- 大規模な変更なので、バックアップを推奨
- コンパイルエラーが出る場合は個別に対応

#### Q3: テストコードを作成したい

**依頼例:**
```
「CombatCharacter::ApplyDamage()のユニットテストを作成して」
```

**Claudeの実装:**
```cpp
// Source/AntigravityTest/Tests/CombatCharacterTest.cpp
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Variant_Combat/Gameplay/CombatCharacter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatCharacterDamageTest,
    "AntigravityTest.Combat.Character.Damage",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCombatCharacterDamageTest::RunTest(const FString& Parameters)
{
    // テストロジック
    // ...
    return true;
}

#endif // WITH_AUTOMATION_TESTS
```

**実行:**
- エディタのSession Frontendから実行
- コマンドライン: `-ExecCmds="Automation RunTests AntigravityTest"`

#### Q4: 既存のプラグインを参考に新しいプラグインを作成したい

**依頼例:**
```
「ScreenTransitionSystemを参考に、新しいDialogSystemプラグインを作成して。構造は似た感じで、ダイアログ管理機能を実装してください」
```

**Claudeの実装フロー:**
1. ScreenTransitionSystemの構造を分析
   - .upluginファイルのメタデータ
   - Build.csのモジュール設定
   - クラス構成（Manager, Widget, Data等）
2. 同様の構造でDialogSystemを作成
3. ダイアログ固有の機能を実装
4. ドキュメント作成

**ポイント:**
- 既存プラグインの設計パターンを踏襲
- 一貫性のあるAPI設計

#### Q5: EnhancedInput用のInput Actionを追加したい

**依頼例:**
```
「Dashアクションを追加して、CombatCharacterにバインドして」
```

**Claudeの実装フロー:**
1. Content/Variant_Combat/Input/にIA_Dash.uassetを作成（Blueprint指示）
2. CombatCharacter.hにUInputAction* DashActionを追加
3. SetupPlayerInputComponentでバインド
4. DoDash()メソッドを実装
5. Input Mapping Contextに追加する手順を説明

**Note:** Claudeはアセットファイル（.uasset）を直接作成できないため、エディタでの手順を説明します。

---

### 実装後の確認事項チェックリスト

#### C++実装時

- [ ] **Build.cs更新**
  - 必要なモジュールを`PublicDependencyModuleNames`または`PrivateDependencyModuleNames`に追加
  - 例: `UMG`, `AIModule`, `GameplayAbilities`

- [ ] **API macro設定**
  - 公開クラスに`ANTIGRAVITYTEST_API`マクロを追加
  - 例: `class ANTIGRAVITYTEST_API ACombatCharacter : public ACharacter`

- [ ] **BlueprintCallableカテゴリ**
  - `UFUNCTION(BlueprintCallable, Category="...")`を設定
  - カテゴリ名は機能ごとに統一（例: "Combat", "Movement", "UI"）

- [ ] **UPROPERTYメタデータ**
  - `EditAnywhere`/`VisibleAnywhere`を適切に設定
  - `BlueprintReadWrite`/`BlueprintReadOnly`を設定
  - `Category`を設定
  - 必要に応じてmeta指定:
    - `ClampMin`/`ClampMax`: 数値範囲制限
    - `Units`: 単位表示
    - `AllowPrivateAccess`: プライベート変数へのBlueprint アクセス

- [ ] **TObjectPtr使用**
  - UObject派生クラスのポインタは`TObjectPtr<T>`を使用
  - 例: `TObjectPtr<USpringArmComponent> CameraBoom;`

- [ ] **GENERATED_BODY()マクロ**
  - 全てのUCLASS/USTRUCTに`GENERATED_BODY()`が含まれていること

- [ ] **コンパイル確認**
  - `Build.bat`を実行してエラーがないこと
  - 警告も可能な限り修正

#### Blueprint実装時

- [ ] **命名規約**
  - Blueprint クラス: `BP_`
  - Widget Blueprint: `WBP_`
  - Material: `M_`
  - Input Action: `IA_`

- [ ] **親クラス設定**
  - C++クラスを親として選択
  - 適切な基底クラスから派生

- [ ] **カテゴリ整理**
  - 変数や関数をカテゴリごとに整理
  - C++のCategoryと一致させる

#### プラグイン実装時

- [ ] **.upluginファイル**
  - メタデータ（Name, Version, Description）を設定
  - モジュール定義を追加
  - 依存プラグインを指定

- [ ] **Build.cs**
  - 依存モジュールを全て記載
  - PublicDependencyModuleNames vs PrivateDependencyModuleNamesを適切に使い分け

- [ ] **ドキュメント**
  - README.mdを作成
  - 使用方法、API仕様、サンプルコードを記載

- [ ] **エディタで認識**
  - プラグインがエディタのプラグインリストに表示されること
  - 有効化/無効化が正しく動作すること

#### 最終確認

- [ ] **エディタで動作確認**
  - 実装した機能が正しく動作すること
  - Blueprintから呼び出しが可能なこと
  - クラッシュやエラーが発生しないこと

- [ ] **コードレビュー**
  - 命名規約に従っていること
  - コメントが適切に記載されていること
  - 不要なコードが残っていないこと

- [ ] **Git commit**
  - 適切なコミットメッセージ
  - 関連ファイルを全て含む

---

## まとめ

このガイドを参考に、Claude Codeに効果的に依頼してください。

**依頼のポイント:**
1. **明確な要件**: 何を実装したいか具体的に
2. **実装場所の指定**: どこに実装するか明示
3. **参考コードの提示**: 既存コードを参考にする
4. **確認事項**: 実装後に何を確認するか明記

**Claudeの強み:**
- コーディング規約を理解している（CLAUDE.mdを参照）
- 既存コードを分析して一貫性のある実装を提供
- Blueprint連携を考慮した設計
- ドキュメント生成

**Claudeの制限:**
- アセットファイル（.uasset）は直接作成できない → 手順を説明
- エディタ操作は実行できない → 操作方法を説明
- コンパイルは実行可能だが、エディタでの動作確認は不可 → 確認手順を提示

効率的に開発を進めるために、このガイドを活用してください。
