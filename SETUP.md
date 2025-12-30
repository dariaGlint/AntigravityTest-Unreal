# AntigravityTest - 開発環境セットアップガイド

このガイドでは、AntigravityTestプロジェクトの開発環境をセットアップする手順を説明します。新しい開発者やClaude Codeがプロジェクト環境を迅速に理解・構築できるようにすることを目的としています。

## 必要な環境

### Unreal Engine 5.7のインストール

1. **Epic Games Launcherのインストール**
   - [Epic Games Launcher](https://www.epicgames.com/store/ja/download)をダウンロードしてインストール

2. **Unreal Engine 5.7のインストール**
   - Epic Games Launcherを起動
   - 「Unreal Engine」タブを選択
   - 「ライブラリ」→「エンジンバージョン」→「+」ボタンをクリック
   - バージョン5.7を選択してインストール
   - デフォルトインストール場所: `C:\Program Files\Epic Games\UE_5.7`

3. **カスタムパスの場合の設定**
   - Unreal Engineを別の場所にインストールした場合は、以下のファイルを修正:
     - `Build.bat`: UnrealBuildToolのパスを修正
     - `LaunchEditor.bat`: UnrealEditorのパスを修正

   例:
   ```bat
   REM デフォルト
   "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"

   REM カスタムパス
   "D:\UnrealEngine\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
   ```

### IDE / エディタの推奨設定

#### Visual Studio (推奨)

1. **必要なワークロード**
   - C++によるデスクトップ開発
   - C++によるゲーム開発

2. **推奨拡張機能**
   - Visual Assist（オプション、コード補完強化）
   - UnrealVS（Unreal Engine統合）

3. **Visual Studio設定**
   - ソリューションプラットフォーム: `Win64`
   - ソリューション構成: `Development Editor`

#### JetBrains Rider

1. **必要なプラグイン**
   - Unreal Engine Support（自動的にインストールされます）

2. **Riderでプロジェクトを開く**
   - `.sln`ファイルまたは`.uproject`ファイルを開く
   - 初回起動時にUnreal Engineのパスを設定

### Git

- **Git for Windows**: [公式サイト](https://git-scm.com/)からインストール
- **推奨設定**:
  ```bash
  git config --global core.autocrlf true
  git config --global core.longpaths true  # 長いパス名のサポート
  ```

## プロジェクトのセットアップ

### 1. リポジトリのクローン

```bash
git clone https://github.com/dariaGlint/AntigravityTest-Unreal.git
cd AntigravityTest-Unreal
```

### 2. プロジェクトファイルの生成

Windowsの場合:
1. `AntigravityTest.uproject`を右クリック
2. 「Generate Visual Studio project files」を選択

または、コマンドラインから:
```bat
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="%CD%\AntigravityTest.uproject" -game -rocket -progress
```

### 3. 初回ビルド手順

```bat
# プロジェクトをビルド
.\Build.bat

# エディタを起動
.\LaunchEditor.bat
```

**注意**: 初回ビルドは時間がかかります（10〜30分程度）。

## 必須プラグイン

### エンジンプラグイン

以下のプラグインが `.uproject` で有効化されています:

| プラグイン名 | 説明 | タイプ |
|------------|------|--------|
| **StateTree** | State Tree AIシステム | Runtime |
| **GameplayStateTree** | ゲームプレイ用State Tree拡張 | Runtime |
| **GameplayAbilities** | Gameplay Ability System (GAS) | Runtime |
| **AIAssistant** | AI開発支援 | Editor |
| **ModelingToolsEditorMode** | モデリングツール | Editor only |

これらのプラグインは自動的に有効化されます。追加のインストール作業は不要です。

### カスタムプラグイン

プロジェクトには以下のカスタムプラグインが含まれています:

#### ScreenTransitionSystem
- **場所**: `Plugins/ScreenTransitionSystem/`
- **機能**: UI画面遷移システム（非同期ロード、トランジションエフェクト、画面スタック管理）
- **モジュールタイプ**: Runtime
- **ドキュメント**: `Plugins/ScreenTransitionSystem/Documentation/`

#### MermaidMaterial
- **場所**: `Plugins/MermaidMaterial/`
- **機能**: Pythonスクリプトによるマテリアル生成
- **モジュールタイプ**: Runtime
- **スクリプト**: `Plugins/MermaidMaterial/Content/Python/`

## 使用モジュール

プロジェクトの `.Build.cs` ファイルで使用している主要モジュール:

### Core モジュール

| モジュール名 | 説明 | 用途 |
|------------|------|------|
| **Core** | Unreal Engine基本機能 | 必須 |
| **CoreUObject** | UObjectシステム | 必須 |
| **Engine** | ゲームエンジン機能 | 必須 |

### 入力・UI モジュール

| モジュール名 | 説明 | 用途 |
|------------|------|------|
| **EnhancedInput** | Enhanced Input System | 入力処理 |
| **UMG** | Unreal Motion Graphics | UIウィジェット |
| **Slate** | Slateフレームワーク | UI基盤 |
| **SlateCore** | Slateコアシステム | UI基盤 |

### AI・ゲームプレイ モジュール

| モジュール名 | 説明 | 用途 |
|------------|------|------|
| **AIModule** | AI機能（BehaviorTree, EQS等） | AI実装 |
| **StateTreeModule** | State Tree システム | State Tree AI |
| **GameplayAbilities** | Gameplay Ability System | アビリティシステム |

### 追加モジュール

新しいモジュールを追加する場合は、`Source/AntigravityTest/AntigravityTest.Build.cs`を編集:

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "YourNewModule"  // ここに追加
});
```

## scriptsディレクトリの活用

プロジェクトルートの `scripts/` ディレクトリには、開発を支援するスクリプトが含まれています。

### プラグイン関連

#### link-plugin.bat/sh
プラグインのシンボリックリンクを作成します（対話型）。

**Windows**:
```bat
scripts\link-plugin.bat
```

**Linux/Mac**:
```bash
./scripts/link-plugin.sh
```

使用例:
```
プラグイン名: ScreenTransitionSystem
リンク先プロジェクトパス: C:\Projects\MyUE5Project
```

#### quick-link.sh
プラグインを高速でリンクします（非対話型、推奨）。

```bash
./scripts/quick-link.sh ScreenTransitionSystem /path/to/UE5Project
```

#### check-plugin-syntax.sh
プラグインの構文をチェックします。

```bash
./scripts/check-plugin-syntax.sh ScreenTransitionSystem
```

### ビルド関連

#### test-build.bat
プロジェクトのビルドをテストします。

```bat
scripts\test-build.bat
```

### その他

#### apply-labels-to-issues.bat/sh
GitHubのissueにラベルを適用します。

```bash
./scripts/apply-labels-to-issues.sh
```

詳細は `scripts/README.md` を参照してください。

## Git worktreeの使い方

Git worktreeを使用すると、同じリポジトリの異なるブランチを別々のディレクトリで作業できます。

### worktreeの概念

- **メインworktree**: `git clone`で作成した元のディレクトリ
- **追加worktree**: 別のブランチ用に作成した追加のディレクトリ

### worktreeの作成

```bash
# 新しいブランチでworktreeを作成
git worktree add ../AntigravityTest-feature feature/my-feature

