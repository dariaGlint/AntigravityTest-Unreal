# AntigravityTest - Unreal Engine 5 プロジェクト

このドキュメントは、Claude Code がこのプロジェクトで効率的に作業するための情報を提供します。

## プロジェクト概要

### 基本情報
- **Unreal Engine バージョン**: 5.7
- **プロジェクトタイプ**: C++ ゲームプロジェクト
- **プロジェクト名**: AntigravityTest
- **モジュールタイプ**: Runtime
- **主な依存関係**: Engine, AIModule, UMG

### プロジェクトの特徴
このプロジェクトは、複数のゲームプレイバリアント（Combat、SideScrolling、Platforming）を含む実験的なUE5プロジェクトです。戦闘システム、AI、UI、入力処理などの機能を実装しています。

## ディレクトリ構造

```
AntigravityTest/
├── Source/                        # C++ソースコード
│   └── AntigravityTest/
│       ├── AI/                    # AI関連のコード
│       ├── Component/             # 再利用可能なコンポーネント
│       ├── Items/                 # アイテムシステム
│       ├── Tests/                 # テストコード
│       ├── UI/                    # UIウィジェット
│       ├── Variant_Combat/        # 戦闘バリアント
│       │   ├── AI/               # 戦闘AI（Enemy, AIController等）
│       │   └── Gameplay/         # 戦闘ゲームプレイ要素
│       ├── Variant_SideScrolling/ # 横スクロールバリアント
│       └── Variant_Platforming/   # プラットフォーマーバリアント
│
├── Content/                       # Blueprintとアセット
│   ├── Input/                    # 入力設定
│   │   ├── Actions/              # Input Actions
│   │   └── Touch/                # タッチ入力
│   ├── ThirdPerson/              # サードパーソン関連
│   ├── Variant_Combat/           # 戦闘バリアント
│   │   ├── Anims/                # アニメーション
│   │   ├── Blueprints/           # ブループリント
│   │   ├── Input/                # 入力設定
│   │   ├── Materials/            # マテリアル
│   │   ├── UI/                   # UIウィジェット
│   │   └── VFX/                  # ビジュアルエフェクト
│   ├── Variant_SideScrolling/    # 横スクロールバリアント
│   └── MermaidMaterials/         # Mermaidマテリアル
│
├── Plugins/                       # プロジェクトプラグイン
│   ├── ScreenTransitionSystem/   # 画面遷移システムプラグイン
│   │   ├── Source/               # C++ソースコード
│   │   └── Documentation/        # ドキュメント
│   └── MermaidMaterial/          # Mermaidマテリアルプラグイン
│       └── Content/
│           └── Python/           # Pythonスクリプト
│
├── Config/                        # プロジェクト設定
│   ├── DefaultEngine.ini         # エンジン設定
│   ├── DefaultGame.ini           # ゲーム設定
│   ├── DefaultInput.ini          # 入力設定
│   ├── DefaultEditor.ini         # エディタ設定
│   └── DefaultEditorPerProjectUserSettings.ini
│
└── scripts/                       # 開発支援スクリプト
    ├── link-plugin.bat/sh        # プラグインリンクスクリプト
    ├── quick-link.sh             # プラグイン高速リンク
    ├── check-plugin-syntax.sh    # プラグイン構文チェック
    └── test-build.bat            # ビルドテスト
```

## コーディング規約

### C++コーディング規約

#### 命名規則
- **クラス名**: PascalCase、型プレフィックス付き
  - `A` - Actor派生クラス（例: `ACombatCharacter`, `ACombatEnemy`）
  - `U` - UObject派生クラス（例: `USpringArmComponent`, `UCombatLifeBar`）
  - `F` - 構造体（例: `FInputActionValue`, `FTimerHandle`）
  - `I` - インターフェース（例: `ICombatAttacker`, `ICombatDamageable`）
  - `E` - 列挙型（例: `EEndPlayReason`）
  - `T` - テンプレート（例: `TObjectPtr<T>`）

- **関数名**: PascalCase
  - 例: `BeginPlay()`, `SetupPlayerInputComponent()`, `ApplyDamage()`
  - Blueprint呼び出し可能関数の接頭辞: `Do`（例: `DoMove()`, `DoLook()`）
  - Blueprintイベントの接頭辞: `BP_`（例: `BP_ToggleCamera()`）

- **変数名**:
  - メンバ変数: PascalCase（例: `CameraBoom`, `FollowCamera`）
  - ローカル変数: PascalCase（例: `Damage`, `DamageCauser`）
  - ブール値: `b`プレフィックス（例: `bIsAttacking`, `bIsChargingAttack`）
  - ポインタ: 通常はプレフィックスなし、ただし明示的な場合は検討

- **定数**: UPPER_SNAKE_CASE または PascalCase

#### UPROPERTYマクロ使用ガイドライン

