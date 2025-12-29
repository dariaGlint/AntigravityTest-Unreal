# Team Management System Plugin

UE5用のチーム管理システムプラグイン

## 概要

マルチプレイヤーゲーム向けの包括的なチーム管理システムです。以下の機能を提供します：

- チーム分け（手動/自動）
- フレンドリーファイア制御
- スコア集計（個人/チーム）
- チームカラー/マーカー

## 主要クラス

### ATeamInfo
チーム情報を管理するActorクラス。以下の機能を持ちます：

- チームID、チーム名、チームカラーの管理
- チームメンバーの追加/削除
- チームスコアの集計
- 個人スコア、キル/デス/アシストの記録

### UTeamManagementComponent
プレイヤーに付与するコンポーネント。以下の機能を持ちます：

- チームへの参加/脱退
- 味方/敵の判定
- フレンドリーファイア制御
- チームカラーの取得

### ATeamManager
ゲーム全体のチーム管理を行うマネージャークラス。以下の機能を持ちます：

- チームの作成/削除
- プレイヤーのチーム割り当て（手動/自動）
- チームのリセット
- スコアによるチームソート

## 使用方法

### 1. 初期設定

GameModeまたはGameStateで`ATeamManager`を1つ生成します：

```cpp
ATeamManager* TeamManager = GetWorld()->SpawnActor<ATeamManager>();
TeamManager->InitializeDefaultTeams(2); // 2チーム作成
```

### 2. プレイヤーへの適用

PlayerStateまたはCharacterに`UTeamManagementComponent`を追加します：

```cpp
// C++の場合
UTeamManagementComponent* TeamComp = CreateDefaultSubobject<UTeamManagementComponent>(TEXT("TeamManagement"));

// Blueprintの場合
// コンポーネントパネルから「Team Management Component」を追加
```

### 3. チームへの割り当て

```cpp
// 自動割り当て
TeamManager->AutoAssignPlayerToTeam(Player);

// 手動割り当て
TeamManager->AssignPlayerToTeam(Player, 1); // チーム1に割り当て
```

### 4. フレンドリーファイアの制御

ダメージ処理の前にチェックを行います：

```cpp
UTeamManagementComponent* AttackerTeamComp = Attacker->FindComponentByClass<UTeamManagementComponent>();
if (AttackerTeamComp && !AttackerTeamComp->CanDamage(Target))
{
    // フレンドリーファイアが無効で味方への攻撃の場合、ダメージを与えない
    return;
}

// ダメージ処理
```

### 5. スコアの記録

```cpp
ATeamInfo* Team = TeamManager->GetTeamById(TeamId);
if (Team)
{
    // スコア加算
    Team->UpdateMemberScore(PlayerId, 100);

    // キル記録
    Team->RecordMemberKill(PlayerId);

    // デス記録
    Team->RecordMemberDeath(PlayerId);

    // アシスト記録
    Team->RecordMemberAssist(PlayerId);
}
```

### 6. チームカラーの取得と適用

```cpp
UTeamManagementComponent* TeamComp = Player->FindComponentByClass<UTeamManagementComponent>();
if (TeamComp)
{
    FLinearColor TeamColor = TeamComp->GetTeamColor();
    // マテリアルに色を適用
    DynamicMaterial->SetVectorParameterValue(TEXT("TeamColor"), TeamColor);
}
```

## Blueprint使用例

すべてのクラスと関数はBlueprint対応です。

### チーム作成
- `Create Team` ノード: 新しいチームを作成
- `Initialize Default Teams` ノード: デフォルトチームを初期化

### チーム割り当て
- `Auto Assign Player To Team` ノード: 自動割り当て
- `Assign Player To Team` ノード: 手動割り当て

### 味方/敵判定
- `Is Friendly` ノード: 味方かどうかチェック
- `Is Enemy` ノード: 敵かどうかチェック
- `Can Damage` ノード: ダメージを与えられるかチェック

## チーム自動割り当てモード

`ATeamManager`の`AssignmentMode`で設定できます：

- **Manual**: 手動割り当てのみ
- **RoundRobin**: ラウンドロビン方式で順番に割り当て
- **BalanceByCount**: 人数が少ないチームに割り当て
- **BalanceByScore**: スコアが低いチームに割り当て

## ネットワーク対応

すべての主要クラスはレプリケーション対応しており、マルチプレイヤーゲームで使用できます。

## 依存モジュール

- Core
- CoreUObject
- Engine
- NetCore

## ライセンス

このプラグインはプロジェクト専用です。
