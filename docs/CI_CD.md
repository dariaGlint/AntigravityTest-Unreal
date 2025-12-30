# CI/CDワークフローガイド

このディレクトリには、リポジトリの自動化を実現するGitHub Actionsワークフローが含まれています。

## 目次
- [ワークフロー概要](#ワークフロー概要)
  - [claude.yml](#claudeyml)
  - [claude-code-review.yml](#claude-code-reviewyml)
  - [auto-comment-issues.yml](#auto-comment-issuesyml)
- [GitHub Actionsの基本](#github-actionsの基本)
- [必要なシークレット設定](#必要なシークレット設定)
- [ワークフローのカスタマイズ](#ワークフローのカスタマイズ)
- [Claude Codeとの連携](#claude-codeとの連携)
- [トラブルシューティング](#トラブルシューティング)
- [ベストプラクティス](#ベストプラクティス)

---

## ワークフロー概要

### claude.yml

#### 概要と目的
Claudeによるコード実装とIssue対応を自動化するワークフローです。IssueやPull Requestのコメントで`@claude`をメンションすることで、Claudeが自動的にタスクを実行します。

#### トリガー条件
以下のイベントで実行されます：

- **issue_comment (created)**: Issueにコメントが作成されたとき
- **pull_request_review_comment (created)**: Pull Requestのレビューコメントが作成されたとき
- **issues (opened, assigned)**: Issueが開かれた、または割り当てられたとき
- **pull_request_review (submitted)**: Pull Requestレビューが提出されたとき

ただし、実際に実行されるのは以下の条件を満たす場合のみ：
- コメント本文に`@claude`が含まれている
- Issueのタイトルまたは本文に`@claude`が含まれている

#### 実行内容
1. リポジトリをチェックアウト（浅いクローン: `fetch-depth: 1`）
2. Claude Code Actionを実行
3. 指定された指示に従ってコードを実装、Issue対応、PR作成などを実行

#### 使用している環境変数/シークレット
- `CLAUDE_CODE_OAUTH_TOKEN`: Claude Code APIの認証トークン（**必須**）

#### Permissions（権限）
- `contents: write` - コードの読み書き
- `pull-requests: write` - Pull Requestの作成・更新
- `issues: write` - Issueへのコメント
- `id-token: write` - OIDC認証
- `actions: read` - CI結果の読み取り（PRでのCI結果確認用）

#### Claude Args（カスタム設定）
```yaml
claude_args: '--allowed-tools Bash(gh pr:*)'
```
- `gh pr`コマンドの使用を許可（PR作成などに使用）

#### 期待される動作
1. ユーザーが`@claude`をメンションしてタスクを依頼
2. Claudeが自動的にタスクを理解し実装を開始
3. 必要に応じて新しいブランチを作成
4. コードを実装し、コミット・プッシュ
5. Pull Requestを自動作成（`gh pr create`を使用）
6. コメントで進捗と結果を報告

---

### claude-code-review.yml

#### 概要と目的
Claudeが作成したPull Requestを自動的にレビューし、コード品質を保証するワークフローです。レビュー結果に応じて自動修正を促し、品質の高いコードをmainブランチにマージできるようにします。

#### コードレビュー自動化の仕組み
1. Claudeが作成したPR（`claude/`で始まるブランチ）が対象
2. PRが作成または更新されると自動的にレビューを実行
3. 以下の観点でコードをチェック：
   - コード品質とベストプラクティス
   - 潜在的なバグや問題
   - パフォーマンスの考慮
   - セキュリティ上の懸念
   - UE5プラグインとしての実装の正しさ

#### トリガー条件
```yaml
on:
  pull_request:
    types: [opened, synchronize]
```

- **opened**: Pull Requestが新規作成されたとき
- **synchronize**: Pull Requestに新しいコミットがプッシュされたとき

ただし、`claude/`で始まるブランチのPRのみが対象：
```yaml
if: startsWith(github.event.pull_request.head.ref, 'claude/')
```

#### レビュー結果の確認方法
1. Pull Requestのコメント欄を確認
2. レビュー結果は以下のいずれかの形式で投稿されます：
   - **問題がある場合**: `@claude 以下の問題を修正してください：` + 問題点のリスト
   - **問題がない場合**: `レビュー完了: 問題ありません。マージ可能です。`

#### 修正ループの動作

##### "Enable automated review-fix loop for Claude PRs"機能
この機能により、レビューで指摘された問題を自動的に修正する循環プロセスが実現されます：

1. **レビュー実行**: PRが作成/更新されると自動レビューが開始
2. **問題検出**: レビューで問題が見つかった場合、`@claude 以下の問題を修正してください：`というコメントが投稿される
3. **自動修正**: `claude.yml`ワークフローがこのコメントを検知し、Claudeが問題を修正
4. **再レビュー**: 修正コミットがプッシュされると再度レビューが実行される（synchronizeイベント）
5. **循環**: 問題がなくなるまで2-4を繰り返す

##### 無限ループ防止
レビュー回数の上限を設けています：
```yaml
まず `gh pr view ${{ github.event.pull_request.number }} --comments` でコメントを確認し、
既に「@claude 以下の問題を修正してください」というコメントが3回以上ある場合は、
レビューをスキップして「レビュー回数上限に達しました。手動確認をお願いします。」とコメント
```

3回の自動修正試行後、人間による手動確認が必要になります。

#### Permissions（権限）
- `contents: read` - コードの読み取り
- `pull-requests: write` - PRへのコメント
- `issues: write` - コメント投稿
- `id-token: write` - OIDC認証

#### Claude Args（カスタム設定）
```yaml
claude_args: '--allowed-tools "Bash(gh issue view:*),Bash(gh search:*),Bash(gh issue list:*),Bash(gh pr comment:*),Bash(gh pr diff:*),Bash(gh pr view:*),Bash(gh pr list:*)"'
```
レビューに必要な`gh`コマンド群を許可しています。

#### 環境変数
```yaml
env:
  GH_TOKEN: ${{ secrets.PAT_TOKEN }}
```
- `PAT_TOKEN`: Personal Access Token（PRへのコメント権限が必要）

---

### auto-comment-issues.yml

#### 概要と目的
新しいIssueに自動的にコメントを投稿し、Claudeによる実装を促すワークフローです。Issueが作成されると定期的にチェックし、まだ対応されていないIssueに対して自動的に`@claude`をメンションします。

#### Issue自動コメントの仕組み
1. **定期実行**: 1時間ごとに全てのオープンなIssueをチェック
2. **条件判定**: 以下の条件を満たすIssueにコメント：
   - Pull Requestではないこと
   - 紐付けられたPRが存在しないこと
   - 既に自動コメントがない、またはClaudeからの応答がないこと
3. **コメント投稿**: `@claude pluginで使える形で実装して。実装が終わったら'gh pr create'コマンドを使ってPullRequestをmainブランチに対して自動作成して。手動リンクは不要です。`
4. **制限**: 1回の実行で最大1つのIssueにのみコメント（サーバー負荷軽減のため）

#### トリガー条件
```yaml
on:
  schedule:
    - cron: '0 */1 * * *'  # 1時間ごと（UTC時間）
  workflow_dispatch:  # 手動実行も可能
```

- **schedule**: 定期実行（1時間ごと）
- **workflow_dispatch**: GitHub Actionsのページから手動で実行可能

#### カスタマイズ方法

##### コメント内容の変更
`script`セクションの`body`を編集：
```javascript
body: '@claude pluginで使える形で実装して。実装が終わったら`gh pr create`コマンドを使ってPullRequestをmainブランチに対して自動作成して。手動リンクは不要です。'
```

##### 実行頻度の変更
`cron`式を編集：
```yaml
# 例: 12時間ごとに変更
- cron: '0 */12 * * *'

# 例: 毎日午前9時（UTC）
- cron: '0 9 * * *'
```

##### コメント数の制限変更
`maxCommentsPerRun`の値を変更：
```javascript
const maxCommentsPerRun = 1; // 任意の数に変更可能
```

##### 検出するClaudeボットの種類を変更
`isClaudeBot`の条件を編集：
```javascript
const isClaudeBot = comment.user.login === 'claude' ||
                   comment.user.login === 'claude[bot]' ||
                   comment.user.login === 'claude-ai[bot]' ||
                   comment.user.login === 'anthropics-claude[bot]';
```

#### Permissions（権限）
- `issues: write` - Issueへのコメント投稿
- `contents: write` - リポジトリコンテンツへのアクセス

#### 環境変数/シークレット
```yaml
github-token: ${{ secrets.PAT_TOKEN || secrets.GITHUB_TOKEN }}
```
- `PAT_TOKEN`: Personal Access Token（推奨）
- `GITHUB_TOKEN`: GitHubが自動提供するトークン（フォールバック）

---

## GitHub Actionsの基本

### ワークフローファイルの構造

GitHub Actionsのワークフローファイルは以下の基本構造を持ちます：

```yaml
name: ワークフロー名

on:
  # トリガー定義

jobs:
  job-name:
    runs-on: ubuntu-latest
    permissions:
      # 権限設定
    steps:
      - name: ステップ名
        uses: action-name@version
        with:
          # パラメータ
```

#### 主要な構成要素
- **name**: ワークフローの表示名
- **on**: トリガー条件
- **jobs**: 実行するジョブの定義
- **runs-on**: 実行環境（ubuntu-latest, windows-latest, macos-latestなど）
- **permissions**: ワークフローに付与する権限
- **steps**: 実行する個別のステップ

### トリガーの種類

#### push
コードがプッシュされたときに実行：
```yaml
on:
  push:
    branches:
      - main
      - develop
    paths:
      - 'src/**'
```

#### pull_request
Pull Requestのイベントで実行：
```yaml
on:
  pull_request:
    types: [opened, synchronize, reopened]
    branches:
      - main
```

#### issue_comment
Issueやプルリクエストにコメントが投稿されたときに実行：
```yaml
on:
  issue_comment:
    types: [created]
```

#### workflow_dispatch
手動実行を可能にする：
```yaml
on:
  workflow_dispatch:
    inputs:
      environment:
        description: 'Deployment environment'
        required: true
        default: 'staging'
```

#### schedule
定期実行（cron形式）：
```yaml
on:
  schedule:
    - cron: '0 0 * * *'  # 毎日午前0時（UTC）
```

### ジョブとステップの概念

#### ジョブ（Jobs）
- 並列または直列で実行される作業単位
- 各ジョブは独立したランナーで実行される
- `needs`キーワードで依存関係を定義可能

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - name: Build
        run: npm run build

  test:
    needs: build
    runs-on: ubuntu-latest
    steps:
      - name: Test
        run: npm test
```

#### ステップ（Steps）
- ジョブ内で順次実行される個別のタスク
- アクション（`uses`）またはシェルコマンド（`run`）を実行

```yaml
steps:
  - name: Checkout
    uses: actions/checkout@v4

  - name: Setup Node
    uses: actions/setup-node@v4
    with:
      node-version: '18'

  - name: Install dependencies
    run: npm install
```

---

## 必要なシークレット設定

GitHub Actionsでセキュアな情報を扱うには、リポジトリのSecretsを設定する必要があります。

### GitHub Token (GITHUB_TOKEN)

#### 自動提供トークン
GitHubが自動的に提供するトークンで、基本的な操作に使用できます：
```yaml
env:
  GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

**制限事項**:
- ワークフローをトリガーできない
- 一部の高度な操作が制限される

### Personal Access Token (PAT_TOKEN)

#### 設定が必要なワークフロー
- `claude-code-review.yml`: PRへのコメント投稿とワークフロートリガー
- `auto-comment-issues.yml`: Issueへのコメント投稿

#### 作成手順
1. GitHubの設定 → Developer settings → Personal access tokens → Tokens (classic)
2. "Generate new token (classic)"をクリック
3. 以下の権限を選択：
   - `repo` (フルアクセス)
   - `workflow` (ワークフロー編集)
4. トークンを生成してコピー

#### リポジトリへの追加
1. リポジトリの Settings → Secrets and variables → Actions
2. "New repository secret"をクリック
3. Name: `PAT_TOKEN`
4. Secret: コピーしたトークンを貼り付け
5. "Add secret"をクリック

### Claude Code OAuth Token (CLAUDE_CODE_OAUTH_TOKEN)

#### 目的
Claude Code APIへのアクセスに必要な認証トークンです。

#### 取得方法
1. [Claude Code](https://claude.ai/code)にアクセス
2. OAuth認証フローに従ってトークンを取得
3. 詳細は[Claude Code Action ドキュメント](https://github.com/anthropics/claude-code-action)を参照

#### 設定方法
1. リポジトリの Settings → Secrets and variables → Actions
2. "New repository secret"をクリック
3. Name: `CLAUDE_CODE_OAUTH_TOKEN`
4. Secret: 取得したトークンを貼り付け
5. "Add secret"をクリック

### その他のAPI Key

プロジェクトで外部APIを使用する場合、同様の手順でシークレットを追加します：

```yaml
env:
  API_KEY: ${{ secrets.MY_API_KEY }}
  DATABASE_URL: ${{ secrets.DATABASE_URL }}
```

### Secretsの使用上の注意
- **ログに出力されない**: Secretsはログ上で自動的にマスクされる
- **Pull Requestでの制限**: フォークからのPRではSecretsにアクセスできない
- **定期的な更新**: セキュリティのため定期的にトークンを再生成・更新する

---

## ワークフローのカスタマイズ

### 新しいワークフローの追加方法

1. `.github/workflows/`ディレクトリに新しいYAMLファイルを作成
2. 基本テンプレートを使用：

```yaml
name: My Custom Workflow

on:
  push:
    branches:
      - main

jobs:
  custom-job:
    runs-on: ubuntu-latest
    permissions:
      contents: read

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Run custom script
        run: |
          echo "Hello, World!"
          # カスタムスクリプトをここに追加
```

3. コミット＆プッシュで自動的に有効化

### 既存ワークフローの編集

#### 安全な編集手順
1. ローカルで編集前にバックアップを作成
2. 変更をテストブランチで試す
3. `workflow_dispatch`トリガーを追加して手動テスト
4. 問題なければmainブランチにマージ

#### 一般的なカスタマイズ

##### トリガー条件の変更
```yaml
# 特定のブランチのみ
on:
  push:
    branches:
      - main
      - develop

# 特定のパスのみ
on:
  pull_request:
    paths:
      - 'src/**'
      - '!src/test/**'
```

##### 実行環境の変更
```yaml
jobs:
  build:
    runs-on: windows-latest  # Windowsで実行
    # または
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    runs-on: ${{ matrix.os }}
```

##### タイムアウトの設定
```yaml
jobs:
  long-running-job:
    runs-on: ubuntu-latest
    timeout-minutes: 30  # 30分でタイムアウト
```

### ローカルでのテスト方法

#### act を使用したローカルテスト
[act](https://github.com/nektos/act)はGitHub ActionsをローカルのDockerで実行できるツールです。

##### インストール
```bash
# macOS
brew install act

# Windows (Chocolatey)
choco install act-cli

# Linux
curl https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash
```

##### 使用方法
```bash
# 全てのワークフローを実行
act

# 特定のイベントをシミュレート
act push

# 特定のジョブのみ実行
act -j build

# シークレットを指定
act -s GITHUB_TOKEN=your-token
```

#### 制限事項
- 一部のGitHub固有の機能は動作しない
- Dockerが必要
- 大きなワークフローは実行に時間がかかる

#### デバッグモード
ワークフローにデバッグステップを追加：
```yaml
- name: Debug
  run: |
    echo "Event name: ${{ github.event_name }}"
    echo "Repository: ${{ github.repository }}"
    echo "Branch: ${{ github.ref }}"
```

---

## Claude Codeとの連携

### Claude CodeからのPR作成フロー

#### 基本的な流れ
1. **Issue作成**: ユーザーが実装したい機能や修正したいバグをIssueとして作成
2. **自動コメント**: `auto-comment-issues.yml`が定期的にチェックし、新しいIssueに`@claude`をメンション
3. **Claude起動**: `claude.yml`が`@claude`メンションを検知してワークフローを開始
4. **実装**: Claudeが以下を実行：
   - 新しいブランチ作成（`claude/issue-{番号}-{日時}`形式）
   - コードを実装
   - コミット＆プッシュ
5. **PR作成**: `gh pr create`コマンドでPull Requestを自動作成
6. **自動レビュー**: `claude-code-review.yml`がPRを検知して自動レビューを実行

#### 手動でのClaude起動
自動コメントを待たずに、手動で`@claude`をメンションすることも可能：

```markdown
@claude この機能を実装してください：
- 新しいUIコンポーネントを追加
- ユニットテストを含める
- ドキュメントを更新
```

#### ブランチ命名規則
Claudeが作成するブランチは以下の形式：
```
claude/issue-{Issue番号}-{YYYYMMDD}-{HHMM}
```

例: `claude/issue-42-20250115-1430`

### 自動レビューの活用

#### レビュー結果の解釈
- **問題ありません**: そのままマージ可能
- **問題を修正してください**: 自動修正ループが開始される
- **レビュー回数上限に達しました**: 手動確認が必要

#### 手動マージのタイミング
以下の場合は人間が最終確認してマージ：
- 自動レビューが3回繰り返された場合
- 大規模なリファクタリングの場合
- セキュリティに関わる変更の場合

### Issue作成時の自動化

#### 効果的なIssue作成のベストプラクティス
Claudeが理解しやすいIssueを作成するコツ：

1. **明確なタイトル**: 何を実装/修正するかを簡潔に記述
2. **詳細な説明**:
   - 目的
   - 期待される動作
   - 技術的な要件
   - UE5プラグインとしての制約
3. **チェックリスト**: タスクを明確に列挙
4. **サンプルコード**: 可能であれば例を提示

#### 良いIssueの例
```markdown
## 目的
プレイヤーのスコアを保存・読み込む機能を実装

## 実装内容
- [ ] SaveGameオブジェクトの作成
- [ ] スコア保存機能
- [ ] スコア読み込み機能
- [ ] エラーハンドリング

## 技術要件
- UE5のSaveGameシステムを使用
- Blueprint呼び出し可能な関数として実装
- プラグイン: ScreenTransitionSystem内に実装

## テスト
- [ ] 保存・読み込みの単体テスト
- [ ] エラーケースのテスト
```

---

## トラブルシューティング

### ワークフローが実行されない

#### 確認項目
1. **トリガー条件の確認**:
   ```yaml
   # @claudeが含まれているか？
   if: contains(github.event.comment.body, '@claude')
   ```

2. **ブランチ制限の確認**:
   ```yaml
   # 特定のブランチのみがトリガー対象の場合
   on:
     push:
       branches:
         - main
   ```

3. **権限の確認**:
   - リポジトリのActionsが有効か？
   - Settings → Actions → General → Actions permissions

4. **シークレットの確認**:
   - 必要なシークレットが設定されているか？
   - Settings → Secrets and variables → Actions

#### 解決方法
- Actionsタブでワークフローの状態を確認
- 最近のコミットでワークフローファイルが壊れていないかチェック
- YAMLのインデントやシンタックスエラーを確認（[YAML Lint](http://www.yamllint.com/)を使用）

### ワークフローが失敗する

#### よくあるエラー

##### 1. 認証エラー
```
Error: Unable to get ACTIONS_ID_TOKEN_REQUEST_TOKEN env variable
```

**原因**: `id-token: write`権限が不足
**解決策**:
```yaml
permissions:
  id-token: write
```

##### 2. シークレット未設定
```
Error: Input required and not supplied: claude_code_oauth_token
```

**原因**: `CLAUDE_CODE_OAUTH_TOKEN`が設定されていない
**解決策**: シークレットを設定（上記「必要なシークレット設定」を参照）

##### 3. 権限不足
```
Error: Resource not accessible by integration
```

**原因**: トークンの権限不足
**解決策**:
- `PAT_TOKEN`を使用
- 必要な権限（`repo`, `workflow`）を付与

##### 4. ブランチプッシュエラー
```
Error: failed to push some refs
```

**原因**: ブランチ保護ルールまたは権限不足
**解決策**:
- Settings → Branches → Branch protection rulesを確認
- Botアカウントを許可リストに追加

#### デバッグ方法

1. **ログの確認**:
   - Actionsタブ → 失敗したワークフロー → 各ステップをクリック

2. **デバッグログの有効化**:
   - Settings → Secrets and variables → Actions
   - New repository secret: `ACTIONS_STEP_DEBUG` = `true`

3. **ローカルで再現**:
   - `act`を使用してローカルで実行
   - 問題を特定しやすくなる

### ログの確認方法

#### Actionsタブでの確認
1. リポジトリのActionsタブにアクセス
2. 該当のワークフロー実行をクリック
3. 各ジョブとステップを展開して詳細ログを確認

#### ログのダウンロード
1. ワークフロー実行ページの右上「...」メニュー
2. "Download log archive"を選択
3. ZIPファイルをダウンロード・解凍して確認

#### ログの読み方
- **緑チェック**: 成功
- **赤バツ**: 失敗
- **黄色円**: 実行中またはスキップ
- **グレー円**: 実行されなかった（条件不一致）

#### 重要なログ出力
```yaml
- name: Debug output
  run: |
    echo "::debug::This is a debug message"
    echo "::notice::This is a notice"
    echo "::warning::This is a warning"
    echo "::error::This is an error"
```

---

## ベストプラクティス

### ワークフローの命名規則

#### ファイル名
- **小文字とハイフン**: `my-workflow.yml`
- **目的を明確に**: `deploy-production.yml`
- **環境を含める**: `test-staging.yml`

#### ワークフロー名
```yaml
name: Deploy to Production  # 明確で読みやすい名前
```

#### ジョブ名
```yaml
jobs:
  build-and-test:  # 何をするか明確に
    name: Build and Test Application
```

### 実行時間の最適化

#### 1. キャッシュの活用
```yaml
- name: Cache dependencies
  uses: actions/cache@v3
  with:
    path: node_modules
    key: ${{ runner.os }}-node-${{ hashFiles('**/package-lock.json') }}
```

#### 2. 並列実行
```yaml
jobs:
  test:
    strategy:
      matrix:
        node-version: [16, 18, 20]
    steps:
      - uses: actions/setup-node@v4
        with:
          node-version: ${{ matrix.node-version }}
```

#### 3. 浅いクローン
```yaml
- uses: actions/checkout@v4
  with:
    fetch-depth: 1  # 最新のコミットのみ取得
```

#### 4. 不要なステップをスキップ
```yaml
- name: Run tests
  if: github.event_name == 'pull_request'  # PRの時のみ実行
  run: npm test
```

#### 5. タイムアウトの設定
```yaml
jobs:
  build:
    timeout-minutes: 10  # 長時間実行を防ぐ
```

### セキュリティ考慮事項

#### 1. Secretsの適切な管理
```yaml
# ❌ 悪い例: ログに出力される可能性
- run: echo ${{ secrets.API_KEY }}

# ✅ 良い例: 環境変数として使用
- run: ./script.sh
  env:
    API_KEY: ${{ secrets.API_KEY }}
```

#### 2. 最小権限の原則
```yaml
# 必要な権限のみを付与
permissions:
  contents: read
  pull-requests: write
```

#### 3. サードパーティActionのバージョン固定
```yaml
# ❌ 悪い例: 最新版を使用（予期せぬ変更のリスク）
- uses: actions/checkout@main

# ✅ 良い例: バージョンを固定
- uses: actions/checkout@v4

# ✅ さらに良い例: コミットSHAを使用
- uses: actions/checkout@8ade135a41bc03ea155e62e844d188df1ea18608
```

#### 4. Pull Requestからのワークフロー実行
```yaml
on:
  pull_request_target:  # フォークからのPRでも実行
    types: [opened]

jobs:
  comment:
    # フォークからのPRには慎重に
    if: github.event.pull_request.head.repo.full_name == github.repository
```

#### 5. コード実行の検証
```yaml
# 信頼できないコードを実行する前に検証
- name: Validate input
  run: |
    if [[ ! "${{ github.event.comment.body }}" =~ ^[a-zA-Z0-9\ ]+$ ]]; then
      echo "Invalid input"
      exit 1
    fi
```

#### 6. 定期的なセキュリティ監査
- 使用していないSecretsを削除
- トークンを定期的に再生成
- Actionsの更新を確認
- Dependabotを有効化

### その他のベストプラクティス

#### ドキュメント化
```yaml
# ワークフロー内にコメントを追加
name: CI Pipeline

# このワークフローは以下を実行します：
# 1. コードをチェックアウト
# 2. 依存関係をインストール
# 3. テストを実行
# 4. ビルドを作成
```

#### エラーハンドリング
```yaml
- name: Build
  run: npm run build
  continue-on-error: false  # エラー時は停止

- name: Optional task
  run: npm run optional
  continue-on-error: true  # エラーでも続行
```

#### 通知設定
失敗時にSlackやメールで通知する設定を追加：
```yaml
- name: Notify failure
  if: failure()
  uses: slackapi/slack-github-action@v1
  with:
    webhook-url: ${{ secrets.SLACK_WEBHOOK }}
    payload: |
      {
        "text": "Workflow failed: ${{ github.workflow }}"
      }
```

---

## 参考リンク

- [GitHub Actions公式ドキュメント](https://docs.github.com/ja/actions)
- [Claude Code Action](https://github.com/anthropics/claude-code-action)
- [Workflow syntax for GitHub Actions](https://docs.github.com/ja/actions/using-workflows/workflow-syntax-for-github-actions)
- [Actions Marketplace](https://github.com/marketplace?type=actions)

---

**最終更新**: 2025-12-30
