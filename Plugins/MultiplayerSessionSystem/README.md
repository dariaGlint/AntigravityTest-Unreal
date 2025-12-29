# Multiplayer Session System

包括的なマルチプレイヤーセッション管理システムプラグインです。

## 機能

### ルーム作成/検索
- オンラインセッションの作成
- セッション検索機能
- カスタムセッション設定（最大プレイヤー数、LANモード、パスワード保護など）

### プレイヤー参加/退出処理
- セッションへの参加
- セッションからの退出
- プレイヤーリストの管理と同期

### ホスト移行ロジック
- ホストが切断した際の自動移行
- 新しいホストの選択と通知

### セッション情報の同期
- セッション状態の同期
- プレイヤー情報の同期
- リアルタイム更新

## 使い方

### C++での使用

```cpp
#include "MultiplayerSessionManager.h"

// ゲームインスタンスからマネージャーを取得
UMultiplayerSessionManager* SessionManager = GetGameInstance()->GetSubsystem<UMultiplayerSessionManager>();

// セッション設定の作成
FSessionSettings Settings;
Settings.RoomName = TEXT("MyRoom");
Settings.MaxPlayers = 4;
Settings.bIsLAN = false;

// セッションの作成
SessionManager->CreateSession(Settings);

// デリゲートのバインド
SessionManager->OnCreateSessionComplete.AddDynamic(this, &AMyClass::OnSessionCreated);
```

### Blueprintでの使用

1. **セッションマネージャーの取得**
   ```
   Get Session Manager ノードを使用
   ```

2. **セッションの作成**
   ```
   Quick Create Session ノードを使用
   または
   Make Session Settings → Create Session
   ```

3. **セッションの検索**
   ```
   Quick Find Sessions ノードを使用
   または
   Find Sessions ノードを使用
   ```

4. **イベントのバインド**
   - On Create Session Complete
   - On Find Sessions Complete
   - On Join Session Complete
   - On Player Joined
   - On Player Left
   - On Host Migration Complete

## API リファレンス

### UMultiplayerSessionManager

#### 主要な関数

- `CreateSession(const FSessionSettings& Settings)` - 新しいセッションを作成
- `FindSessions(int32 MaxSearchResults, bool bIsLAN)` - セッションを検索
- `JoinSession(int32 SessionIndex)` - セッションに参加
- `DestroySession()` - セッションを破棄
- `StartSession()` - セッションを開始
- `LeaveSession()` - セッションから退出
- `GetSessionPlayers()` - セッション内のプレイヤーリストを取得
- `IsHost()` - 現在のプレイヤーがホストかどうかを確認
- `InitiateHostMigration()` - ホスト移行を開始

#### デリゲート

- `OnCreateSessionComplete` - セッション作成完了時
- `OnFindSessionsComplete` - セッション検索完了時
- `OnJoinSessionComplete` - セッション参加完了時
- `OnDestroySessionComplete` - セッション破棄完了時
- `OnStartSessionComplete` - セッション開始完了時
- `OnPlayerJoined` - プレイヤー参加時
- `OnPlayerLeft` - プレイヤー退出時
- `OnHostMigrationComplete` - ホスト移行完了時

### データ構造

#### FSessionSettings
- RoomName - ルーム名
- MaxPlayers - 最大プレイヤー数
- bIsLAN - LANモードかどうか
- bUsesPresence - プレゼンス機能を使用するか
- bAllowInvites - 招待を許可するか
- bIsPasswordProtected - パスワード保護されているか
- Password - パスワード
- MapName - マップ名
- GameMode - ゲームモード

#### FSessionRoomInfo
- RoomName - ルーム名
- HostName - ホスト名
- CurrentPlayers - 現在のプレイヤー数
- MaxPlayers - 最大プレイヤー数
- Ping - Ping値
- bIsLAN - LANモードかどうか
- bIsPasswordProtected - パスワード保護されているか
- MapName - マップ名
- GameMode - ゲームモード

#### FSessionPlayerInfo
- PlayerName - プレイヤー名
- UniqueId - ユニークID
- bIsHost - ホストかどうか
- Ping - Ping値

## 必要な設定

### DefaultEngine.ini

```ini
[OnlineSubsystem]
DefaultPlatformService=Null

[OnlineSubsystemNull]
bEnabled=true

[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/OnlineSubsystemUtils.IpNetDriver",DriverClassNameFallback="/Script/OnlineSubsystemUtils.IpNetDriver")
```

## ライセンス

このプラグインはAntigravityTestプロジェクトの一部です。
