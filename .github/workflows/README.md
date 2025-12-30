# GitHub Actions Workflows

このディレクトリには、プロジェクトで使用するGitHub Actions ワークフローが含まれています。

## 再利用可能なワークフロー (Plugins)

### claude-auto-implement.yml

Claude に自動実装を依頼するための再利用可能なワークフロー。他のワークフローから呼び出すことができます。

#### 機能

- 指定したissueまたはすべてのopen issueに対して、Claudeに実装を依頼するコメントを自動投稿
- PRが既にリンクされているissueはスキップ
- 既にClaude botが反応しているissueはスキップ

#### 使い方

他のワークフローから以下のように呼び出します:

```yaml
jobs:
  my-job:
    uses: ./.github/workflows/claude-auto-implement.yml
    with:
      issue_number: 123  # オプション: 特定のissue番号
      max_comments_per_run: 1  # オプション: 1回の実行で最大何件コメントするか
      comment_message: '@claude 実装して'  # オプション: カスタムメッセージ
    secrets:
      PAT_TOKEN: ${{ secrets.PAT_TOKEN }}
```

#### パラメータ

| パラメータ | 説明 | 必須 | デフォルト |
|-----------|------|------|-----------|
| `issue_number` | コメントする特定のissue番号。指定しない場合は全てのopen issueをチェック | いいえ | なし |
| `max_comments_per_run` | 1回の実行で最大何件のissueにコメントするか | いいえ | 1 |
| `comment_message` | 投稿するコメントメッセージ | いいえ | '@claude pluginで使える形で実装して...' |

#### シークレット

| シークレット | 説明 | 必須 |
|------------|------|------|
| `PAT_TOKEN` | GitHub Personal Access Token。指定しない場合は `GITHUB_TOKEN` を使用 | いいえ |

#### 使用例

詳細な使用例は [example-claude-plugin-usage.yml](./example-claude-plugin-usage.yml) を参照してください。

##### 例1: 特定のissueにコメント

```yaml
jobs:
  comment-on-issue:
    uses: ./.github/workflows/claude-auto-implement.yml
    with:
      issue_number: 46
    secrets:
      PAT_TOKEN: ${{ secrets.PAT_TOKEN }}
```

##### 例2: open issueを自動検出してコメント

```yaml
jobs:
  auto-comment:
    uses: ./.github/workflows/claude-auto-implement.yml
    with:
      max_comments_per_run: 3
    secrets:
      PAT_TOKEN: ${{ secrets.PAT_TOKEN }}
```

##### 例3: カスタムメッセージでコメント

```yaml
jobs:
  custom-comment:
    uses: ./.github/workflows/claude-auto-implement.yml
    with:
      issue_number: 50
      comment_message: '@claude この機能を実装してください。完了したらPRを作成してください。'
    secrets:
      PAT_TOKEN: ${{ secrets.PAT_TOKEN }}
```

## 既存のワークフロー

### auto-comment-issues.yml

1時間ごとにopen issueをチェックし、Claudeに実装を依頼するコメントを自動投稿します。

- **トリガー**: cron (1時間ごと)、手動実行 (workflow_dispatch)
- **使用プラグイン**: claude-auto-implement.yml

### simple-manual-claude-comment.yml

**推奨**: 手動でissueにClaude実装依頼コメントを投稿するワークフロー（最もシンプル）。

- **トリガー**: workflow_dispatch (手動実行のみ)
- **機能**:
  - Issue番号を空欄で実行 → 全てのopen issueをリスト表示
  - Issue番号を入力して実行 → その番号のissueにコメント投稿
- **使い方**:
  1. Actions タブから "Simple Manual Claude Comment" を選択
  2. "Run workflow" をクリック
  3. Issue番号を空欄にして実行 → issueリストを確認
  4. 再度実行して、コメントしたいissue番号を入力
  5. 必要に応じてカスタムメッセージを入力
  6. "Run workflow" をクリック

### manual-claude-comment.yml

手動でissueにClaude実装依頼コメントを投稿するワークフロー（モード選択式）。

- **トリガー**: workflow_dispatch (手動実行のみ)
- **モード**:
  - `list-issues`: 全てのopen issueをリスト表示
  - `comment-on-issue`: 指定したissueにコメント投稿
- **使い方**:
  1. Actions タブから "Manual Claude Comment on Issue" を選択
  2. "Run workflow" をクリック
  3. モードで "list-issues" を選択してissueリストを確認
  4. 再度実行して、モードで "comment-on-issue" を選択
  5. Issue番号とメッセージを入力して実行

### claude.yml

Claudeに関連するワークフロー（詳細は要確認）

### claude-code-review.yml

Claude によるコードレビューを自動化するワークフロー。

- PRが作成されたときに自動的にレビューを実行
- レビュー結果に基づいて自動修正を行う

### example-claude-plugin-usage.yml

`claude-auto-implement.yml` の使用例を示すサンプルワークフロー。

## 開発ガイド

### 新しいワークフローの追加

1. `.github/workflows/` ディレクトリに新しいYAMLファイルを作成
2. ワークフローの定義を記述
3. 必要に応じて、既存の再利用可能なワークフローを活用

### 再利用可能なワークフローの作成

再利用可能なワークフローを作成するには、`workflow_call` トリガーを使用します:

```yaml
name: My Reusable Workflow

on:
  workflow_call:
    inputs:
      my_input:
        description: 'Input description'
        required: true
        type: string
    secrets:
      MY_SECRET:
        description: 'Secret description'
        required: false

jobs:
  my-job:
    runs-on: ubuntu-latest
    steps:
      - name: Do something
        run: echo "${{ inputs.my_input }}"
```

### ベストプラクティス

- ワークフロー名は明確で分かりやすいものにする
- 再利用可能なワークフローは汎用的に設計する
- パラメータにはデフォルト値を設定する
- ドキュメントを充実させる
- シークレットは環境変数として安全に管理する

## トラブルシューティング

### ワークフローが実行されない

- トリガー条件を確認
- リポジトリの設定でActions が有効になっているか確認
- ワークフローファイルの構文エラーをチェック

### ワークフローが失敗する

- Actions タブでログを確認
- 権限設定を確認 (`permissions` セクション)
- シークレットが正しく設定されているか確認

### 再利用可能なワークフローを呼び出せない

- `uses:` のパスが正しいか確認 (`./.github/workflows/workflow-name.yml`)
- 呼び出し元と呼び出し先が同じリポジトリにあるか確認
- `workflow_call` トリガーが定義されているか確認

## 参考リンク

- [GitHub Actions ドキュメント](https://docs.github.com/en/actions)
- [Reusing workflows](https://docs.github.com/en/actions/using-workflows/reusing-workflows)
- [Workflow syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