# 既存のブランチでworktreeを作成
git worktree add ../AntigravityTest-bugfix bugfix/fix-123
```

### worktreeの確認

```bash
# 現在のworktree一覧を表示
git worktree list
```

出力例:
```
/home/user/AntigravityTest-Unreal         2f180c0 [main]
/home/user/AntigravityTest-feature        a1b2c3d [feature/my-feature]
/home/user/AntigravityTest-bugfix         4e5f6g7 [bugfix/fix-123]
```

### worktreeの削除

```bash
# worktreeを削除（ディレクトリも削除）
git worktree remove ../AntigravityTest-feature

# または、手動でディレクトリを削除してから
git worktree prune
```

### worktreeのベストプラクティス

1. **ブランチごとにworktreeを作成**: 複数の機能を並行開発する場合に便利
2. **ビルドの独立性**: 各worktreeで個別にビルドされるため、干渉しない
3. **エディタの注意**: 同じプロジェクトの複数のworktreeを同時にUnreal Editorで開かない

### /worktree-feature コマンド（Claude Code）

Claude Codeを使用している場合、以下のコマンドでworktreeを簡単に作成できます:

```
/worktree-feature my-feature-name
```

これにより、`feature/my-feature-name`ブランチが作成され、新しいworktreeが自動的にセットアップされます。

## トラブルシューティング（基本）

### "プロジェクトファイルが古い" エラー

**症状**: Unreal Editorを起動すると「プロジェクトファイルが古い」というメッセージが表示される

**解決方法**:
1. エディタを閉じる
2. `.uproject`ファイルを右クリック
3. 「Generate Visual Studio project files」を選択
4. 再度エディタを起動

### ビルドエラー時の対処

**症状**: ビルドが失敗する、または謎のエラーが発生する

**解決方法1: Intermediateフォルダの削除**
```bat
# Intermediateフォルダを削除
rmdir /s /q Intermediate

