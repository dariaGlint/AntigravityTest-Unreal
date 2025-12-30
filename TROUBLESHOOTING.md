# トラブルシューティングガイド

このドキュメントは、AntigravityTest プロジェクトでよく発生する問題と解決策をまとめています。開発者とClaude Codeが問題を迅速に解決できるように、体系的に整理されています。

## 目次
- [ビルドエラー](#ビルドエラー)
- [プラグイン関連](#プラグイン関連)
- [Blueprint関連](#blueprint関連)
- [Git worktree関連](#git-worktree関連)
- [Enhanced Input関連](#enhanced-input関連)
- [Gameplay Abilities関連](#gameplay-abilities関連)
- [State Tree関連](#state-tree関連)
- [パフォーマンス問題](#パフォーマンス問題)
- [その他](#その他)
- [トラブルシューティングフロー](#トラブルシューティングフロー)

---

## ビルドエラー

### C++コンパイルエラー

#### モジュール依存関係のエラー
**症状**: `Cannot open include file` または `Unresolved external symbol`

**原因**: 必要なモジュールが `.Build.cs` ファイルに追加されていない

**解決方法**:
1. `Source/AntigravityTest/AntigravityTest.Build.cs` を開く
2. `PublicDependencyModuleNames` または `PrivateDependencyModuleNames` に必要なモジュールを追加
   ```csharp
   PublicDependencyModuleNames.AddRange(new string[] {
       "Core",
       "CoreUObject",
       "Engine",
       "AIModule",  // 追加例
       "UMG"        // 追加例
   });
   ```
3. プロジェクトファイルを再生成
4. ビルドを再実行

**よく使うモジュール**:
- `Engine`: 基本的なゲーム機能
- `AIModule`: AI機能 (BehaviorTree, EQS等)
- `UMG`: UIウィジェット
- `Slate`, `SlateCore`: UI基盤
- `InputCore`: 入力処理
- `EnhancedInput`: Enhanced Input System

---

#### ヘッダーファイルが見つからない
**症状**: `fatal error C1083: Cannot open include file: 'XXX.h'`

**原因**: インクルードパスが正しく設定されていない、またはモジュールが追加されていない

**解決方法**:
1. インクルード文が正しいか確認
   ```cpp
   // プロジェクト内のヘッダー
   #include "CombatCharacter.h"

   // エンジンのヘッダー
   #include "GameFramework/Character.h"
   ```
2. `.Build.cs` で `PublicIncludePaths` または `PrivateIncludePaths` を確認
3. 必要なモジュールが `PublicDependencyModuleNames` に追加されているか確認
4. プロジェクトファイルを再生成

---

#### リンクエラー
**症状**: `error LNK2019: unresolved external symbol`

**原因**: API マクロが正しく設定されていない、または依存モジュールが不足

**解決方法**:
1. クラス宣言に API マクロを追加
   ```cpp
   UCLASS()
   class ANTIGRAVITYTEST_API ACombatCharacter : public ACharacter
   {
       GENERATED_BODY()
   };
   ```
2. `.Build.cs` で依存モジュールを確認
3. プロジェクトファイルを再生成
   - `.uproject` を右クリック → `Generate Visual Studio project files`
4. クリーンビルドを実行

---

### プロジェクトファイルが古い

**症状**: "Project files are outdated. Would you like to rebuild them?"

**解決方法**:
1. **方法1**: エディタのプロンプトで `Yes` を選択
2. **方法2**: 手動で再生成
   - `.uproject` ファイルを右クリック
   - `Generate Visual Studio project files` を選択
3. **方法3**: 完全なクリーン再生成
   ```bat
   # 一時ファイルを削除
   rmdir /s /q Intermediate
   rmdir /s /q Binaries

   # プロジェクトファイルを再生成
   右クリック → Generate Visual Studio project files
   ```

---

### ビルド後にエディタが起動しない

**症状**: `LaunchEditor.bat` 実行後にエディタが起動しない

**原因**: Unreal Engine のインストールパスが正しくない

**解決方法**:
1. `LaunchEditor.bat` を開いて、パスを確認
   ```bat
   "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
   ```
2. Unreal Engine 5.7 が正しくインストールされているか確認
3. パスを環境に合わせて修正
4. コマンドプロンプトからログを確認
   ```bat
   LaunchEditor.bat
   # エラーメッセージを確認
   ```

---

## プラグイン関連

### プラグインが認識されない

**症状**: プラグインがエディタのプラグインリストに表示されない

**原因**: `.uplugin` ファイルの構文エラー、または配置場所が間違っている

**解決方法**:
1. `.uplugin` ファイルの構文をチェック
   ```bash
   ./scripts/check-plugin-syntax.sh
   ```
2. プラグインの配置を確認
   ```
   Plugins/
   └── YourPlugin/
       ├── YourPlugin.uplugin
       ├── Source/
       └── Content/
   ```
3. `.uplugin` ファイルのJSON構文を確認
   ```json
   {
       "FileVersion": 3,
       "Version": 1,
       "VersionName": "1.0",
       "FriendlyName": "Your Plugin",
       "Description": "Description",
       "Category": "Other",
       "CreatedBy": "Your Name",
       "CreatedByURL": "",
       "DocsURL": "",
       "MarketplaceURL": "",
       "SupportURL": "",
       "EnabledByDefault": true,
       "CanContainContent": true,
       "IsBetaVersion": false,
       "IsExperimentalVersion": false,
       "Installed": false,
       "Modules": [
           {
               "Name": "YourPlugin",
               "Type": "Runtime",
               "LoadingPhase": "Default"
           }
       ]
   }
   ```
4. エディタを再起動

---

### プラグインのリンクエラー

**症状**: worktree 使用時にプラグインがリンクされない

**原因**: シンボリックリンクが作成されていない

**解決方法**:
1. **高速リンク（推奨）**:
   ```bash
   ./scripts/quick-link.sh ScreenTransitionSystem /path/to/UE5Project
   ```
2. **対話型リンク**:
   ```bash
   ./scripts/link-plugin.sh
   ```
3. **Claude Code スキル**:
   ```
   /link-plugin
   ```
4. **Windows の場合**: 管理者権限で実行
   ```bat
   # 管理者権限でコマンドプロンプトを開く
   scripts\link-plugin.bat
   ```
5. シンボリックリンクの確認
   ```bash
   # Linux/Mac
   ls -la Plugins/

   # Windows
   dir Plugins
   ```

---

### プラグインモジュールのロード失敗

**症状**: "Plugin 'XXX' failed to load because module 'YYY' could not be loaded"

**原因**: プラグインの依存モジュールが不足、または `.uproject` の設定が不正

**解決方法**:
1. プラグインの `.Build.cs` で依存モジュールを確認
   ```csharp
   PublicDependencyModuleNames.AddRange(new string[] {
       "Core",
       "CoreUObject",
       "Engine",
       // 必要なモジュールを追加
   });
   ```
2. プロジェクトの `.uproject` でプラグインが有効化されているか確認
   ```json
   "Plugins": [
       {
           "Name": "YourPlugin",
           "Enabled": true
       }
   ]
   ```
3. プロジェクトファイルを再生成
4. クリーンビルドを実行

---

## Blueprint関連

### Blueprintコンパイルエラー

#### 親クラスのC++変更後のエラー
**症状**: Blueprint を開くと "The blueprint could not be compiled" エラー

**原因**: 親C++クラスの関数シグネチャや変数が変更された

**解決方法**:
1. Blueprint を開く
2. エラーメッセージを確認
3. 変更されたC++の関数/変数に合わせてBlueprintを修正
4. `Compile` ボタンをクリック
5. エラーが解決しない場合、親クラスを再度選択
   - `File` → `Reparent Blueprint`
   - 同じ親クラスを再選択

---

#### 削除された関数/変数への参照
**症状**: Blueprint内に赤いノード（エラーノード）が表示される

**原因**: C++で削除された関数や変数をBlueprintで参照している

**解決方法**:
1. Blueprint を開く
2. 赤いノードを見つける
3. 赤いノードを削除または代替ノードに置き換え
4. `Compile` をクリック
5. 保存

---

### BlueprintからC++関数が見えない

**症状**: C++で定義した関数がBlueprintのノードリストに表示されない

**原因**: `UFUNCTION` マクロが正しく設定されていない

**解決方法**:
1. C++ヘッダーファイルで `UFUNCTION` マクロを確認
   ```cpp
   // Blueprintから呼び出し可能
   UFUNCTION(BlueprintCallable, Category="YourCategory")
   void YourFunction();

   // Blueprintで実装可能
   UFUNCTION(BlueprintImplementableEvent, Category="YourCategory")
   void YourEvent();

   // Blueprint実装可能、C++でもデフォルト実装あり
   UFUNCTION(BlueprintNativeEvent, Category="YourCategory")
   void YourNativeEvent();
   ```
2. 関数が `public` または `protected` であることを確認
3. プロジェクトを再ビルド
   ```bat
   Build.bat
   ```
4. エディタを再起動
5. Blueprintを開き直す

---

## Git worktree関連

### worktreeが作成できない

**症状**: `git worktree add` コマンドがエラーになる

**原因**: ブランチ名の重複、またはパスの問題

**解決方法**:
1. 既存のworktreeを確認
   ```bash
   git worktree list
   ```
2. ブランチ名が重複していないか確認
   ```bash
   git branch -a
   ```
3. パスの区切り文字を確認（Windowsの場合）
   ```bash
   # OK
   git worktree add ../worktree-name branch-name

   # OK (Windows)
   git worktree add ..\worktree-name branch-name
   ```
4. 絶対パスを使用
   ```bash
   git worktree add /full/path/to/worktree branch-name
   ```

---

### worktree削除後もリストに残る

**症状**: `git worktree list` に削除済みのworktreeが表示される

**原因**: worktreeのメタデータが残っている

**解決方法**:
1. worktreeをクリーンアップ
   ```bash
   git worktree prune
   ```
2. 手動で削除（上記で解決しない場合）
   ```bash
   # .git/worktreesディレクトリを確認
   ls -la .git/worktrees/

   # 対象のworktreeディレクトリを削除
   rm -rf .git/worktrees/worktree-name
   ```
3. 再度確認
   ```bash
   git worktree list
   ```

---

### worktree間でプラグインが同期しない

**症状**: あるworktreeでプラグインが認識されるが、別のworktreeでは認識されない

**原因**: シンボリックリンクが各worktreeで個別に必要

**解決方法**:
1. 各worktreeでシンボリックリンクを確認
   ```bash
   cd /path/to/worktree
   ls -la Plugins/
   ```
2. シンボリックリンクを再作成
   ```bash
   ./scripts/link-plugin.sh
   # または
   ./scripts/quick-link.sh PluginName /path/to/UE5Project
   ```
3. エディタを再起動

---

## Enhanced Input関連

### 入力が反応しない

**症状**: キーボード/マウス入力が反応しない

**原因**: Input Mapping Context が設定されていない、または Input Action の設定が不正

**解決方法**:
1. **Input Mapping Context の確認**:
   - `Content/Input/` または各バリアントの `Input/` フォルダを確認
   - 必要な Input Mapping Context (IMC) が存在するか確認

2. **PlayerController での Context 追加を確認**:
   ```cpp
   void AYourPlayerController::BeginPlay()
   {
       Super::BeginPlay();

       if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
           ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
       {
           Subsystem->AddMappingContext(DefaultMappingContext, 0);
       }
   }
   ```

3. **Input Action の設定を確認**:
   - Character クラスで Input Action が正しくバインドされているか確認
   ```cpp
   UPROPERTY(EditAnywhere, Category="Input")
   TObjectPtr<UInputAction> MoveAction;

   void ACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
   {
       if (UEnhancedInputComponent* EnhancedInputComponent =
           Cast<UEnhancedInputComponent>(PlayerInputComponent))
       {
           EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
                                              this, &ACharacter::Move);
       }
   }
   ```

4. **Blueprint での設定確認**:
   - Blueprint の Details パネルで Input Action が設定されているか確認
   - Input Mapping Context が Player Controller に設定されているか確認

5. **プロジェクト設定の確認**:
   - `Edit` → `Project Settings` → `Input`
   - `Default Input Class` が `EnhancedInputComponent` になっているか確認

---

## Gameplay Abilities関連

### Abilityが発動しない

**症状**: Gameplay Ability が発動しない

**原因**: Ability System Component が追加されていない、または Ability が Grant されていない

**解決方法**:
1. **Ability System Component の追加を確認**:
   ```cpp
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
   TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

   // コンストラクタで作成
   AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
   ```

2. **Ability の Grant を確認**:
   ```cpp
   void AYourCharacter::BeginPlay()
   {
       Super::BeginPlay();

       if (AbilitySystemComponent)
       {
           for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
           {
               AbilitySystemComponent->GiveAbility(
                   FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
           }
       }
   }
   ```

3. **Gameplay Tag の設定を確認**:
   - `Project Settings` → `Gameplay Tags`
   - 必要な Gameplay Tag が定義されているか確認
   - Ability で使用する Tag が正しいか確認

4. **Ability の Activation 条件を確認**:
   - `CanActivateAbility` の条件を確認
   - Cost や Cooldown の設定を確認

---

### Abilityのレプリケーションエラー

**症状**: マルチプレイヤーで Ability が正しく動作しない

**原因**: Replication 設定が不正、またはネットワークロールの確認が不足

**解決方法**:
1. **Replication 設定を確認**:
   ```cpp
   UPROPERTY(Replicated)
   TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

   void AYourCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);

       DOREPLIFETIME(AYourCharacter, AbilitySystemComponent);
   }
   ```

2. **ネットワークロールを確認**:
   ```cpp
   if (HasAuthority())
   {
       // サーバー側の処理
   }
   else
   {
       // クライアント側の処理
   }
   ```

3. **Ability System Component の初期化**:
   - サーバーとクライアントの両方で正しく初期化されているか確認

---

## State Tree関連

### State Treeが動作しない

**症状**: State Tree が実行されない

**原因**: State Tree Component が追加されていない、または State Tree Asset の Binding が不正

**解決方法**:
1. **State Tree Component の追加を確認**:
   ```cpp
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StateTree")
   TObjectPtr<UStateTreeComponent> StateTreeComponent;

   // コンストラクタで作成
   StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
   ```

2. **State Tree Asset の Binding を確認**:
   - State Tree エディタを開く
   - Parameters タブで必要なパラメータがバインドされているか確認
   - 各 State で Condition と Task が正しく設定されているか確認

3. **ログでエラーメッセージを確認**:
   ```
   Saved/Logs/AntigravityTest.log
   ```
   - State Tree 関連のエラーやワーニングを検索

4. **State Tree の開始を確認**:
   ```cpp
   void AYourAIController::BeginPlay()
   {
       Super::BeginPlay();

       if (StateTreeComponent && StateTreeAsset)
       {
           StateTreeComponent->SetStateTree(StateTreeAsset);
           StateTreeComponent->StartLogic();
       }
   }
   ```

---

## パフォーマンス問題

### エディタが重い

**症状**: Unreal Editor の動作が遅い

**原因**: キャッシュやシェーダーの問題

**解決方法**:
1. **Derived Data Cache を削除**:
   ```bat
   rmdir /s /q DerivedDataCache
   ```
   - 次回起動時に再生成されます

2. **Saved フォルダをクリア**:
   ```bat
   # ログは残して他を削除
   cd Saved
   rmdir /s /q Config
   rmdir /s /q Screenshots
   ```

3. **Shader 再コンパイル**:
   - エディタで `Window` → `Developer Tools` → `Shader Complexity`
   - `Edit` → `Project Settings` → `Rendering`
   - `r.ShaderDevelopmentMode=1` を有効化

4. **エディタの設定を確認**:
   - `Edit` → `Editor Preferences` → `Performance`
   - `Use Less CPU when in Background` を有効化
   - `Monitor Editor Performance` を確認

---

### ビルドが遅い

**症状**: C++ プロジェクトのビルドに時間がかかる

**原因**: Incremental build が無効、または PCH の問題

**解決方法**:
1. **Incremental build の確認**:
   - Visual Studio で `Configuration Properties` → `General`
   - `Use Incremental Linking` が `Yes` になっているか確認

2. **PCH (Precompiled Header) の使用を確認**:
   - `.Build.cs` ファイルで以下を確認
   ```csharp
   PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
   ```

3. **並列ビルドの設定**:
   - Visual Studio で `Tools` → `Options` → `Projects and Solutions` → `Build and Run`
   - `maximum number of parallel project builds` を CPU コア数に設定

4. **不要なモジュールを削除**:
   - 使用していないモジュールを `.Build.cs` から削除

---

## その他

### エディタクラッシュ

**症状**: Unreal Editor がクラッシュする

**解決方法**:
1. **ログを確認**:
   ```
   Saved/Logs/AntigravityTest.log
   ```
   - クラッシュ直前のログメッセージを確認

2. **Crash Reporter のログを確認**:
   ```
   Saved/Crashes/
   ```
   - 最新のクラッシュレポートを開く
   - Call Stack を確認してクラッシュ原因を特定

3. **Intermediate フォルダを削除後の再ビルド**:
   ```bat
   rmdir /s /q Intermediate
   rmdir /s /q Binaries
   # プロジェクトファイルを再生成
   # ビルドを実行
   ```

4. **プラグインを無効化して切り分け**:
   - `.uproject` を開く
   - 疑わしいプラグインを `"Enabled": false` に設定
   - エディタを再起動

5. **グラフィックスドライバの更新**:
   - GPU のドライバを最新版に更新

---

### Pythonスクリプトエラー (MermaidMaterial)

**症状**: MermaidMaterial プラグインの Python スクリプトがエラーになる

**原因**: Python 環境の問題、または依存ライブラリの不足

**解決方法**:
1. **Python 環境の確認**:
   - Unreal Editor が使用する Python のバージョンを確認
   - `Edit` → `Project Settings` → `Plugins` → `Python`

2. **依存ライブラリのインストール**:
   ```bash
   # Unreal が使用する Python で pip install
   python -m pip install <required-library>
   ```

3. **スクリプトのパスを確認**:
   - `Plugins/MermaidMaterial/Content/Python/` にスクリプトが存在するか確認
   - スクリプトのパーミッションを確認 (Linux/Mac)

4. **ログでエラーメッセージを確認**:
   ```
   Saved/Logs/AntigravityTest.log
   ```
   - Python 関連のエラーを検索

---

## トラブルシューティングフロー

問題が発生した場合、以下のフローに従って解決を試みてください。

```
問題発生
  ↓
1. ログ確認 (Saved/Logs/)
  ├─ エラーメッセージを特定
  └─ クラッシュの場合は Saved/Crashes/ も確認
  ↓
2. Intermediate/ と Binaries/ 削除
  ├─ rmdir /s /q Intermediate
  └─ rmdir /s /q Binaries
  ↓
3. プロジェクトファイル再生成
  └─ .uproject 右クリック → Generate Visual Studio project files
  ↓
4. クリーンビルド
  ├─ Visual Studio で "Clean Solution"
  └─ "Rebuild Solution"
  ↓
5. エディタで動作確認
  ├─ 問題が解決したか確認
  └─ 解決しない場合は次のステップへ
  ↓
6. それでも解決しない
  ├─ このドキュメントで該当する問題を検索
  ├─ CLAUDE.md を確認
  └─ GitHub Issue を作成
```

---

## 追加リソース

### 参考ドキュメント
- [CLAUDE.md](./CLAUDE.md) - プロジェクトの開発ガイド
- [Unreal Engine 5.7 Documentation](https://docs.unrealengine.com/5.7/)
- [Epic C++ Coding Standard](https://docs.unrealengine.com/5.7/epic-cplusplus-coding-standard-for-unreal-engine/)

### スクリプト
- `scripts/link-plugin.sh` - プラグインリンクスクリプト
- `scripts/quick-link.sh` - プラグイン高速リンク
- `scripts/check-plugin-syntax.sh` - プラグイン構文チェック
- `scripts/test-build.bat` - ビルドテスト

### ログの場所
- **エディタログ**: `Saved/Logs/AntigravityTest.log`
- **クラッシュレポート**: `Saved/Crashes/`
- **ビルドログ**: Visual Studio の Output ウィンドウ

---

**最終更新**: 2025-12-30
