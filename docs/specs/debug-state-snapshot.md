# Debug State Snapshot Plugin

## 概要

ゲーム状態のスナップショットを保存・復元するプラグイン。バグ再現やテストシナリオの作成を支援する。任意の時点でゲーム状態を保存し、後で復元できる。

## 基本情報

| 項目 | 値 |
|------|-----|
| プラグイン名 | DebugStateSnapshot |
| モジュールタイプ | Runtime (DeveloperOnly) |
| ロードフェーズ | Default |
| 対象ビルド | Editor, Development のみ |

## 要件

### 必須要件

1. **スナップショット保存**
   - 名前付きスロットに状態を保存
   - 自動保存（チェックポイント通過時など）
   - クイックセーブ（1キーで即座に保存）
   - 保存対象の選択（全体/プレイヤーのみ/特定Actor）

2. **保存対象データ**
   - **Player**: 位置、回転、HP、ステート、インベントリ
   - **Enemies**: 位置、HP、AIステート、ターゲット
   - **World**: 破壊可能オブジェクトの状態、ドアの開閉状態
   - **GameMode**: スコア、進行状況、チェックポイント
   - **Timestamp**: 保存日時、プレイ時間

3. **スナップショット復元**
   - 保存したスロットから状態を復元
   - クイックロード（1キーで即座に復元）
   - 部分復元（プレイヤーのみ復元など）

4. **スナップショット管理**
   - スロット一覧表示
   - スナップショットの削除
   - スナップショットの説明/メモ追加
   - ファイルへのエクスポート/インポート

5. **UI**
   - シンプルなスナップショットマネージャーUI
   - スロット選択、プレビュー情報表示
   - 保存/復元/削除ボタン

### オプション要件

1. **差分表示**
   - 現在の状態と保存状態の差分を表示

2. **自動スナップショット**
   - 一定時間ごとに自動保存
   - ローテーション（古いものから削除）

3. **シナリオ機能**
   - 複数のスナップショットをシーケンスとして保存
   - バグ再現手順の記録

## アーキテクチャ

### クラス構成

```
UStateSnapshotSubsystem (UGameInstanceSubsystem)
├── SaveSnapshot()         # スナップショット保存
├── LoadSnapshot()         # スナップショット復元
├── DeleteSnapshot()       # スナップショット削除
├── GetSnapshotList()      # スロット一覧取得
├── GetSnapshotInfo()      # スナップショット情報取得
├── ExportSnapshot()       # ファイルエクスポート
├── ImportSnapshot()       # ファイルインポート
└── OnSnapshotSaved/Loaded # デリゲート

FStateSnapshot (構造体)
├── SlotName               # スロット名
├── Description            # 説明
├── SavedTime              # 保存日時
├── PlayTime               # プレイ時間
├── LevelName              # レベル名
├── ThumbnailData          # サムネイル画像（オプション）
└── StateData              # 状態データ（シリアライズ済み）

ISnapshotable (インターフェース)
├── SaveToSnapshot()       # スナップショットへ保存
├── LoadFromSnapshot()     # スナップショットから復元
└── GetSnapshotId()        # 識別子取得

USnapshotManagerWidget (UUserWidget)
├── SlotListView           # スロットリスト
├── InfoPanel              # 情報パネル
├── ActionButtons          # アクションボタン
└── QuickSlotIndicator     # クイックスロット表示

USnapshotBlueprintLibrary (UBlueprintFunctionLibrary)
├── QuickSave()            # クイックセーブ
├── QuickLoad()            # クイックロード
├── SaveToSlot()           # スロットに保存
└── LoadFromSlot()         # スロットから復元
```

### モジュール構成

```
DebugStateSnapshot/
├── DebugStateSnapshot.uplugin
├── Content/
│   └── UI/
│       └── WBP_SnapshotManager.uasset
└── Source/
    └── DebugStateSnapshot/
        ├── DebugStateSnapshot.Build.cs
        ├── Public/
        │   ├── DebugStateSnapshotModule.h
        │   ├── StateSnapshotSubsystem.h
        │   ├── StateSnapshot.h
        │   ├── Snapshotable.h              # インターフェース
        │   ├── SnapshotManagerWidget.h
        │   └── SnapshotBlueprintLibrary.h
        └── Private/
            ├── DebugStateSnapshotModule.cpp
            ├── StateSnapshotSubsystem.cpp
            ├── SnapshotManagerWidget.cpp
            ├── SnapshotBlueprintLibrary.cpp
            └── Serializers/
                ├── PlayerStateSerializer.cpp
                ├── ActorStateSerializer.cpp
                ├── WorldStateSerializer.cpp
                └── GameModeStateSerializer.cpp
```

## インターフェース

### スナップショット保存

