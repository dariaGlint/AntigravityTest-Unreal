# Achievement System Plugin

Unreal Engine 5用の包括的なアチーブメント/トロフィーシステムプラグイン

## 概要

このプラグインは、ゲームに完全な機能を持つアチーブメントシステムを追加します。達成条件の定義、進捗追跡、解除処理、統計データ収集などの機能を提供します。

## 主な機能

### 1. アチーブメント定義
- **複数のアチーブメントタイプ**
  - Simple: 一度達成すれば解除されるシンプルなアチーブメント
  - Progressive: 進捗を蓄積して達成するアチーブメント
  - Count: 複数回の達成が必要なカウント型アチーブメント
  - Hidden: 達成条件が隠されているアチーブメント

- **カテゴリ分類**
  - General（一般）
  - Combat（戦闘）
  - Exploration（探索）
  - Collection（収集）
  - Social（ソーシャル）
  - Secret（秘密）

### 2. 進捗追跡システム
- リアルタイムの進捗更新
- 進捗パーセンテージの自動計算
- 進捗更新イベント通知

### 3. 解除処理
- 自動解除検出
- 手動解除機能
- 解除イベント通知
- 解除時刻の記録

### 4. 統計データ収集
- 総アチーブメント数
- 解除済みアチーブメント数
- 総獲得ポイント
- 完了率計算
- カテゴリ別解除数

### 5. データ永続化
- SaveGame システムを使用した進捗データの保存/ロード
- カスタマイズ可能なセーブスロット

## 使用方法

### 1. プラグインの有効化

1. プロジェクトの `.uproject` ファイルを開く
2. プラグインセクションに以下を追加（または、エディタのプラグインマネージャーから有効化）

### 2. アチーブメント定義の作成

#### データテーブルを使用する方法（推奨）

1. コンテンツブラウザで右クリック → Miscellaneous → Data Table
2. Row Structure として `AchievementDefinition` を選択
3. データテーブルにアチーブメントを追加

例：
```cpp
// アチーブメント ID: "FirstKill"
Display Name: "初めての撃破"
Description: "初めて敵を倒した"
Type: Simple
Category: Combat
Required Progress: 1
Is Hidden: false
Reward Points: 10
```

#### Blueprint/C++から直接登録

```cpp
// C++の場合
UAchievementManager* Manager = GetGameInstance()->GetSubsystem<UAchievementManager>();

FAchievementDefinition NewAchievement;
NewAchievement.AchievementId = FName("FirstKill");
NewAchievement.DisplayName = FText::FromString("初めての撃破");
NewAchievement.Description = FText::FromString("初めて敵を倒した");
NewAchievement.Type = EAchievementType::Simple;
NewAchievement.Category = EAchievementCategory::Combat;
NewAchievement.RequiredProgress = 1;
NewAchievement.RewardPoints = 10;

Manager->RegisterAchievement(NewAchievement);
```

### 3. 進捗の更新

```cpp
// C++の場合
UAchievementManager* Manager = GetGameInstance()->GetSubsystem<UAchievementManager>();

// 進捗を1追加
Manager->UpdateProgress(FName("FirstKill"));

// 進捗を指定量追加
Manager->UpdateProgress(FName("Kill100Enemies"), 5);

// 進捗を直接設定
Manager->SetProgress(FName("CollectCoins"), 50);
```

Blueprintでも同様に使用可能です。

### 4. イベントのリスニング

```cpp
// C++の場合
UAchievementManager* Manager = GetGameInstance()->GetSubsystem<UAchievementManager>();

// アチーブメント解除時のイベント
Manager->OnAchievementUnlocked.AddDynamic(this, &AMyClass::OnAchievementUnlocked);

// 進捗更新時のイベント
Manager->OnAchievementProgressUpdated.AddDynamic(this, &AMyClass::OnProgressUpdated);
```

### 5. 統計データの取得

```cpp
// C++の場合
UAchievementManager* Manager = GetGameInstance()->GetSubsystem<UAchievementManager>();

FAchievementStatistics Stats = Manager->GetStatistics();

UE_LOG(LogTemp, Log, TEXT("Total: %d, Unlocked: %d, Completion: %.2f%%"),
    Stats.TotalAchievements,
    Stats.UnlockedAchievements,
    Stats.CompletionRate * 100.0f);
```

### 6. データの保存/ロード

```cpp
// C++の場合
UAchievementManager* Manager = GetGameInstance()->GetSubsystem<UAchievementManager>();

// 保存
Manager->SaveProgress("MyAchievementSave");

// ロード
Manager->LoadProgress("MyAchievementSave");
```

## API リファレンス

### UAchievementManager

GameInstanceSubsystemとして実装されており、ゲーム全体で単一のインスタンスとして動作します。

#### 主要メソッド

- `RegisterAchievement(const FAchievementDefinition& Definition)` - アチーブメントを登録
- `LoadAchievementsFromDataTable(UDataTable* DataTable)` - データテーブルからロード
- `UpdateProgress(FName AchievementId, int32 Progress)` - 進捗を更新
- `SetProgress(FName AchievementId, int32 Progress)` - 進捗を設定
- `UnlockAchievement(FName AchievementId)` - 強制的に解除
- `GetProgress(FName AchievementId)` - 進捗データを取得
- `IsAchievementUnlocked(FName AchievementId)` - 解除済みかチェック
- `GetAllAchievementDefinitions()` - すべての定義を取得
- `GetAchievementsByCategory(EAchievementCategory Category)` - カテゴリ別に取得
- `GetStatistics()` - 統計データを取得
- `ResetAllProgress()` - すべての進捗をリセット
- `SaveProgress(const FString& SlotName)` - 進捗を保存
- `LoadProgress(const FString& SlotName)` - 進捗をロード

#### イベント

- `OnAchievementUnlocked` - アチーブメント解除時
- `OnAchievementProgressUpdated` - 進捗更新時

## システム要件

- Unreal Engine 5.0以降
- C++17以降

## ライセンス

このプラグインはプロジェクト内で自由に使用・改変できます。

## サポート

問題が発生した場合は、プロジェクトのIssueトラッカーに報告してください。
