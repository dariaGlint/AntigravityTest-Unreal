# Command Console System Plugin

UE5用の包括的なコマンドコンソールシステムプラグインです。チートコマンド、デバッグ機能、パラメータ調整機能を提供します。

## 機能

### 1. コマンドコンソールマネージャー
- カスタムコンソールコマンドの登録・管理
- 開発ビルド限定機能の制御
- コマンドヘルプシステム

### 2. チートコマンド
以下のチートコマンドが利用可能です（すべて開発ビルド限定）:

- `god [on|off]` - ゴッドモード（無敵）の切り替え
- `noclip [on|off]` - ノークリップモード（壁抜け）の切り替え
- `teleport <x> <y> <z>` - 指定座標へテレポート
- `give <item_name> [quantity]` - アイテムの付与
- `sethealth <value>` - プレイヤーの体力設定
- `killenemies` - すべての敵を倒す
- `timescale <value>` - ゲームの時間スケール設定
- `unlockall` - すべてのコンテンツをアンロック

### 3. デバッグ情報表示
以下のデバッグ情報表示コマンドが利用可能です:

- `showfps [on|off]` - FPS表示の切り替え
- `showmemory [on|off]` - メモリ使用量表示の切り替え
- `shownetwork [on|off]` - ネットワーク統計表示の切り替え
- `showai [on|off]` - AIデバッグ表示の切り替え
- `showphysics [on|off]` - 物理デバッグ表示の切り替え
- `showcollision [on|off]` - コリジョンデバッグ表示の切り替え
- `showall` - すべてのデバッグ情報を表示
- `hideall` - すべてのデバッグ情報を非表示
- `sysinfo` - システム情報をログに出力

### 4. パラメータ調整
以下のゲームパラメータ調整コマンドが利用可能です:

- `setspeed <multiplier>` - 移動速度の倍率設定
- `setgravity <scale>` - 重力スケール設定
- `setjump <multiplier>` - ジャンプ高さの倍率設定
- `setdamage <multiplier>` - ダメージ倍率設定
- `setexp <multiplier>` - 経験値獲得倍率設定
- `setrenderdist <distance>` - 描画距離設定
- `setfov <degrees>` - 視野角（FOV）設定
- `resetparams` - すべてのパラメータをデフォルトにリセット
- `listparams` - 現在のパラメータ値を一覧表示

## 使用方法

### プラグインの有効化
1. プロジェクトの `.uproject` ファイルにプラグインを追加するか、エディタのプラグインマネージャーから有効化します
2. プロジェクトを再ビルドします

### コマンドの実行
ゲーム内でコンソールを開き（通常は `~` キー）、コマンドを入力します。

例:
```
god on
teleport 0 0 100
setspeed 2.0
showfps on
```

### Blueprint/C++からの使用
```cpp
// C++の例
UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
if (Manager)
{
    Manager->ExecuteCommand(TEXT("god on"));
}
```

### カスタムコマンドの登録
```cpp
// C++でカスタムコマンドを登録
UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
if (Manager)
{
    Manager->RegisterCommand(
        TEXT("mycommand"),
        TEXT("My custom command description"),
        TEXT("mycommand <arg1> <arg2>"),
        true, // Dev build only
        FConsoleCommandDelegate::CreateLambda([](const TArray<FString>& Args) {
            // コマンド実行時の処理
            UE_LOG(LogTemp, Log, TEXT("My command executed!"));
        })
    );
}
```

## 開発ビルド限定機能

セキュリティ上の理由から、ほとんどのチート・デバッグコマンドは開発ビルド（Development/Debug）でのみ動作します。
シッピングビルドでは自動的に無効化されます。

## 注意事項

- このプラグインは主に開発・デバッグ目的で使用してください
- シッピングビルドでは機能が制限されます
- 一部の機能は、ゲーム固有のシステム（ダメージ、インベントリなど）との統合が必要です

## 技術仕様

- **UE Version**: 5.7+
- **Module Type**: Runtime
- **Dependencies**: Core, CoreUObject, Engine, InputCore, Slate, SlateCore, UMG

## ライセンス

Copyright Epic Games, Inc. All Rights Reserved.
