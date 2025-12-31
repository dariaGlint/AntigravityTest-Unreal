# Debug Console Commands Plugin

## 概要

開発者向けのコンソールコマンド集プラグイン。ゲームプレイのテストやデバッグを高速化するためのチートコマンドを提供する。

## 基本情報

| 項目 | 値 |
|------|-----|
| プラグイン名 | DebugConsoleCommands |
| モジュールタイプ | Runtime (DeveloperOnly) |
| ロードフェーズ | Default |
| 対象ビルド | Editor, Development のみ |

## 要件

### 必須要件

1. **コマンド登録システム**
   - コンソールコマンドを一元管理するサブシステム
   - コマンドの動的な登録・解除が可能
   - カテゴリ別のコマンド整理

2. **基本コマンドセット**
   - `God` - 無敵モード切替
   - `Heal [Amount]` - HP回復（引数省略で全回復）
   - `Kill` - プレイヤーを即死させる
   - `Teleport X Y Z` - 指定座標へテレポート
   - `SpawnActor <ClassName> [X Y Z]` - アクターをスポーン
   - `SetSpeed <Multiplier>` - 移動速度倍率変更
   - `SlowMo <Scale>` - ゲーム速度変更（0.1〜2.0）
   - `Fly` - 飛行モード切替
   - `Ghost` - ノークリップモード切替

3. **戦闘デバッグコマンド**（Variant_Combat連携）
   - `SetHP <Amount>` - HP設定
   - `SetMaxHP <Amount>` - 最大HP設定
   - `SpawnEnemy <EnemyType> [Count]` - 敵スポーン
   - `KillAllEnemies` - 全敵を即死
   - `ToggleAI` - AI有効/無効切替

4. **情報表示コマンド**
   - `ShowStats` - 現在のプレイヤーステータス表示
   - `ListCommands [Category]` - 利用可能なコマンド一覧
   - `Help <CommandName>` - コマンドのヘルプ表示

### オプション要件

1. **コマンド履歴**
   - 直近のコマンド履歴を保持
   - 上下キーで履歴呼び出し（UE標準機能を活用）

2. **コマンドエイリアス**
   - 短縮コマンドの定義（例: `tp` = `Teleport`）

3. **Blueprint拡張**
   - Blueprintからカスタムコマンドを追加可能

## アーキテクチャ

### クラス構成

```
UDebugCommandSubsystem (UGameInstanceSubsystem)
├── RegisterCommand()      # コマンド登録
├── UnregisterCommand()    # コマンド解除
├── ExecuteCommand()       # コマンド実行
└── GetCommandList()       # コマンド一覧取得

FDebugCommand (構造体)
├── Name                   # コマンド名
├── Description            # 説明
├── Category              # カテゴリ
├── Usage                 # 使用方法
└── ExecuteDelegate       # 実行デリゲート

UDebugCommandBlueprintLibrary (UBlueprintFunctionLibrary)
├── ExecuteDebugCommand() # BPからコマンド実行
└── IsDebugCommandsEnabled() # 有効判定
```

### モジュール構成

```
DebugConsoleCommands/
├── DebugConsoleCommands.uplugin
└── Source/
    └── DebugConsoleCommands/
        ├── DebugConsoleCommands.Build.cs
        ├── Public/
        │   ├── DebugConsoleCommandsModule.h
        │   ├── DebugCommandSubsystem.h
        │   ├── DebugCommand.h
        │   └── DebugCommandBlueprintLibrary.h
        └── Private/
            ├── DebugConsoleCommandsModule.cpp
            ├── DebugCommandSubsystem.cpp
            ├── DebugCommandBlueprintLibrary.cpp
            └── Commands/
                ├── PlayerCommands.cpp      # God, Heal, Kill等
                ├── MovementCommands.cpp    # Teleport, Fly, Ghost等
                ├── SpawnCommands.cpp       # SpawnActor, SpawnEnemy等
                ├── CombatCommands.cpp      # SetHP, KillAllEnemies等
                └── SystemCommands.cpp      # SlowMo, ShowStats等
```

