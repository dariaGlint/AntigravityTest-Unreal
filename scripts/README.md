# スクリプト集

このディレクトリには、開発を効率化するためのスクリプトが含まれています。

## link-plugin - プラグインのシンボリックリンク作成

Worktreeで開発したプラグインをUE5プロジェクトにシンボリックリンクで接続します。

### quick-link.sh - コマンドライン版（最も簡単）

引数を指定してすぐに実行できるシンプル版です。

**使用方法:**
```bash
# Claude Codeから実行する場合
Bash: scripts/quick-link.sh ScreenTransitionSystem C:/MyUE5Project

# または直接ターミナルから
./scripts/quick-link.sh ScreenTransitionSystem C:/MyUE5Project
```

**Claudeに依頼する例:**
```
「ScreenTransitionSystemをC:/MyUE5Projectにリンクして」
```

これだけでCloudeがシンボリックリンクを作成します。

### 利点

- **リアルタイム反映**: Worktreeでの変更が即座にUE5プロジェクトに反映
- **重複なし**: ファイルをコピーする必要がなく、一箇所で管理
- **簡単な切り替え**: 複数のWorktreeを簡単に切り替え可能

### 使用方法

#### Windows

1. **管理者権限でコマンドプロンプトを開く**
   - `scripts\link-plugin.bat` を右クリック → 「管理者として実行」

2. **プロンプトに従って入力**
   ```
   Enter plugin name: ScreenTransitionSystem
   Enter your UE5 project path: C:\MyUE5Project
   ```

3. **完了**
   - シンボリックリンクが作成され、UE5プロジェクトでプラグインが利用可能になります

#### Linux/Mac

1. **ターミナルでスクリプトを実行**
   ```bash
   cd scripts
   ./link-plugin.sh
   ```

2. **プロンプトに従って入力**
   ```
   Enter plugin name: ScreenTransitionSystem
   Enter your UE5 project path: /home/user/MyUE5Project
   ```

3. **完了**
   - シンボリックリンクが作成されます

### トラブルシューティング

#### Windows: "アクセスが拒否されました"

**原因**: 管理者権限が必要です

**解決方法**:
1. `link-plugin.bat` を右クリック
2. 「管理者として実行」を選択

#### プラグインが見つからない

**原因**: プラグイン名が間違っているか、Worktreeに存在しません

**解決方法**:
1. Worktreeの `Plugins` フォルダを確認
2. 正確なプラグイン名を入力（大文字小文字を区別）

#### 既存のプラグインフォルダがある

**原因**: UE5プロジェクトに既に同名のフォルダが存在します

**解決方法**:
- スクリプトが確認メッセージを表示します
- `y` を入力すると既存のフォルダを削除して新しいシンボリックリンクを作成

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

### 使用例

#### ケース1: 新しいWorktreeでプラグインを開発

```bash
# Worktreeを作成
git worktree add feature-new-transition

# Worktreeに移動
cd feature-new-transation

# プラグインをUE5プロジェクトにリンク
scripts\link-plugin.bat
> ScreenTransitionSystem
> C:\MyUE5Project

# UE5エディタでプラグインを確認
# 変更を加えると即座にUE5に反映される
```

#### ケース2: 複数のプロジェクトで同じプラグインをテスト

```bash
# プロジェクトAにリンク
scripts\link-plugin.bat
> ScreenTransitionSystem
> C:\ProjectA

# プロジェクトBにもリンク
scripts\link-plugin.bat
> ScreenTransitionSystem
> C:\ProjectB

# 両方のプロジェクトで同じコードが使用される
```

#### ケース3: 異なるバージョンのプラグインを比較

```bash
# Worktree1（古いバージョン）
cd worktree1
scripts\link-plugin.bat
> ScreenTransitionSystem
> C:\ProjectOld

# Worktree2（新しいバージョン）
cd worktree2
scripts\link-plugin.bat
> ScreenTransitionSystem
> C:\ProjectNew

# 2つのUE5プロジェクトで異なるバージョンをテスト
```

## 今後追加される可能性のあるスクリプト

- `build-plugin.bat/sh` - プラグインのビルド
- `package-plugin.bat/sh` - プラグインのパッケージング
- `test-plugin.bat/sh` - プラグインのテスト実行
- `clean-plugin.bat/sh` - ビルド成果物のクリーンアップ
