# プラグイン統合ガイド

このドキュメントは、AntigravityTestプロジェクトにおけるプラグインの使い方、開発方法、統合手順をまとめたガイドです。

## 目次

1. [既存プラグインの概要](#既存プラグインの概要)
2. [プラグイン間の依存関係](#プラグイン間の依存関係)
3. [新規プラグイン作成ガイド](#新規プラグイン作成ガイド)
4. [プラグインのリンクとデプロイ](#プラグインのリンクとデプロイ)
5. [Blueprint連携のベストプラクティス](#blueprint連携のベストプラクティス)
6. [テストとデバッグ](#テストとデバッグ)

---

## 既存プラグインの概要

### ScreenTransitionSystem

**機能**: UE5用の柔軟な画面遷移システムプラグイン

**主要な特徴**:
- GameInstanceSubsystemベースの中央管理システム
- 画面スタック機能（前の画面に戻る機能）
- トランジションエフェクト（フェード、スライドなど）
- Blueprint完全対応
- ライフサイクル管理（OnEnter、OnExit、OnPause、OnResume）
- カスタムトランジションエフェクトの作成が可能

**主要クラス**:
- `UScreenTransitionManager` - GameInstanceSubsystem、画面遷移の中央管理
- `UScreenBase` - すべての画面の基底クラス（UUserWidget派生）
- `UTransitionEffect` - トランジションエフェクトの基底クラス

**モジュール依存関係**:
```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "UMG",
    "Slate",
    "SlateCore"
});
```

**ディレクトリ構造**:
```
Plugins/ScreenTransitionSystem/
├── README.md                           # 基本的な使い方
├── ScreenTransitionSystem.uplugin      # プラグイン定義
├── Source/ScreenTransitionSystem/
│   ├── Public/
│   │   ├── ScreenTransitionManager.h   # メインマネージャー
│   │   ├── ScreenBase.h                # 画面基底クラス
│   │   └── TransitionEffect.h          # トランジションエフェクト
│   └── Private/
│       └── (実装ファイル)
├── Content/
│   ├── UI/                             # UIアセット用
│   └── Effects/                        # エフェクト用
└── Documentation/
    ├── Usage.md                        # 詳細な使用方法
    ├── API.md                          # APIリファレンス
    └── Examples.md                     # サンプルコード
```

**使い方の基本**:

```cpp
// C++での画面遷移
UScreenTransitionManager* Manager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
Manager->TransitionToScreen(UWBP_TitleScreen::StaticClass(), true);

// 画面をスタックにプッシュ（戻る機能）
Manager->PushScreen(UWBP_SettingsScreen::StaticClass(), true, true);

// 前の画面に戻る
Manager->PopScreen(true);
```

**Blueprint使用例**:
```
Get Game Instance
  └─ Get Subsystem (Screen Transition Manager)
      └─ Transition To Screen
          ├─ Screen Class: WBP_TitleScreen
          ├─ Use Transition: true
          └─ Transition Effect Class: (Optional)
```

**詳細ドキュメント**:
- [README.md](Plugins/ScreenTransitionSystem/README.md)
- [詳細な使用方法](Plugins/ScreenTransitionSystem/Documentation/Usage.md)
- [APIリファレンス](Plugins/ScreenTransitionSystem/Documentation/API.md)
- [サンプル集](Plugins/ScreenTransitionSystem/Documentation/Examples.md)

---

### MermaidMaterial

**機能**: Mermaid図からマテリアルグラフを生成するエディタ拡張プラグイン

**主要な特徴**:
- Python Script Plugin統合
- Mermaid構文でマテリアルグラフを定義
- エディタスクリプティング機能
- ベータ版・実験的機能

**モジュール依存関係**:
プラグイン依存（`.uplugin`で定義）:
```json
"Plugins": [
    {
        "Name": "PythonScriptPlugin",
        "Enabled": true
    },
    {
        "Name": "EditorScriptingUtilities",
        "Enabled": true
    }
]
```

**ディレクトリ構造**:
```
Plugins/MermaidMaterial/
├── MermaidMaterial.uplugin
└── Content/
    └── Python/
        ├── init_unreal.py          # Unreal初期化
        ├── mermaid_parser.py       # Mermaid構文パーサー
        ├── material_generator.py   # マテリアル生成
        └── mermaid_window.py       # エディタウィンドウUI
```

**Python連携について**:
- UE5のPython Script Pluginを使用
- エディタ内でPythonスクリプトを実行
- `Content/Python/`にスクリプトを配置
- Mermaid図をパースしてマテリアルノードを自動生成

**使い方**:
1. エディタで`Tools > Python`メニューから実行
2. Mermaid構文でマテリアルグラフを記述
3. Pythonスクリプトがマテリアルアセットを生成

**注意**: このプラグインはベータ版・実験的機能です。本番環境での使用前に十分なテストを行ってください。

---

## プラグイン間の依存関係

### 依存関係マップ

現在のプロジェクトには、プラグイン間の直接的な依存関係はありません。各プラグインは独立して動作します。

```
AntigravityTest (メインプロジェクト)
├── ScreenTransitionSystem (独立)
│   └─ 依存: UMG, Slate, SlateCore
└── MermaidMaterial (独立)
    └─ 依存: PythonScriptPlugin, EditorScriptingUtilities
```

### モジュール間の連携方法

プラグイン間で連携が必要な場合は、以下の方法を使用します：

#### 1. インターフェースを使用した疎結合

```cpp
// PluginA/Public/IPluginAInterface.h
UINTERFACE(MinimalAPI, Blueprintable)
class UPluginAInterface : public UInterface
{
    GENERATED_BODY()
};

class IPluginAInterface
{
    GENERATED_BODY()

public:
    virtual void DoSomething() = 0;
};

// PluginBでPluginAのインターフェースを実装
class UPluginBClass : public UObject, public IPluginAInterface
{
    virtual void DoSomething() override;
};
```

#### 2. Subsystemを使用した機能提供

```cpp
// PluginA/Public/PluginASubsystem.h
UCLASS()
class UPluginASubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="PluginA")
    void RegisterCallback(UObject* Listener);
};

// PluginBから利用
UPluginASubsystem* Subsystem = GetGameInstance()->GetSubsystem<UPluginASubsystem>();
if (Subsystem)
{
    Subsystem->RegisterCallback(this);
}
```

#### 3. Build.csでの依存設定

プラグイン間で直接依存する場合は、`.Build.cs`ファイルで依存関係を明示します：

```csharp
// PluginB.Build.cs
public class PluginB : ModuleRules
{
    public PluginB(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "PluginA"  // PluginAへの依存を追加
            }
        );
    }
}
```

**注意**: 循環依存を避けるため、プラグイン間の依存関係は最小限に抑えることを推奨します。

---

## 新規プラグイン作成ガイド

### 基本手順

#### 1. プラグインディレクトリの作成

```bash
# プロジェクトルートで実行
mkdir -p Plugins/MyPlugin/Source/MyPlugin/{Public,Private}
mkdir -p Plugins/MyPlugin/Content
mkdir -p Plugins/MyPlugin/Resources
```

#### 2. .upluginファイルの作成

`Plugins/MyPlugin/MyPlugin.uplugin`:

```json
{
    "FileVersion": 3,
    "Version": 1,
    "VersionName": "1.0",
    "FriendlyName": "My Plugin",
    "Description": "プラグインの説明",
    "Category": "Gameplay",
    "CreatedBy": "Your Name",
    "CreatedByURL": "",
    "DocsURL": "",
    "MarketplaceURL": "",
    "SupportURL": "",
    "CanContainContent": true,
    "IsBetaVersion": false,
    "IsExperimentalVersion": false,
    "Installed": false,
    "Modules": [
        {
            "Name": "MyPlugin",
            "Type": "Runtime",
            "LoadingPhase": "Default"
        }
    ]
}
```

**主要なフィールド**:
- `FileVersion`: 3（UE5用）
- `FriendlyName`: エディタで表示される名前
- `Category`: プラグインのカテゴリ（Gameplay, UI, Editor, etc.）
- `CanContainContent`: Contentフォルダを持つ場合はtrue
- `Modules[].Type`: Runtime（実行時）/ Editor（エディタのみ）/ RuntimeAndEditor

#### 3. Build.csファイルの作成

`Plugins/MyPlugin/Source/MyPlugin/MyPlugin.Build.cs`:

```csharp
using UnrealBuildTool;

public class MyPlugin : ModuleRules
{
    public MyPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine"
                // 必要なモジュールを追加
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                // プライベート依存モジュール
            }
        );
    }
}
```

**よく使うモジュール**:
- `Core`, `CoreUObject`, `Engine` - 基本モジュール（必須）
- `UMG` - UI Widget
- `Slate`, `SlateCore` - UI フレームワーク
- `AIModule` - AI機能
- `GameplayAbilities` - Gameplay Ability System
- `InputCore` - 入力処理
- `OnlineSubsystem` - オンライン機能

#### 4. モジュールヘッダーの作成

`Plugins/MyPlugin/Source/MyPlugin/Public/MyPlugin.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMyPluginModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
```

#### 5. モジュール実装の作成

`Plugins/MyPlugin/Source/MyPlugin/Private/MyPlugin.cpp`:

```cpp
#include "MyPlugin.h"

#define LOCTEXT_NAMESPACE "FMyPluginModule"

void FMyPluginModule::StartupModule()
{
    // プラグイン起動時の処理
    UE_LOG(LogTemp, Log, TEXT("MyPlugin module has started"));
}

void FMyPluginModule::ShutdownModule()
{
    // プラグイン終了時の処理
    UE_LOG(LogTemp, Log, TEXT("MyPlugin module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyPluginModule, MyPlugin)
```

### プラグインテンプレート構造

完全なプラグイン構造の例：

```
Plugins/MyPlugin/
├── MyPlugin.uplugin               # プラグイン定義
├── README.md                      # 使い方とドキュメント
├── Source/
│   └── MyPlugin/
│       ├── Public/                # パブリックヘッダー
│       │   ├── MyPlugin.h        # モジュールヘッダー
│       │   ├── MyComponent.h     # コンポーネント例
│       │   └── MySubsystem.h     # サブシステム例
│       ├── Private/               # 実装ファイル
│       │   ├── MyPlugin.cpp
│       │   ├── MyComponent.cpp
│       │   └── MySubsystem.cpp
│       └── MyPlugin.Build.cs     # ビルド設定
├── Content/                       # Blueprintとアセット
│   ├── Blueprints/
│   ├── UI/
│   └── Data/
├── Resources/                     # アイコンなど
│   └── Icon128.png
└── Documentation/                 # 詳細ドキュメント
    ├── Usage.md
    ├── API.md
    └── Examples.md
```

### 既存プラグインを参考にする方法

#### ScreenTransitionSystemを参考にしたシステム型プラグイン

ScreenTransitionSystemのようなシステム型プラグインを作成する場合：

**特徴**:
- GameInstanceSubsystemを使用した中央管理
- UObjectベースのデータクラス
- Blueprint完全対応
- ライフサイクル管理

**参考にすべきファイル**:
1. `ScreenTransitionManager.h` - Subsystemの実装例
2. `ScreenBase.h` - 基底クラスの設計
3. `TransitionEffect.h` - カスタマイズ可能な機能の実装

**実装パターン**:
```cpp
// GameInstanceSubsystemの実装例
UCLASS()
class MYPLUGIN_API UMySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Subsystemの初期化
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Blueprint呼び出し可能な主要機能
    UFUNCTION(BlueprintCallable, Category="MyPlugin")
    void DoSomething();

    // イベント
    UPROPERTY(BlueprintAssignable, Category="MyPlugin")
    FOnSomethingChanged OnSomethingChanged;
};
```

#### コンポーネント型プラグイン

ActorComponentやSceneComponentベースのプラグインを作成する場合：

```cpp
// コンポーネントの実装例
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPLUGIN_API UMyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMyComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                              FActorComponentTickFunction* ThisTickFunction) override;

public:
    UFUNCTION(BlueprintCallable, Category="MyComponent")
    void DoSomething();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
    float SomeValue = 1.0f;
};
```

### エディタ拡張プラグイン

MermaidMaterialのようなエディタ拡張プラグインを作成する場合：

**Module Type**: Editor

`.uplugin`:
```json
"Modules": [
    {
        "Name": "MyEditorPlugin",
        "Type": "Editor",
        "LoadingPhase": "Default"
    }
]
```

`Build.cs`:
```csharp
PublicDependencyModuleNames.AddRange(
    new string[]
    {
        "Core",
        "CoreUObject",
        "Engine",
        "UnrealEd",           // エディタ機能
        "EditorStyle",        // エディタスタイル
        "Slate",              // UI
        "SlateCore",          // UI
        "PropertyEditor",     // プロパティエディタ
        "EditorScriptingUtilities"  // エディタスクリプティング
    }
);
```

---

## プラグインのリンクとデプロイ

### scripts/link-plugin の使い方

Worktreeで開発したプラグインをUE5プロジェクトにシンボリックリンクで接続します。

#### quick-link.sh - コマンドライン版（推奨）

最も簡単な方法です。引数を指定してすぐに実行できます。

```bash
# コマンドライン形式
./scripts/quick-link.sh <PluginName> <TargetProjectPath>

# 例
./scripts/quick-link.sh ScreenTransitionSystem /path/to/UE5Project
```

#### link-plugin.bat/sh - 対話型

プロンプトに従って入力する対話型バージョンです。

**Windows**:
```batch
# 管理者権限でコマンドプロンプトを開く
scripts\link-plugin.bat

# プロンプトに従って入力
Enter plugin name: ScreenTransitionSystem
Enter your UE5 project path: C:\MyUE5Project
```

**Linux/Mac**:
```bash
./scripts/link-plugin.sh

# プロンプトに従って入力
Enter plugin name: ScreenTransitionSystem
Enter your UE5 project path: /home/user/MyUE5Project
```

### worktree間でのプラグイン共有

Git worktreeを使用すると、複数のブランチで同時に開発できます。

```bash
# 新しいWorktreeを作成
git worktree add ../feature-branch feature-branch

# Worktreeに移動
cd ../feature-branch

# プラグインをUE5プロジェクトにリンク
./scripts/quick-link.sh ScreenTransitionSystem /path/to/UE5Project

# UE5エディタで確認
# このWorktreeでの変更が即座にUE5に反映される
```

**利点**:
- **リアルタイム反映**: Worktreeでの変更が即座にUE5プロジェクトに反映
- **重複なし**: ファイルをコピーする必要がなく、一箇所で管理
- **簡単な切り替え**: 複数のWorktreeを簡単に切り替え可能

### 使用例

#### 新しいWorktreeでプラグインを開発

```bash
# Worktreeを作成
git worktree add ../feature-new-transition feature-new-transition

# Worktreeに移動
cd ../feature-new-transition

# プラグインをUE5プロジェクトにリンク
./scripts/quick-link.sh ScreenTransitionSystem C:/MyUE5Project

# UE5エディタでプラグインを確認
# 変更を加えると即座にUE5に反映される
```

#### 複数のプロジェクトで同じプラグインをテスト

```bash
# プロジェクトAにリンク
./scripts/quick-link.sh ScreenTransitionSystem C:/ProjectA

# プロジェクトBにもリンク
./scripts/quick-link.sh ScreenTransitionSystem C:/ProjectB

# 両方のプロジェクトで同じコードが使用される
```

#### 異なるバージョンのプラグインを比較

```bash
# Worktree1（古いバージョン）
cd ../worktree1
./scripts/quick-link.sh ScreenTransitionSystem C:/ProjectOld

# Worktree2（新しいバージョン）
cd ../worktree2
./scripts/quick-link.sh ScreenTransitionSystem C:/ProjectNew

# 2つのUE5プロジェクトで異なるバージョンをテスト
```

### 注意事項

1. **シンボリックリンクの削除**
   - シンボリックリンクを削除する場合は、フォルダごと削除してください
   - リンク元のファイルは削除されません

2. **UE5での動作**
   - シンボリックリンクはUE5エディタで通常のフォルダとして認識されます
   - プラグインの有効化・無効化は通常通り行えます

3. **バージョン管理**
   - シンボリックリンク自体はgitで管理されません
   - 各開発者が自分の環境でリンクを作成する必要があります

4. **Windows権限**
   - Windowsではシンボリックリンクの作成に管理者権限が必要です
   - `link-plugin.bat`を右クリック → 「管理者として実行」

詳細は[scripts/README.md](scripts/README.md)を参照してください。

---

## Blueprint連携のベストプラクティス

### UFUNCTION(BlueprintCallable)の使い方

Blueprint から C++ 関数を呼び出せるようにします。

```cpp
// 基本的な使い方
UFUNCTION(BlueprintCallable, Category="MyPlugin")
void DoSomething();

// 戻り値あり
UFUNCTION(BlueprintCallable, Category="MyPlugin")
float CalculateDamage(float BaseDamage, float Multiplier);

// 複数のパラメータ
UFUNCTION(BlueprintCallable, Category="MyPlugin")
void ApplyEffect(AActor* Target, float Duration, bool bIsPositive);
```

**Categoryの設定**:
- Blueprintエディタでの可読性を向上させるため、必ず適切なCategoryを設定
- プラグイン名や機能名をCategoryに使用（例: `"ScreenTransition"`, `"Combat"`, `"Quest"`）

### UPROPERTY(EditAnywhere, BlueprintReadWrite)の設定

Blueprint でプロパティを編集・参照できるようにします。

```cpp
// エディタで編集可能、Blueprintから読み書き可能
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
float MaxHealth = 100.0f;

// エディタで編集可能、Blueprintから読み取り専用
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
float BaseDamage = 10.0f;

// 実行時にのみ表示、Blueprintから読み取り専用
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status")
float CurrentHealth = 0.0f;

// エディタで編集可能、プライベートメンバへのアクセス許可
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
TObjectPtr<USpringArmComponent> CameraBoom;
```

**設定の組み合わせ**:
- `EditAnywhere` - エディタとインスタンスの両方で編集可能
- `EditDefaultsOnly` - エディタ（ブループリントのデフォルト）でのみ編集可能
- `EditInstanceOnly` - インスタンス（レベル配置後）でのみ編集可能
- `VisibleAnywhere` / `VisibleDefaultsOnly` / `VisibleInstanceOnly` - 編集不可、表示のみ
- `BlueprintReadWrite` - Blueprint から読み書き可能
- `BlueprintReadOnly` - Blueprint から読み取り専用

### カテゴリ分けとTooltip設定

```cpp
// Categoryでグループ化
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Settings")
float BaseDamage = 10.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Settings")
float CriticalMultiplier = 2.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
float WalkSpeed = 600.0f;

// Tooltipで説明を追加
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Settings", meta = (ToolTip="基本ダメージ量。クリティカル時はCriticalMultiplierで乗算されます"))
float BaseDamageWithTooltip = 10.0f;

// 数値範囲を制限
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Settings", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
float ClampedDamage = 50.0f;

// UIでスライダー表示
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (UIMin = "0.0", UIMax = "100.0"))
float SliderValue = 50.0f;

// 単位を指定
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (Units = "cm/s"))
float Speed = 600.0f;
```

**便利なmetaタグ**:
- `ToolTip` - プロパティの説明
- `ClampMin` / `ClampMax` - 値の範囲制限（実際の値を制限）
- `UIMin` / `UIMax` - UIスライダーの範囲（値自体は制限しない）
- `Units` - 単位の表示（cm, s, deg, など）
- `AllowPrivateAccess` - プライベートメンバへのBlueprintアクセス許可
- `BlueprintProtected` - 派生Blueprintからのみアクセス可能

### BlueprintImplementableEvent vs BlueprintNativeEvent

#### BlueprintImplementableEvent

Blueprint専用のイベント。C++実装は不要。

```cpp
// Blueprintでのみ実装するイベント
UFUNCTION(BlueprintImplementableEvent, Category="Combat")
void BP_OnDamageReceived(float Damage);

// 呼び出し側（C++）
void AMyCharacter::TakeDamage(float Damage)
{
    CurrentHealth -= Damage;
    BP_OnDamageReceived(Damage);  // Blueprintで実装されたイベントを呼び出し
}
```

#### BlueprintNativeEvent

C++でデフォルト実装を提供し、Blueprintでオーバーライド可能。

```cpp
// ヘッダー
UFUNCTION(BlueprintNativeEvent, Category="Combat")
void OnDamageReceived(float Damage);

// 実装には _Implementation サフィックスが必須
virtual void OnDamageReceived_Implementation(float Damage);

// CPP
void AMyCharacter::OnDamageReceived_Implementation(float Damage)
{
    // C++でのデフォルト実装
    CurrentHealth -= Damage;
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

// 呼び出し側
void AMyCharacter::TakeDamage(float Damage)
{
    OnDamageReceived(Damage);  // Blueprintでオーバーライドされていればそちらを実行
}
```

**使い分けのガイドライン**:
- `BlueprintImplementableEvent`: Blueprint専用、C++でデフォルト動作が不要な場合
  - 例: ビジュアルエフェクト、サウンド再生、カメラシェイク
- `BlueprintNativeEvent`: C++でデフォルト実装、Blueprintでカスタマイズ可能
  - 例: ダメージ処理、アイテム使用、AI判定

### ScreenTransitionSystemの実装例を参考

ScreenTransitionSystemは、優れたBlueprint連携の例です：

```cpp
// ScreenBase.h から抜粋

// Blueprint実装可能なライフサイクルイベント
UFUNCTION(BlueprintNativeEvent, Category="Screen")
void OnEnter();
virtual void OnEnter_Implementation();

UFUNCTION(BlueprintNativeEvent, Category="Screen")
void OnExit();
virtual void OnExit_Implementation();

UFUNCTION(BlueprintNativeEvent, Category="Screen")
void OnPause();
virtual void OnPause_Implementation();

UFUNCTION(BlueprintNativeEvent, Category="Screen")
void OnResume();
virtual void OnResume_Implementation();

// Blueprint呼び出し可能な設定
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Settings")
bool bShowMouseCursor = true;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Settings")
bool bEnableClickEvents = true;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Settings")
int32 ZOrder = 0;
```

### インターフェースのBlueprint連携

```cpp
// インターフェース定義
UINTERFACE(MinimalAPI, Blueprintable)
class UCombatDamageable : public UInterface
{
    GENERATED_BODY()
};

class ICombatDamageable
{
    GENERATED_BODY()

public:
    // Blueprint呼び出し可能なインターフェースメソッド
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Combat")
    void ApplyDamage(float Damage, AActor* DamageCauser);
};

// 実装クラス
UCLASS()
class AMyCharacter : public ACharacter, public ICombatDamageable
{
    GENERATED_BODY()

public:
    // インターフェースメソッドの実装
    virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser) override;
};
```

**Blueprint での使用**:
```
Target Actor (ICombatDamageable をキャスト)
  └─ Apply Damage
      ├─ Damage: 50.0
      └─ Damage Causer: Self
```

---

## テストとデバッグ

### プラグインの単体テスト

#### テストクラスの作成

```cpp
// Plugins/MyPlugin/Source/MyPlugin/Private/Tests/MyPluginTest.cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "MyPlugin.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyPluginBasicTest,
    "MyPlugin.BasicTest",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMyPluginBasicTest::RunTest(const FString& Parameters)
{
    // テストコード
    UMyComponent* Component = NewObject<UMyComponent>();
    TestNotNull(TEXT("Component should be created"), Component);

    Component->DoSomething();
    TestEqual(TEXT("Value should be updated"), Component->SomeValue, 2.0f);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
```

#### テストの実行

**エディタから**:
1. Window > Test Automation
2. テストを検索（例: "MyPlugin"）
3. 実行したいテストを選択してRunボタンをクリック

**コマンドラインから**:
```batch
# Windowsの場合
UnrealEditor.exe "C:/Path/To/Project.uproject" -ExecCmds="Automation RunTests MyPlugin" -unattended -nopause -NullRHI -log

# Linuxの場合
./UnrealEditor "Path/To/Project.uproject" -ExecCmds="Automation RunTests MyPlugin" -unattended -nopause -NullRHI -log
```

### scripts/check-plugin-syntax.shの使用

プラグインの構文エラーを事前チェックします。

```bash
# 特定のプラグインをチェック
./scripts/check-plugin-syntax.sh ScreenTransitionSystem

# すべてのプラグインをチェック
./scripts/check-plugin-syntax.sh
```

このスクリプトは以下をチェックします：
- `.uplugin` ファイルの存在
- `.Build.cs` ファイルの存在と構文
- ヘッダーファイルの基本的な構文
- `GENERATED_BODY()` マクロの存在

### scripts/test-build.batでのビルド確認

プラグインを含むプロジェクト全体をビルドして確認します。

```batch
# Windowsでビルドテスト
scripts\test-build.bat
```

**実行内容**:
1. Unreal Build Tool を実行
2. プロジェクトとすべてのプラグインをコンパイル
3. エラーと警告を出力

**注意**: GitHub Actions などの CI 環境では、Unreal Engine のパスが環境変数として設定されている必要があります。

### デバッグのベストプラクティス

#### ログ出力

```cpp
// 基本的なログ
UE_LOG(LogTemp, Log, TEXT("MyPlugin: Something happened"));

// 警告
UE_LOG(LogTemp, Warning, TEXT("MyPlugin: This is a warning"));

// エラー
UE_LOG(LogTemp, Error, TEXT("MyPlugin: An error occurred"));

// カスタムログカテゴリ
DECLARE_LOG_CATEGORY_EXTERN(LogMyPlugin, Log, All);

// CPPファイルで定義
DEFINE_LOG_CATEGORY(LogMyPlugin);

// 使用
UE_LOG(LogMyPlugin, Log, TEXT("Custom log category message"));

// 変数の値を出力
UE_LOG(LogMyPlugin, Log, TEXT("Health: %f, Name: %s"), CurrentHealth, *PlayerName);
```

#### ブループリントデバッグノード

Blueprintでデバッグしやすいように、専用のデバッグ関数を提供します。

```cpp
// デバッグ情報を取得
UFUNCTION(BlueprintCallable, Category="MyPlugin|Debug")
FString GetDebugInfo() const;

// デバッグ描画
UFUNCTION(BlueprintCallable, Category="MyPlugin|Debug")
void DrawDebugInfo(float Duration = 5.0f);
```

#### アサーション

```cpp
// 条件チェック（開発ビルドのみ）
check(SomePointer != nullptr);
checkSlow(ExpensiveCondition());  // 低頻度チェック（遅い条件）

// メッセージ付きアサーション
checkf(Condition, TEXT("Error message with value: %d"), SomeValue);

// リリースビルドでも実行されるチェック
verify(SomeFunction());
```

#### Visual Studio デバッガー

1. Visual Studio で `.sln` ファイルを開く
2. プラグインのコードにブレークポイントを設定
3. F5 キーでデバッグ開始（UE5 エディタが起動）
4. エディタ内でプラグイン機能を実行してブレークポイントにヒット

### トラブルシューティング

#### プラグインが認識されない

1. `.uplugin` ファイルが存在するか確認
2. JSON 構文が正しいか確認
3. エディタを再起動
4. プロジェクトファイルを再生成（右クリック > Generate Visual Studio project files）

#### コンパイルエラー

1. **"Cannot open include file"**
   - `Build.cs` で必要なモジュールが追加されているか確認
   - インクルードパスが正しいか確認

2. **"Unresolved external symbol"**
   - `Build.cs` で依存モジュールが追加されているか確認
   - リンクエラーの場合、プロジェクトを再生成

3. **"UPROPERTY/UFUNCTION not recognized"**
   - `GENERATED_BODY()` マクロがクラスに含まれているか確認
   - ヘッダーファイル名とクラス名が一致しているか確認

#### Blueprint エラー

1. **"Blueprint could not be loaded"**
   - 参照している C++ クラスがコンパイルされているか確認
   - C++ クラスの関数シグネチャが変更された場合、Blueprint を開き直す

2. **"Function/Variable not found"**
   - `BlueprintCallable`, `BlueprintReadWrite` 等が正しく設定されているか確認
   - C++ コンパイル後、エディタを再起動

---

## まとめ

このドキュメントでは、以下の内容をカバーしました：

1. **既存プラグインの概要** - ScreenTransitionSystem と MermaidMaterial の使い方
2. **プラグイン間の依存関係** - モジュール間の連携方法と依存設定
3. **新規プラグイン作成ガイド** - ゼロからプラグインを作成する手順
4. **プラグインのリンクとデプロイ** - Worktree とシンボリックリンクを使った開発フロー
5. **Blueprint 連携のベストプラクティス** - UPROPERTY/UFUNCTION の使い方
6. **テストとデバッグ** - 単体テスト、構文チェック、デバッグ手法

詳細は各プラグインのドキュメントと `CLAUDE.md` を参照してください。

---

## 関連ドキュメント

- [CLAUDE.md](CLAUDE.md) - プロジェクト全体の開発ガイド
- [ScreenTransitionSystem README](Plugins/ScreenTransitionSystem/README.md)
- [ScreenTransitionSystem Usage](Plugins/ScreenTransitionSystem/Documentation/Usage.md)
- [ScreenTransitionSystem API](Plugins/ScreenTransitionSystem/Documentation/API.md)
- [ScreenTransitionSystem Examples](Plugins/ScreenTransitionSystem/Documentation/Examples.md)
- [scripts/README.md](scripts/README.md) - 開発支援スクリプト
