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

## analyze-blueprints - Blueprint分析ツール

Unreal EngineプロジェクトのBlueprintアセットを分析し、命名規則、フォルダ構造、ベストプラクティスをチェックします。

### ⚠️ 現在の状態：Phase 1のみ実装済み

**Phase 1実装（現在利用可能）:**
- Blueprintファイルの命名規則チェック（BP_、WBP_などのプレフィックス）
- フォルダ構造の整理状態確認
- Blueprint一覧とカテゴリ分類
- 基本的な統計情報（ファイル数、種類別集計、バリアント別集計）
- 改善提案レポート

**Phase 2-3（未実装）:**
- Unreal Python API統合による詳細分析
- ノード数、関数数の測定
- イベントグラフの複雑度分析
- パフォーマンス分析（Tick使用検出、重い処理の検出）

### 使用方法

#### Windows
```bat
scripts\analyze-blueprints.bat
```

#### Linux/Mac
```bash
./scripts/analyze-blueprints.sh
```

#### オプション

```bash
# 全体分析（デフォルト）
./scripts/analyze-blueprints.sh

# 特定のフォルダのみ分析
./scripts/analyze-blueprints.sh --path Content/Variant_Combat/Blueprints

# Mermaid図も生成
./scripts/analyze-blueprints.sh --diagram

# ヘルプ表示
./scripts/analyze-blueprints.sh --help
```

### 出力例

```
═══════════════════════════════════════════════════════
  Blueprint Analysis Report
═══════════════════════════════════════════════════════

Scanning for Blueprint assets...
Found 150 assets

─────────────────────────────────────────────────────
1. Asset Statistics by Type
─────────────────────────────────────────────────────

Blueprint Classes (BP_*):     45
Widget Blueprints (WBP_*):    12
Materials (M_*):              30
Material Instances (MI_*):    25
Input Actions (IA_*):         10
Other/Unknown:                28

─────────────────────────────────────────────────────
2. Assets by Variant
─────────────────────────────────────────────────────

Variant_Combat:               80
Variant_SideScrolling:        20
Variant_Platforming:          15
Core/Shared:                  35
```

### Claude Codeから実行

```
「Blueprintを分析して」
```

これでClaude Codeが自動的に分析を実行し、レポートを表示します。

詳細は `.claude/commands/analyze-blueprints.md` を参照してください。

## ue-asset-analyzer - アセット分析・最適化ツール

Unreal Engineプロジェクトのアセットを包括的に分析し、未使用アセットの検出、サイズ最適化、依存関係の可視化を行います。

### 主な機能

1. **未使用アセット検出**
   - 参照されていないアセットの特定
   - プロジェクトサイズの削減可能容量を計算
   - 削除可能なアセットをリスト化

2. **アセットサイズ分析**
   - 大容量アセットのランキング
   - タイプ別（テクスチャ、メッシュ、オーディオ等）のサイズ統計
   - フォルダ別のサイズ内訳

3. **依存関係分析**
   - アセット間の参照関係をマッピング
   - 循環参照の検出
   - リダイレクタの検出

4. **最適化提案**
   - テクスチャ圧縮設定の推奨
   - マテリアルインスタンス化の提案
   - LOD設定の推奨

### 使用方法

#### 基本的な使用

```bash
# 全体分析
./scripts/ue-asset-analyzer.sh

# 未使用アセット検出
./scripts/ue-asset-analyzer.sh --type unused --show-sizes

# サイズ分析
./scripts/ue-asset-analyzer.sh --type size

# 特定フォルダのみ分析
./scripts/ue-asset-analyzer.sh --path Content/Variant_Combat
```

#### 最適化レポート生成

```bash
# 最適化提案を含むレポート生成
./scripts/ue-asset-analyzer.sh --optimize --output report.md

# 大きな未使用アセットを検出（10MB以上）
./scripts/ue-asset-analyzer.sh --type unused --size-threshold 10
```

#### CI/CD統合

```bash
# 未使用アセットが見つかったら失敗
./scripts/ue-asset-analyzer.sh --fail-on-waste

# 無駄が100MBを超えたら失敗
./scripts/ue-asset-analyzer.sh --fail-threshold 100
```

### Windows版

```bat
scripts\ue-asset-analyzer.bat
```

Windows版は自動的にGit Bashを検出し、完全版を実行します。Git Bashがない場合は基本的な分析のみ行います。

### 出力例

```
═══════════════════════════════════════════════════════
  UE Asset Analyzer v1.0.0
═══════════════════════════════════════════════════════

Scanning for assets...
Found 234 assets

─────────────────────────────────────────────────────
Unused Asset Analysis
─────────────────────────────────────────────────────

Found 15 unused assets
Potential savings: 124.5 MB

High Priority (Large unused assets):
  ✗ Content/TestAssets/T_Debug_4K.uasset (45.2 MB)
  ✗ Content/OldAssets/SK_Character_V1.uasset (32.1 MB)

─────────────────────────────────────────────────────
Asset Size Analysis
─────────────────────────────────────────────────────

Total Content Size: 2.4 GB

By Asset Type:
  Textures:              1.2 GB (50.0%)  - 89 assets
  Static Meshes:         680 MB (28.3%)  - 45 assets
  Skeletal Meshes:       320 MB (13.3%)  - 12 assets
  Audio:                 150 MB (6.3%)   - 34 assets
  Other:                 50 MB (2.1%)    - 54 assets

Top 10 Largest Assets:
  1. T_Environment_Atlas_4K.uasset          120.5 MB
  2. SK_MainCharacter.uasset                 85.2 MB
  ...
```

### Claude Codeから実行

```
「未使用アセットを見つけて」
「プロジェクトサイズを最適化したい」
「Content/Charactersフォルダのアセットを分析して」
```

### 設定ファイル

`Config/AssetAnalyzer.ini` で動作をカスタマイズできます：

```ini
[AssetAnalyzer]
LargeAssetThreshold=10.0
WasteThreshold=100.0
TextureMaxResolution=2048

[TextureOptimization]
DefaultCompression=DXT5
NormalMapCompression=BC5
```

### 制限事項

現在の実装（Phase 1）はファイルシステムベースの分析です：

- ランタイムで動的にロードされるアセットの検出は限定的
- バイナリファイルの参照検出のため、一部の参照を見逃す可能性
- Blueprint内部のノード分析は未対応

より正確な分析には、Unreal EditorのReference ViewerやSize Mapツールを併用してください。

詳細は `.claude/commands/ue-asset-analyzer.md` を参照してください。

## 今後追加される可能性のあるスクリプト

- `build-plugin.bat/sh` - プラグインのビルド
- `package-plugin.bat/sh` - プラグインのパッケージング
- `test-plugin.bat/sh` - プラグインのテスト実行
- `clean-plugin.bat/sh` - ビルド成果物のクリーンアップ