## インターフェース

### コマンド登録

```cpp
// コマンドの登録
UFUNCTION(BlueprintCallable, Category="Debug Commands")
void RegisterCommand(
    const FString& Name,
    const FString& Description,
    const FString& Category,
    const FString& Usage,
    const FDebugCommandDelegate& ExecuteDelegate
);

// デリゲート定義
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FDebugCommandDelegate,
    const TArray<FString>&, Args,
    FString&, OutResult
);
```

### コマンド実行

```cpp
// コンソールからの実行（Execコマンド経由）
static bool Exec(UWorld* World, const TCHAR* Cmd, FOutputDevice& Ar);

// プログラムからの実行
UFUNCTION(BlueprintCallable, Category="Debug Commands")
bool ExecuteCommand(const FString& CommandLine, FString& OutResult);
```

## 振る舞い

### 正常系

1. **コマンド実行成功**
   - コマンドが認識され、正常に実行される
   - 結果メッセージがコンソールに出力される
   - 例: `God mode enabled`

2. **引数付きコマンド**
   - 引数が正しくパースされる
   - 省略可能な引数はデフォルト値を使用
   - 例: `Heal` → 全回復、`Heal 50` → 50回復

### エラー時

1. **コマンド未認識**
   - `Unknown debug command: <command>. Type 'ListCommands' for available commands.`

2. **引数エラー**
   - `Usage: <correct usage>`
   - 例: `Usage: Teleport X Y Z`

3. **実行不可**
   - 対象が存在しない場合: `No player character found`
   - 条件を満たさない場合: `Cannot execute in current state`

### エッジケース

1. **プレイヤー不在時**
   - プレイヤー関連コマンドは警告を出して無視

2. **無効なビルド**
   - Shippingビルドではコマンドが登録されない
   - `Debug commands are not available in this build`

## 制約・前提条件

### ビルド制限

```cpp
#if !UE_BUILD_SHIPPING
// デバッグコマンドの実装
#endif
```

### 依存関係

- **Engine**: 基本機能
- **GameplayTags**: コマンドカテゴリ管理（オプション）

### パフォーマンス要件

- コマンド登録/解除: 即時
- コマンド実行: 1フレーム以内に完了
- メモリ: コマンドあたり約1KB以下

## セキュリティ考慮

1. **Shippingビルド除外**
   - すべてのデバッグコマンドはShippingビルドで無効化

2. **ネットワーク考慮**
   - サーバー権限チェック（将来のマルチプレイ対応）
   - クライアントからの不正なコマンド実行防止

## 使用例

### コンソールからの使用

```
# 無敵モード有効化
God

# 座標(100, 200, 300)へテレポート
Teleport 100 200 300

# HPを50に設定
SetHP 50

# 敵を3体スポーン
SpawnEnemy BP_CombatEnemy 3

# ゲーム速度を半分に
SlowMo 0.5

# 利用可能なコマンド一覧
ListCommands

# Teleportコマンドのヘルプ
Help Teleport
```

### Blueprintからの使用

```cpp
// デバッグコマンドを実行
FString Result;
UDebugCommandBlueprintLibrary::ExecuteDebugCommand(
    GetWorld(),
    TEXT("SpawnEnemy BP_CombatEnemy 5"),
    Result
);
```

## テスト計画

1. **ユニットテスト**
   - 各コマンドの引数パース
   - 正常系/異常系の動作確認

2. **統合テスト**
   - コンソールからの実行
   - Blueprintからの実行
   - Variant_Combatとの連携

3. **ビルドテスト**
   - Developmentビルドで有効確認
   - Shippingビルドで無効確認

## 今後の拡張

1. **イベントログビューア連携**
   - コマンド実行履歴をログに記録

2. **セーブ/ロードデバッグ連携**
   - `SaveState <SlotName>` コマンド追加
   - `LoadState <SlotName>` コマンド追加

3. **ビジュアルコマンドパレット**
   - IMGUIベースのコマンド選択UI
