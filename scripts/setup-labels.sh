#!/bin/bash
# GitHubラベルの作成スクリプト

set -e

echo "Creating GitHub labels..."

# 技術領域 (Technology)
gh label create "UE5" --description "UE5のC++/Blueprint機能実装" --color "0075ca" --force || true
gh label create "Plugin" --description "プラグイン開発" --color "1d76db" --force || true
gh label create "AI" --description "AI関連（Behavior Tree, EQS, State Tree等）" --color "5319e7" --force || true
gh label create "UI" --description "UI/UX実装（UMG, Slate）" --color "0e8a16" --force || true
gh label create "Gameplay" --description "ゲームプレイロジック" --color "006b75" --force || true
gh label create "Python" --description "Pythonスクリプト関連" --color "fbca04" --force || true
gh label create "CI/CD" --description "GitHub Actions、ワークフロー" --color "d4c5f9" --force || true

# 作業種類 (Type)
gh label create "documentation" --description "ドキュメント整備" --color "0075ca" --force || true
gh label create "enhancement" --description "既存機能の改善" --color "a2eeef" --force || true
gh label create "bug" --description "バグ修正" --color "d73a4a" --force || true
gh label create "feature" --description "新機能追加" --color "0e8a16" --force || true
gh label create "refactor" --description "リファクタリング" --color "fbca04" --force || true
gh label create "test" --description "テスト関連" --color "d876e3" --force || true

# 優先度 (Priority)
gh label create "priority: high" --description "高優先度" --color "d93f0b" --force || true
gh label create "priority: medium" --description "中優先度" --color "fbca04" --force || true
gh label create "priority: low" --description "低優先度" --color "c2e0c6" --force || true

# 状態 (Status)
gh label create "help wanted" --description "支援が必要" --color "008672" --force || true
gh label create "good first issue" --description "初心者向け" --color "7057ff" --force || true
gh label create "blocked" --description "ブロックされている" --color "e99695" --force || true
gh label create "in progress" --description "作業中" --color "d4c5f9" --force || true
gh label create "claude-auto" --description "Claude自動実装対象" --color "bfdadc" --force || true

echo "✅ Labels created successfully!"
