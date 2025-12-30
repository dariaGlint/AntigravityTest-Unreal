# GitHubラベル体系とラベリングガイドライン

## 目的

プロジェクトのissueを効率的に管理・分類するため、統一されたラベル体系とラベリングルールを提供します。

## ラベル体系

### 1. 技術領域 (Technology) - 青系

| ラベル | 説明 | 色 |
|--------|------|-----|
| `UE5` | UE5のC++/Blueprint機能実装 | `#0075ca` |
| `Plugin` | プラグイン開発 | `#1d76db` |
| `AI` | AI関連（Behavior Tree, EQS, State Tree等） | `#5319e7` |
| `UI` | UI/UX実装（UMG, Slate） | `#0e8a16` |
| `Gameplay` | ゲームプレイロジック | `#006b75` |
| `Python` | Pythonスクリプト関連 | `#fbca04` |
| `CI/CD` | GitHub Actions、ワークフロー | `#d4c5f9` |

### 2. 作業種類 (Type) - 緑/黄/赤系

| ラベル | 説明 | 色 |
|--------|------|-----|
| `documentation` | ドキュメント整備 | `#0075ca` |
| `enhancement` | 既存機能の改善 | `#a2eeef` |
| `bug` | バグ修正 | `#d73a4a` |
| `feature` | 新機能追加 | `#0e8a16` |
| `refactor` | リファクタリング | `#fbca04` |
| `test` | テスト関連 | `#d876e3` |

### 3. 優先度 (Priority) - オレンジ/赤系

| ラベル | 説明 | 色 |
|--------|------|-----|
| `priority: high` | 高優先度 | `#d93f0b` |
| `priority: medium` | 中優先度 | `#fbca04` |
| `priority: low` | 低優先度 | `#c2e0c6` |

### 4. 状態 (Status) - グレー/紫系

| ラベル | 説明 | 色 |
|--------|------|-----|
| `help wanted` | 支援が必要 | `#008672` |
| `good first issue` | 初心者向け | `#7057ff` |
| `blocked` | ブロックされている | `#e99695` |
| `in progress` | 作業中 | `#d4c5f9` |
| `claude-auto` | Claude自動実装対象 | `#bfdadc` |

## ラベリングルール

### 技術領域ラベルの付与基準

#### `UE5` ラベルを付けるケース

- UE5のC++コード実装
- Blueprintの作成・修正
- UE5エンジン機能の使用（Enhanced Input, Gameplay Abilities等）
- **UE5技術情報を含むドキュメント**（例: SETUP.md, ARCHITECTURE.md）

#### `UE5` ラベルを付けないケース

- 純粋なプロセス文書（例: CLAUDE.md, AGENTS.md）
- 一般的なGitHub/CI/CD関連（例: workflow説明）
- プロジェクト管理のみの文書

### 複数ラベルの組み合わせ例

| Issue内容 | ラベル組み合わせ例 |
|-----------|-------------------|
| キャラクターのジャンプ機能追加 | `UE5`, `feature`, `Gameplay`, `priority: high` |
| QuestSystemプラグインのバグ修正 | `Plugin`, `bug`, `priority: high` |
| SETUP.mdの作成 | `documentation`, `UE5`, `priority: high` |
| AGENTS.mdの作成 | `documentation`, `priority: high` |
| GitHub Actionsワークフロー追加 | `CI/CD`, `enhancement`, `priority: medium` |
| Slateインベントリ UI実装 | `UE5`, `UI`, `feature`, `priority: medium` |

## ラベル作成方法

### GitHub CLIを使用する場合

```bash
# 技術領域ラベル
gh label create "UE5" --description "UE5のC++/Blueprint機能実装" --color "0075ca"
gh label create "Plugin" --description "プラグイン開発" --color "1d76db"
gh label create "AI" --description "AI関連（Behavior Tree, EQS, State Tree等）" --color "5319e7"
gh label create "UI" --description "UI/UX実装（UMG, Slate）" --color "0e8a16"
gh label create "Gameplay" --description "ゲームプレイロジック" --color "006b75"
gh label create "Python" --description "Pythonスクリプト関連" --color "fbca04"
gh label create "CI/CD" --description "GitHub Actions、ワークフロー" --color "d4c5f9"

# 作業種類ラベル
gh label create "documentation" --description "ドキュメント整備" --color "0075ca"
gh label create "enhancement" --description "既存機能の改善" --color "a2eeef"
gh label create "bug" --description "バグ修正" --color "d73a4a"
gh label create "feature" --description "新機能追加" --color "0e8a16"
gh label create "refactor" --description "リファクタリング" --color "fbca04"
gh label create "test" --description "テスト関連" --color "d876e3"

# 優先度ラベル
gh label create "priority: high" --description "高優先度" --color "d93f0b"
gh label create "priority: medium" --description "中優先度" --color "fbca04"
gh label create "priority: low" --description "低優先度" --color "c2e0c6"

# 状態ラベル
gh label create "help wanted" --description "支援が必要" --color "008672"
gh label create "good first issue" --description "初心者向け" --color "7057ff"
gh label create "blocked" --description "ブロックされている" --color "e99695"
gh label create "in progress" --description "作業中" --color "d4c5f9"
gh label create "claude-auto" --description "Claude自動実装対象" --color "bfdadc"
```

### GitHubウェブインターフェースを使用する場合

1. リポジトリ → Issues → Labels
2. "New label" をクリック
3. 名前、説明、色を入力
4. "Create label" をクリック

## Issue作成時のガイドライン

新しいissueを作成する際は、以下のポイントを参考にラベルを選択してください:

1. **技術領域**: 主に関連する技術領域を1つ以上選択
2. **作業種類**: 作業の性質を1つ選択（bug, feature, enhancement, etc.）
3. **優先度**: 必要に応じて優先度を設定
4. **状態**: 初期状態では不要。作業開始時や支援が必要な場合に追加

## 参考

このガイドラインの詳細な背景や議論については、[Issue #58](https://github.com/dariaGlint/AntigravityTest-Unreal/issues/58) を参照してください。