```cpp
// 保存オプション
USTRUCT(BlueprintType)
struct FSnapshotSaveOptions
{
    GENERATED_BODY()

    // スロット名（必須）
    UPROPERTY(BlueprintReadWrite)
    FString SlotName;

    // 説明（オプション）
    UPROPERTY(BlueprintReadWrite)
    FString Description;

    // 保存対象
    UPROPERTY(BlueprintReadWrite)
    bool bSavePlayer = true;

    UPROPERTY(BlueprintReadWrite)
    bool bSaveEnemies = true;

    UPROPERTY(BlueprintReadWrite)
    bool bSaveWorldState = true;

    UPROPERTY(BlueprintReadWrite)
    bool bSaveGameMode = true;

    // サムネイル生成
    UPROPERTY(BlueprintReadWrite)
    bool bCaptureThumbnail = true;

    // 特定Actorのみ保存（空なら全Actor）
    UPROPERTY(BlueprintReadWrite)
    TArray<AActor*> SpecificActors;
};

// 保存関数
UFUNCTION(BlueprintCallable, Category="Debug Snapshot")
bool SaveSnapshot(const FSnapshotSaveOptions& Options);

// クイックセーブ
UFUNCTION(BlueprintCallable, Category="Debug Snapshot")
bool QuickSave();
```

### スナップショット復元

```cpp
// 復元オプション
USTRUCT(BlueprintType)
struct FSnapshotLoadOptions
{
    GENERATED_BODY()

    // スロット名
    UPROPERTY(BlueprintReadWrite)
    FString SlotName;

    // 復元対象
    UPROPERTY(BlueprintReadWrite)
    bool bLoadPlayer = true;

    UPROPERTY(BlueprintReadWrite)
    bool bLoadEnemies = true;

    UPROPERTY(BlueprintReadWrite)
    bool bLoadWorldState = true;

    UPROPERTY(BlueprintReadWrite)
    bool bLoadGameMode = true;

    // レベル変更が必要な場合に自動遷移
    UPROPERTY(BlueprintReadWrite)
    bool bAutoTransitionLevel = true;
};

// 復元関数
UFUNCTION(BlueprintCallable, Category="Debug Snapshot")
bool LoadSnapshot(const FSnapshotLoadOptions& Options);

// クイックロード
UFUNCTION(BlueprintCallable, Category="Debug Snapshot")
bool QuickLoad();
```

### ISnapshotableインターフェース

```cpp
UINTERFACE(MinimalAPI, Blueprintable)
class USnapshotable : public UInterface
{
    GENERATED_BODY()
};

class ISnapshotable
{
    GENERATED_BODY()

public:
    // スナップショットへ状態を保存
    virtual void SaveToSnapshot(FArchive& Ar) = 0;

    // スナップショットから状態を復元
    virtual void LoadFromSnapshot(FArchive& Ar) = 0;

    // 一意の識別子を返す
    virtual FString GetSnapshotId() const = 0;
};
```

## 振る舞い

### 正常系

1. **保存フロー**
   ```
   SaveSnapshot呼び出し
   → レベル名、時刻を記録
   → ISnapshotableを実装したActorを収集
   → 各ActorのSaveToSnapshotを呼び出し
   → データをシリアライズ
   → ファイルに書き込み（Saved/Snapshots/）
   → サムネイル生成（オプション）
   → 成功通知
   ```

2. **復元フロー**
   ```
   LoadSnapshot呼び出し
   → スロット名でファイルを検索
   → レベル確認（異なる場合は遷移）
   → データをデシリアライズ
   → 既存Actorを検索、なければスポーン
   → 各ActorのLoadFromSnapshotを呼び出し
   → 成功通知
   ```

3. **クイックセーブ/ロード**
   - 専用スロット「QuickSave」を使用
   - 確認ダイアログなし
   - 画面に保存/復元通知を表示

### エラー時

1. **保存失敗**
   - ディスク容量不足: 警告メッセージ表示
   - 書き込み権限なし: エラーログ出力

2. **復元失敗**
   - スロットが存在しない: `Snapshot not found: <SlotName>`
   - レベルが異なる: 自動遷移または警告
   - データ破損: エラーログ、部分復元を試行

3. **Actorが見つからない**
   - 復元時にActorが削除されていた場合はスキップ
   - ログに警告を出力

### エッジケース

1. **レベル遷移中**
   - 保存/復元を拒否、警告メッセージ

2. **異なるレベルのスナップショット**
   - `bAutoTransitionLevel`がtrueならレベル遷移後に復元
   - falseなら復元拒否

## 制約・前提条件

### ビルド制限

```cpp
#if !UE_BUILD_SHIPPING
// スナップショット機能の実装
#endif
```

### 依存関係

- **Engine**: 基本機能
- **UMG**: UIウィジェット

### ストレージ

- 保存場所: `Saved/Snapshots/`
- ファイル形式: `.snapshot`（バイナリ）
- 1スナップショットあたり: 100KB〜1MB（レベル規模による）
- 最大スロット数: 制限なし（ディスク容量依存）

## 保存対象の詳細

### プレイヤー状態

```cpp
struct FPlayerSnapshotData
{
    FTransform Transform;           // 位置・回転
    float CurrentHP;                // 現在HP
    float MaxHP;                    // 最大HP
    FName CurrentState;             // 現在のステート
    TArray<FInventoryItem> Items;   // インベントリ
    // Variant_Combat固有
    int32 ComboCount;
    bool bIsCharging;
};
```

### 敵状態