# プロジェクトファイルを再生成
# .uprojectを右クリック → Generate Visual Studio project files

# 再ビルド
.\Build.bat
```

**解決方法2: Binariesフォルダも削除（より徹底的）**
```bat
# IntermediateとBinariesを削除
rmdir /s /q Intermediate
rmdir /s /q Binaries

# プロジェクトファイルを再生成
# .uprojectを右クリック → Generate Visual Studio project files

# 再ビルド
.\Build.bat
```

### プラグインが認識されない場合

**症状**: プラグインが有効化されない、またはエディタに表示されない

**解決方法**:
1. `.uplugin`ファイルが存在するか確認
   ```bash
   ls Plugins/ScreenTransitionSystem/ScreenTransitionSystem.uplugin
   ```

2. `.uproject`ファイルでプラグインが有効化されているか確認
   ```json
   {
       "Plugins": [
           {
               "Name": "ScreenTransitionSystem",
               "Enabled": true
           }
       ]
   }
   ```

3. エディタを再起動

4. それでも認識されない場合は、プラグインを再ビルド:
   ```bat
   .\Build.bat
   ```

### シンボリックリンクが作成されない（Windows）

**症状**: `link-plugin.bat`を実行してもシンボリックリンクが作成されない

**解決方法**:
1. 管理者権限でコマンドプロンプトを起動
2. スクリプトを再実行:
   ```bat
   scripts\link-plugin.bat
   ```

**注意**: Windowsでシンボリックリンクを作成するには管理者権限が必要です。

### ビルドが遅い

**症状**: ビルドに非常に時間がかかる

**解決方法**:
1. **Incremental Buildを有効化**: 通常は自動的に有効
2. **Visual Studio設定**:
   - ツール → オプション → プロジェクトおよびソリューション
   - 「並列プロジェクトのビルド数」を増やす（CPUコア数に応じて）
3. **SSDを使用**: プロジェクトをSSDに配置
4. **ウイルス対策ソフトの除外設定**: プロジェクトフォルダとUnreal Engineフォルダを除外

### Git関連のエラー

**症状**: Gitで長いパス名のエラーが発生する

**解決方法**:
```bash
git config --global core.longpaths true
```

**症状**: 改行コードの警告が出る

**解決方法**:
```bash
git config --global core.autocrlf true
```

## 次のステップ

セットアップが完了したら:

1. **CLAUDE.mdを読む**: プロジェクトの構造とコーディング規約を理解
2. **サンプルマップを開く**: `Content/Variant_Combat/Maps/`のマップを開いて動作確認
3. **ビルドとプレイ**: エディタでプレイボタンを押して動作確認
4. **テストを実行**: `Source/AntigravityTest/Tests/`のテストを確認

## 参考資料

- [CLAUDE.md](./CLAUDE.md) - プロジェクト詳細情報とコーディング規約
- [scripts/README.md](./scripts/README.md) - スクリプトの詳細な使用方法
- [Unreal Engine 5.7 Documentation](https://docs.unrealengine.com/5.7/)
- [Git Worktree Documentation](https://git-scm.com/docs/git-worktree)

## サポート

問題が発生した場合:
1. このドキュメントのトラブルシューティングセクションを確認
2. CLAUDE.mdのトラブルシューティングセクションを確認
3. GitHubのIssuesで報告

Happy Coding!