```cpp
// エディタで編集可能な値
UPROPERTY(EditAnywhere, Category="Damage", meta = (ClampMin = 0, ClampMax = 100))
float MaxHP = 5.0f;

// Blueprintから読み取り専用
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
USpringArmComponent* CameraBoom;

// Blueprintから読み書き可能
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
FLinearColor LifeBarColor;

// 実行時にのみ表示
UPROPERTY(VisibleAnywhere, Category="Damage")
float CurrentHP = 0.0f;
```

**metaタグの使用例**:
- `ClampMin/ClampMax`: 数値の範囲制限
- `Units`: 単位の表示（cm, s, deg等）
- `AllowPrivateAccess`: プライベート変数へのBlueprint アクセス許可

#### UFUNCTIONマクロ使用ガイドライン

```cpp
// Blueprintから呼び出し可能
UFUNCTION(BlueprintCallable, Category="Input")
virtual void DoMove(float Right, float Forward);

// Blueprintで実装可能なイベント
UFUNCTION(BlueprintImplementableEvent, Category="Combat")
void BP_ToggleCamera();

// Blueprint実装可能、C++でデフォルト実装あり
UFUNCTION(BlueprintNativeEvent, Category="Combat")
void OnDamageReceived(float Damage);
```

### Blueprint命名規約

- **Blueprint クラス**: `BP_`（例: `BP_CombatCharacter`, `BP_Enemy`）
- **Widget Blueprint**: `WBP_`（例: `WBP_LifeBar`, `WBP_MainMenu`）
- **Material**: `M_`（例: `M_Character`, `M_Environment`）
- **Material Instance**: `MI_`（例: `MI_Character_Blue`, `MI_Floor_Concrete`）
- **Texture**: `T_`（例: `T_Character_Diffuse`, `T_Normal`）
- **Input Action**: `IA_`（例: `IA_Jump`, `IA_Move`）
- **Input Mapping Context**: `IMC_`（例: `IMC_Default`, `IMC_Combat`）

## 技術情報

### よく使うモジュール

#### 必須モジュール（.uproject/Build.cs）
- **Engine**: 基本的なゲーム機能
- **AIModule**: AI機能（BehaviorTree, EQS等）
- **UMG**: UIウィジェット

#### 有効化されているプラグイン
- **StateTree**: State Tree AI システム
- **GameplayStateTree**: ゲームプレイ用State Tree拡張
- **GameplayAbilities**: Gameplay Ability System (GAS)
- **AIAssistant**: AI開発支援
- **ModelingToolsEditorMode**: モデリングツール（エディタのみ）

#### EnhancedInput システム
このプロジェクトはEnhanced Input Systemを使用しています。
- Input Actions: `Content/Input/Actions/`
- Input Mapping Context: 各バリアントごとに設定
- 実装例: `CombatCharacter.h`の`UInputAction*`プロパティ

### ビルドとテストの手順

#### ビルド
```bat
# Windowsでビルド
Build.bat

# または手動で
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" ^
  AntigravityTestEditor Win64 Development -Project="%~dp0AntigravityTest.uproject" -WaitMutex -FromMsBuild
```

#### エディタ起動
```bat
# Windowsでエディタ起動
LaunchEditor.bat

# または手動で
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0AntigravityTest.uproject"
```

#### プラグインリンク（開発時）
```bash
# 高速リンク（推奨）
./scripts/quick-link.sh ScreenTransitionSystem /path/to/UE5Project

# または対話型
./scripts/link-plugin.sh
```

詳細は `scripts/README.md` を参照してください。

### 重要な実装パターン

#### GameMode
- `AntigravityGoalGameMode`: ゲームモードの基底クラス
- 各バリアントごとにGameModeを派生させる

#### Character
- **基底クラス**: `ACharacter`（Unreal標準）
- **戦闘キャラクター**: `ACombatCharacter`
  - インターフェース: `ICombatAttacker`, `ICombatDamageable`
  - コンポーネント: Camera Boom, Follow Camera, Life Bar Widget
  - 機能: コンボ攻撃、チャージ攻撃、ダメージ処理、リスポーン

#### Input処理（Enhanced Input）
```cpp
// Input Actionの定義
UPROPERTY(EditAnywhere, Category="Input")
UInputAction* MoveAction;

// SetupPlayerInputComponentでバインド
void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Move);
    }
}

// 入力ハンドラ
void ACombatCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    // 移動処理...
}
```

#### インターフェース実装
```cpp
// インターフェース定義
UCLASS(abstract)
class ACombatCharacter : public ACharacter, public ICombatAttacker, public ICombatDamageable
{
    GENERATED_BODY()

    // インターフェースメソッドのオーバーライド
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
                            const FVector& DamageLocation,
                            const FVector& DamageImpulse) override;
};
```

## 注意事項

### 実装時の注意点

#### Blueprint連携
- C++クラスは`abstract`にして、Blueprint で具体的な実装を作成する
- `BlueprintImplementableEvent`は C++ 実装不要（Blueprint専用）
- `BlueprintNativeEvent`は C++ にデフォルト実装、Blueprint でオーバーライド可能
- `BlueprintCallable`で Blueprintから呼び出し可能に
- `Category`を適切に設定してBlueprint エディタでの可読性を向上

