# Debug Event Log Viewer Plugin

## 概要

ゲーム内イベントをリアルタイムで記録・表示するプラグイン。ダメージ、ステート遷移、アビリティ発動などのイベント履歴を時系列で確認でき、バグの原因特定を支援する。

## 基本情報

| 項目 | 値 |
|------|-----|
| プラグイン名 | DebugEventLogViewer |
| モジュールタイプ | Runtime (DeveloperOnly) |
| ロードフェーズ | Default |
| 対象ビルド | Editor, Development のみ |

## 要件

### 必須要件

1. **イベント記録システム**
   - タイムスタンプ付きでイベントを記録
   - カテゴリ別にイベントを分類
   - 重要度レベル（Info, Warning, Error）
   - 最大保持件数を設定可能（デフォルト: 1000件）

2. **対応イベントカテゴリ**
   - **Combat**: ダメージ、HP変化、死亡、リスポーン
   - **State**: ステート遷移、アビリティ状態変化
   - **AI**: AI行動決定、ターゲット変更、パス検索
   - **Input**: 入力イベント（オプション、大量になるため）
   - **System**: レベルロード、ポーズ、GameMode変更
   - **Custom**: ユーザー定義イベント

3. **表示UI**
   - オーバーレイウィジェットでリアルタイム表示
   - フィルタリング機能（カテゴリ、重要度、Actor別）
   - 一時停止/再開機能
   - ログのクリア機能
   - スクロール可能なリスト表示

4. **出力機能**
   - ファイルへのエクスポート（CSV, JSON）
   - クリップボードへのコピー

### オプション要件

1. **検索機能**
   - キーワード検索
   - 時間範囲指定

2. **ブックマーク**
   - 重要なイベントにマークを付ける
   - マーク位置へジャンプ

3. **イベント関連付け**
   - 関連するイベントをグループ化
   - 例: 攻撃開始→ダメージ→死亡を一連のイベントとして表示

## アーキテクチャ

### クラス構成

```
UEventLogSubsystem (UGameInstanceSubsystem)
├── LogEvent()             # イベント記録
├── GetEvents()            # イベント取得
├── ClearEvents()          # ログクリア
├── SetFilter()            # フィルタ設定
├── ExportToFile()         # ファイル出力
└── OnEventLogged          # イベント記録時デリゲート

FDebugEvent (構造体)
├── Timestamp              # タイムスタンプ（FDateTime + ゲーム時間）
├── Category               # カテゴリ（EDebugEventCategory）
├── Severity               # 重要度（EDebugEventSeverity）
├── Message                # メッセージ
├── SourceActor            # 発生元Actor（WeakPtr）
├── TargetActor            # 対象Actor（WeakPtr）
├── AdditionalData         # 追加データ（TMap<FString, FString>）
└── FrameNumber            # フレーム番号

UEventLogWidget (UUserWidget)
├── EventListView          # イベントリスト
├── FilterPanel            # フィルタパネル
├── SearchBox              # 検索ボックス
└── ControlButtons         # 制御ボタン

UEventLogBlueprintLibrary (UBlueprintFunctionLibrary)
├── LogDebugEvent()        # BPからイベント記録
├── ToggleEventLogUI()     # UI表示切替
└── ExportEventLog()       # エクスポート
```

### モジュール構成

```
DebugEventLogViewer/
├── DebugEventLogViewer.uplugin
├── Content/
│   └── UI/
│       └── WBP_EventLogViewer.uasset  # UIウィジェット
└── Source/
    └── DebugEventLogViewer/
        ├── DebugEventLogViewer.Build.cs
        ├── Public/
        │   ├── DebugEventLogViewerModule.h
        │   ├── EventLogSubsystem.h
        │   ├── DebugEvent.h
        │   ├── EventLogWidget.h
        │   └── EventLogBlueprintLibrary.h
        └── Private/
            ├── DebugEventLogViewerModule.cpp
            ├── EventLogSubsystem.cpp
            ├── EventLogWidget.cpp
            ├── EventLogBlueprintLibrary.cpp
            └── Listeners/
                ├── CombatEventListener.cpp    # 戦闘イベント監視
                ├── StateEventListener.cpp     # ステート変化監視
                ├── AIEventListener.cpp        # AIイベント監視
                └── SystemEventListener.cpp    # システムイベント監視
```

## インターフェース

### イベント記録

```cpp
// カテゴリ定義
UENUM(BlueprintType)
enum class EDebugEventCategory : uint8
{
    Combat      UMETA(DisplayName = "Combat"),
    State       UMETA(DisplayName = "State"),
    AI          UMETA(DisplayName = "AI"),
    Input       UMETA(DisplayName = "Input"),
    System      UMETA(DisplayName = "System"),
    Custom      UMETA(DisplayName = "Custom")
};

// 重要度定義
UENUM(BlueprintType)
enum class EDebugEventSeverity : uint8
{
    Info        UMETA(DisplayName = "Info"),
    Warning     UMETA(DisplayName = "Warning"),
    Error       UMETA(DisplayName = "Error")
};

// イベント記録関数
UFUNCTION(BlueprintCallable, Category="Debug Event Log")
void LogEvent(
    EDebugEventCategory Category,
    EDebugEventSeverity Severity,
    const FString& Message,
    AActor* SourceActor = nullptr,
    AActor* TargetActor = nullptr,
    const TMap<FString, FString>& AdditionalData = TMap<FString, FString>()
);
```

### イベント取得