```cpp
struct FEnemySnapshotData
{
    FString EnemyClass;             // 敵のクラス名
    FTransform Transform;           // 位置・回転
    float CurrentHP;                // HP
    FName AIState;                  // AIステート
    FString TargetActorId;          // ターゲット
    bool bIsActive;                 // アクティブ状態
};
```

### ワールド状態

```cpp
struct FWorldSnapshotData
{
    TMap<FString, bool> DestructibleStates;    // 破壊状態
    TMap<FString, bool> DoorStates;            // ドア開閉
    TMap<FString, FTransform> MovableActors;   // 移動可能Actor
};
```

## UI設計

### スナップショットマネージャー

```
+--------------------------------------------------+
| Snapshot Manager                      [_][□][×]  |
+--------------------------------------------------+
| Quick Slot: QuickSave_01 (12:34:56)              |
| [Quick Save (F5)] [Quick Load (F9)]              |
+--------------------------------------------------+
| Saved Snapshots:                                 |
| +----------------------------------------------+ |
| | ● boss_fight_start     | 2025-01-15 14:30   | |
| |   "Before boss battle" | Combat_Level       | |
| +----------------------------------------------+ |
| | ○ checkpoint_02        | 2025-01-15 14:15   | |
| |   "After puzzle"       | Puzzle_Level       | |
| +----------------------------------------------+ |
| | ○ bug_repro_001        | 2025-01-14 10:00   | |
| |   "Crash reproduction" | Combat_Level       | |
| +----------------------------------------------+ |
+--------------------------------------------------+
| Selected: boss_fight_start                       |
| Level: Combat_Level | Play Time: 00:45:30        |
| [Load] [Delete] [Export]  [New Snapshot]         |
+--------------------------------------------------+
```

### キーバインド

| キー | 機能 |
|------|------|
| F5 | クイックセーブ |
| F9 | クイックロード |
| F10 | スナップショットマネージャー表示 |

## DebugConsoleCommands連携

```
# クイックセーブ
QuickSave

# クイックロード
QuickLoad

# 名前付きスロットに保存
SaveState boss_fight "Before boss battle"

# スロットから復元
LoadState boss_fight

# スロット一覧
ListSnapshots

# スナップショット削除
DeleteSnapshot old_save

# エクスポート
ExportSnapshot boss_fight C:/debug/boss_fight.snapshot
```

## EventLogViewer連携

```cpp
// スナップショット保存/復元時にイベントログに記録
void UStateSnapshotSubsystem::SaveSnapshot(...)
{
    // 保存処理...

#if !UE_BUILD_SHIPPING
    if (auto* EventLog = GetGameInstance()->GetSubsystem<UEventLogSubsystem>())
    {
        EventLog->LogEvent(
            EDebugEventCategory::System,
            EDebugEventSeverity::Info,
            FString::Printf(TEXT("Snapshot saved: %s"), *Options.SlotName)
        );
    }
#endif
}
```

## 使用例

### コードからの使用

```cpp
// 保存
if (auto* Snapshot = GetGameInstance()->GetSubsystem<UStateSnapshotSubsystem>())
{
    FSnapshotSaveOptions Options;
    Options.SlotName = TEXT("before_boss");
    Options.Description = TEXT("Before entering boss room");
    Snapshot->SaveSnapshot(Options);
}

// 復元
FSnapshotLoadOptions LoadOptions;
LoadOptions.SlotName = TEXT("before_boss");
Snapshot->LoadSnapshot(LoadOptions);
```

### Blueprintからの使用

```
// ノード: Quick Save
// → 画面に "State Saved" と表示

// ノード: Load Snapshot
// Slot Name: "checkpoint_01"
// Load Player: true
// Load Enemies: true
```

### ISnapshotableの実装例

```cpp
// CombatCharacterでの実装
class ACombatCharacter : public ACharacter, public ISnapshotable
{
    // ISnapshotable実装
    virtual void SaveToSnapshot(FArchive& Ar) override
    {
        FTransform Transform = GetActorTransform();
        Ar << Transform;
        Ar << CurrentHP;
        Ar << MaxHP;
        // ...
    }

    virtual void LoadFromSnapshot(FArchive& Ar) override
    {
        FTransform Transform;
        Ar << Transform;
        SetActorTransform(Transform);
        Ar << CurrentHP;
        Ar << MaxHP;
        // ...
    }

    virtual FString GetSnapshotId() const override
    {
        return GetName();
    }
};
```

## テスト計画

1. **ユニットテスト**
   - 保存/復元の整合性
   - シリアライズ/デシリアライズ
   - 複数スロット管理

2. **統合テスト**
   - Variant_Combatでの動作確認
   - レベル遷移を伴う復元
   - 他プラグインとの連携

3. **ストレステスト**
   - 大量のActor保存
   - 連続保存/復元

## 今後の拡張

1. **クラウド同期**
   - スナップショットのクラウド保存

2. **共有機能**
   - 他の開発者とスナップショット共有

3. **タイムライン機能**
   - 自動スナップショットの時系列表示
   - 任意の時点への巻き戻し

4. **差分スナップショット**
   - 変更点のみ保存して容量削減