#### ネットワーク対応
- 現時点ではシングルプレイヤー想定
- 将来的にマルチプレイヤー対応する場合は、Replicationを考慮
- `UPROPERTY(Replicated)`と`GetLifetimeReplicatedProps()`の実装が必要

#### ガベージコレクション
- `UPROPERTY()`マクロでマークされていないUObject参照は GC される可能性あり
- 必ず`UPROPERTY()`でマークするか、`TObjectPtr<>`を使用
- `AddToRoot()`/`RemoveFromRoot()`は最終手段として使用

```cpp
// 良い例
UPROPERTY()
TObjectPtr<UCombatLifeBar> LifeBarWidget;

// 悪い例（GCされる可能性あり）
UCombatLifeBar* LifeBarWidget;  // UPROPERTY()なし
```

#### モジュール依存関係
プラグインやモジュールを追加する場合：

1. **.uproject** に追加:
```json
{
    "Name": "YourPlugin",
    "Enabled": true
}
```

2. **Build.cs** に追加:
```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "YourModule"
});
```

#### メモリ管理
- Actorは`Destroy()`で破棄
- UObjectは参照がなくなればGCが回収
- `NewObject<T>()`で動的に生成
- コンポーネントは`CreateDefaultSubobject<T>()`（コンストラクタのみ）

### トラブルシューティング

#### コンパイルエラー
1. **"Cannot open include file"**
   - `Build.cs`で必要なモジュールが追加されているか確認
   - インクルードパスが正しいか確認

2. **"Unresolved external symbol"**
   - `Build.cs`で依存モジュールが追加されているか確認
   - リンクエラーの場合、プロジェクトを再生成: 右クリック > Generate Visual Studio project files

3. **"UPROPERTY/UFUNCTION not recognized"**
   - `GENERATED_BODY()`マクロがクラスに含まれているか確認
   - ヘッダーファイル名とクラス名が一致しているか確認

#### Blueprint エラー
1. **"Blueprint could not be loaded"**
   - 参照しているC++クラスがコンパイルされているか確認
   - C++クラスの関数シグネチャが変更された場合、Blueprintを開き直す

2. **"Function/Variable not found"**
   - `BlueprintCallable`, `BlueprintReadWrite`等が正しく設定されているか確認
   - C++コンパイル後、エディタを再起動

#### プラグインリンク
- **シンボリックリンクが作成されない**:
  - Windowsは管理者権限が必要
  - `scripts/link-plugin.bat`を管理者として実行
- **プラグインが認識されない**:
  - `.uplugin`ファイルが存在するか確認
  - エディタを再起動

## プラグイン開発

### ScreenTransitionSystem
画面遷移システムプラグイン。非同期ロード、トランジションエフェクト、画面スタック管理をサポート。

- **場所**: `Plugins/ScreenTransitionSystem/`
- **モジュールタイプ**: Runtime
- **機能**: UI画面遷移、非同期ロード、エフェクト管理

### MermaidMaterial
Mermaidマテリアル生成プラグイン。

- **場所**: `Plugins/MermaidMaterial/`
- **機能**: Pythonスクリプトによるマテリアル生成

## 参考資料

- [Unreal Engine 5.7 Documentation](https://docs.unrealengine.com/5.7/)
- [Enhanced Input Documentation](https://docs.unrealengine.com/5.7/enhanced-input-in-unreal-engine/)
- [Gameplay Ability System](https://docs.unrealengine.com/5.7/gameplay-ability-system-for-unreal-engine/)
- [Coding Standard](https://docs.unrealengine.com/5.7/epic-cplusplus-coding-standard-for-unreal-engine/)

## Claude Code での作業のヒント

### 効率的な作業フロー
1. **ファイル検索**: `Glob`ツールで`**/*.h`, `**/*.cpp`を検索
2. **コード検索**: `Grep`ツールで関数やクラスを検索
3. **ビルド確認**: `Build.bat`でコンパイルエラーを確認
4. **プラグインテスト**: `scripts/quick-link.sh`でプラグインをリンク

### 新機能追加時のチェックリスト
- [ ] C++クラスに適切なUPROPERTY/UFUNCTIONマクロを設定
- [ ] Build.csに必要なモジュールを追加
- [ ] Blueprintで派生クラスを作成（必要に応じて）
- [ ] Input Actions/Mapping Contextを設定（入力が必要な場合）
- [ ] Build.batでコンパイル確認
- [ ] エディタで動作確認

### よくある質問
**Q: 新しいC++クラスを追加したい**
- Visual Studioでクラスを追加するか、エディタの「C++ Class」から追加
- `UCLASS()`, `GENERATED_BODY()`マクロを忘れずに
- Build.csに必要なモジュールを追加

**Q: Blueprintから呼び出せない**
- `UFUNCTION(BlueprintCallable)`が設定されているか確認
- 関数がpublic/protectedか確認
- エディタを再起動

**Q: ガベージコレクションで削除される**
- `UPROPERTY()`マクロでマークする
- または`TObjectPtr<>`を使用
