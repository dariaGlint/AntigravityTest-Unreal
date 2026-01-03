# NPC Relationship System

NPC好感度管理と会話システムを提供するUnreal Engine 5プラグイン

## 概要

このプラグインは、NPCとの関係性を管理し、条件分岐のある会話システムを実装するための包括的なフレームワークを提供します。

## 主な機能

### 1. NPC好感度管理 (NPCRelationshipComponent)

- **好感度システム**: -100から100までの範囲でNPCとの好感度を管理
- **関係レベル**: 好感度に基づいて6段階の関係レベルを自動計算
  - 敵対 (Hostile)
  - 非友好的 (Unfriendly)
  - 中立 (Neutral)
  - 友好的 (Friendly)
  - 親密 (Close)
  - 献身的 (Devoted)
- **統計追跡**: 会話回数、プレゼント数などを記録
- **カスタムタグ**: GameplayTagsを使用した柔軟な状態管理
- **イベント通知**: 好感度や関係レベルの変化時にデリゲートで通知

### 2. 会話選択肢の条件分岐 (ConversationSystem)

- **条件付き会話ノード**: 好感度、関係レベル、イベントフラグなどに基づいて会話を分岐
- **動的な選択肢**: プレイヤーの状態に応じて選択肢を表示/非表示
- **選択による影響**: 選択肢ごとに好感度変化やイベントフラグの設定が可能
- **複雑な会話ツリー**: ノードベースの柔軟な会話構造

### 3. 関係値による反応変化

- **自動的な反応調整**: 関係レベルに応じてNPCの反応を変更
- **条件システム**:
  - 最小/最大好感度
  - 関係レベル
  - 会話回数
  - カスタムタグ
  - イベントフラグ

### 4. イベントフラグ連携 (EventFlagManager)

- **グローバルフラグ管理**: ゲーム全体でイベントフラグを管理
- **フラグベースの条件**: 会話や選択肢の表示条件にフラグを使用
- **複合条件**: AND/OR条件でのフラグチェック
- **イベント通知**: フラグ変更時にデリゲートで通知

## 使用方法

### 基本セットアップ

1. プラグインをプロジェクトに追加
2. プレイヤーまたはゲーム管理アクターに `NPCRelationshipComponent` を追加

### Blueprint での使用例

#### 好感度の変更

```cpp
// C++
NPCRelationshipComponent->ModifyAffection(TEXT("NPC_John"), 10.0f);

// または直接設定
NPCRelationshipComponent->SetAffection(TEXT("NPC_John"), 50.0f);
```

#### 関係レベルの確認

```cpp
ERelationshipLevel Level = NPCRelationshipComponent->GetRelationshipLevel(TEXT("NPC_John"));
```

#### イベントフラグの設定

```cpp
UEventFlagManager* EventFlags = UNPCRelationshipBlueprintLibrary::GetEventFlagManager(this);
EventFlags->SetEventFlag(TEXT("QuestCompleted"), true);
```

#### 会話の実行

```cpp
// 会話システムを作成
UConversationSystem* ConvSystem = UNPCRelationshipBlueprintLibrary::CreateConversationSystem(this);

// 会話ノードを取得
FConversationNode Node = ConvSystem->GetConversationNode(ConversationTree, TEXT("Node_Start"));

// 利用可能な選択肢を取得（条件チェック済み）
TArray<FDialogueOption> Options = ConvSystem->GetAvailableOptions(
    Node,
    NPCRelationshipComponent,
    TEXT("NPC_John"),
    EventFlags
);

// ノードの処理（好感度変化など）
ConvSystem->ProcessConversationNode(Node, NPCRelationshipComponent, TEXT("NPC_John"), EventFlags);
```

### 会話ツリーの作成

会話ツリーは `FConversationTree` 構造体で定義します：

```cpp
FConversationTree ConversationTree;
ConversationTree.ConversationId = TEXT("FirstMeeting");
ConversationTree.NPCId = TEXT("NPC_John");
ConversationTree.StartNodeId = TEXT("Node_Start");

// 会話ノードの追加
FConversationNode StartNode;
StartNode.NodeId = TEXT("Node_Start");
StartNode.NPCDialogue = FText::FromString(TEXT("こんにちは！"));

// 選択肢の追加
FDialogueOption Option1;
Option1.OptionId = TEXT("Option_Friendly");
Option1.OptionText = FText::FromString(TEXT("こんにちは、良い天気ですね！"));
Option1.AffectionDelta = 5.0f;
Option1.NextNodeId = TEXT("Node_Friendly");

// 条件付き選択肢
FDialogueOption Option2;
Option2.OptionId = TEXT("Option_Special");
Option2.OptionText = FText::FromString(TEXT("例の件について..."));

// 好感度50以上の場合のみ表示
FConversationCondition Condition;
Condition.ConditionType = EConversationConditionType::MinAffection;
Condition.AffectionValue = 50.0f;
Option2.Conditions.Add(Condition);

StartNode.Options.Add(Option1);
StartNode.Options.Add(Option2);
ConversationTree.Nodes.Add(StartNode);
```

## API リファレンス

### NPCRelationshipComponent

- `ModifyAffection(FName NPCId, float Delta)`: 好感度を変更
- `SetAffection(FName NPCId, float NewAffection)`: 好感度を設定
- `GetAffection(FName NPCId)`: 好感度を取得
- `GetRelationshipLevel(FName NPCId)`: 関係レベルを取得
- `IncrementConversationCount(FName NPCId)`: 会話回数を増加
- `IncrementGiftsGiven(FName NPCId)`: プレゼント数を増加
- `AddCustomTag(FName NPCId, FGameplayTag Tag)`: カスタムタグを追加
- `RemoveCustomTag(FName NPCId, FGameplayTag Tag)`: カスタムタグを削除
- `HasCustomTag(FName NPCId, FGameplayTag Tag)`: カスタムタグの確認

### ConversationSystem

- `CheckCondition(...)`: 単一条件のチェック
- `CheckAllConditions(...)`: すべての条件をチェック（AND）
- `GetConversationNode(...)`: 会話ノードを取得
- `GetAvailableOptions(...)`: 利用可能な選択肢を取得
- `ProcessDialogueOption(...)`: 選択肢を処理
- `ProcessConversationNode(...)`: 会話ノードを処理

### EventFlagManager

- `SetEventFlag(FName FlagName, bool bValue)`: イベントフラグを設定
- `GetEventFlag(FName FlagName)`: イベントフラグを取得
- `ToggleEventFlag(FName FlagName)`: イベントフラグをトグル
- `ClearEventFlag(FName FlagName)`: イベントフラグをクリア
- `CheckAllFlags(TArray<FName>)`: すべてのフラグをチェック（AND）
- `CheckAnyFlag(TArray<FName>)`: いずれかのフラグをチェック（OR）

## ライセンス

このプラグインはMITライセンスの下で提供されています。

## 作成者

ダリア