```cpp
// フィルタ構造体
USTRUCT(BlueprintType)
struct FEventLogFilter
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    TArray<EDebugEventCategory> Categories;

    UPROPERTY(BlueprintReadWrite)
    TArray<EDebugEventSeverity> Severities;

    UPROPERTY(BlueprintReadWrite)
    FString KeywordFilter;

    UPROPERTY(BlueprintReadWrite)
    TWeakObjectPtr<AActor> ActorFilter;

    UPROPERTY(BlueprintReadWrite)
    float TimeRangeSeconds = 0.0f;  // 0 = 無制限
};

// イベント取得
UFUNCTION(BlueprintCallable, Category="Debug Event Log")
TArray<FDebugEvent> GetEvents(const FEventLogFilter& Filter) const;
```

### UI制御

```cpp
// UI表示切替
UFUNCTION(BlueprintCallable, Category="Debug Event Log")
void ToggleEventLogUI();

// 一時停止/再開
UFUNCTION(BlueprintCallable, Category="Debug Event Log")
void SetPaused(bool bPaused);
```

## 振る舞い

### 正常系

1. **イベント記録**
   - イベントが発生するとリングバッファに記録
   - UIが表示中の場合はリアルタイム更新
   - デリゲートで外部に通知

2. **UI表示**
   - デフォルトキー（`F10`）でトグル
   - 新しいイベントは上部に追加
   - 自動スクロール（設定で無効化可能）

3. **フィルタリング**
   - フィルタ変更時に即座に表示更新
   - 複数フィルタはAND条件

### エラー時

1. **バッファオーバーフロー**
   - 最大件数超過時は古いイベントから削除
   - 警告は出さない（想定動作）

2. **Actor参照切れ**
   - 削除されたActorは「<Destroyed>」と表示
   - フィルタリングには影響しない

### パフォーマンス考慮

1. **高頻度イベント**
   - Inputイベントはデフォルト無効
   - スロットリング設定（同一イベントの連続記録を抑制）

2. **UI更新**
   - バッチ更新（フレームごとにまとめて更新）
   - 非表示時はUI更新をスキップ

## 制約・前提条件

### ビルド制限

```cpp
#if !UE_BUILD_SHIPPING
// イベントログ機能の実装
#endif
```

### 依存関係

- **Engine**: 基本機能
- **UMG**: UIウィジェット
- **Slate**: カスタムUIコンポーネント

### メモリ要件

- デフォルト: 最大1000件保持
- 1件あたり約500バイト
- 最大使用メモリ: 約500KB〜1MB

## 自動イベント収集

### Combat連携（Variant_Combat）

```cpp
// ICombatDamageableのApplyDamage時に自動記録
void ACombatCharacter::ApplyDamage(float Damage, AActor* DamageCauser, ...)
{
#if !UE_BUILD_SHIPPING
    if (auto* EventLog = GetGameInstance()->GetSubsystem<UEventLogSubsystem>())
    {
        EventLog->LogEvent(
            EDebugEventCategory::Combat,
            EDebugEventSeverity::Info,
            FString::Printf(TEXT("Damage: %.1f from %s"), Damage, *GetNameSafe(DamageCauser)),
            DamageCauser,
            this,
            {
                {TEXT("Damage"), FString::Printf(TEXT("%.1f"), Damage)},
                {TEXT("RemainingHP"), FString::Printf(TEXT("%.1f"), CurrentHP)}
            }
        );
    }
#endif
}
```

### AI連携

```cpp
// StateTree遷移時に自動記録
// BehaviorTree実行時に自動記録
// AIControllerのターゲット変更時に自動記録
```

## UI設計

### レイアウト

```
+--------------------------------------------------+
| Event Log                          [_][□][×]     |
+--------------------------------------------------+
| [▼Category] [▼Severity] [Search...      ] [Clear]|
+--------------------------------------------------+
| ● 12:34:56.789 [Combat] Player took 25 damage    |
| ● 12:34:56.500 [AI] Enemy_01 changed target      |
| ○ 12:34:55.123 [State] Player: Idle -> Running   |
| ● 12:34:54.000 [Combat] Enemy_02 spawned         |
| ...                                              |
+--------------------------------------------------+
| Items: 156 | Paused: No | [Export] [Copy]        |
+--------------------------------------------------+
```

### 色分け

- **Info**: 白/グレー
- **Warning**: 黄色
- **Error**: 赤

### カテゴリアイコン

- **Combat**: ⚔️ 剣
- **State**: 🔄 矢印
- **AI**: 🤖 ロボット
- **Input**: ⌨️ キーボード
- **System**: ⚙️ 歯車
- **Custom**: 📌 ピン

## 使用例

### コードからの使用

```cpp
// イベント記録
if (auto* EventLog = GetGameInstance()->GetSubsystem<UEventLogSubsystem>())
{
    EventLog->LogEvent(
        EDebugEventCategory::Custom,
        EDebugEventSeverity::Warning,
        TEXT("Player entered danger zone"),
        PlayerCharacter,
        DangerZoneActor
    );
}
```

### Blueprintからの使用

```
// ノード: Log Debug Event
// Category: Custom
// Severity: Info
// Message: "Quest objective completed"
// Source Actor: Self
```

### DebugConsoleCommands連携

```
# イベントログUI表示
ShowEventLog

# イベントログをファイルに出力
ExportEventLog events.json

# イベントログをクリア
ClearEventLog

# カテゴリでフィルタ
FilterEventLog Combat
```

## テスト計画

1. **ユニットテスト**
   - イベント記録と取得
   - フィルタリング
   - バッファオーバーフロー

2. **統合テスト**
   - Variant_Combatとの連携
   - UIの表示・操作
   - エクスポート機能

3. **パフォーマンステスト**
   - 高頻度イベント記録時のFPS影響
   - メモリ使用量

## 今後の拡張

1. **ネットワークログ**
   - マルチプレイ時のRPC呼び出し記録

2. **リプレイ連携**
   - イベントログとリプレイの同期

3. **Visual Logger連携**
   - UE標準のVisual Loggerとの統合
