# 使用方法

## 基本的な使い方

### 1. セッションの作成

```cpp
// C++
UMultiplayerSessionManager* SessionManager = GetGameInstance()->GetSubsystem<UMultiplayerSessionManager>();

FSessionSettings Settings;
Settings.RoomName = TEXT("MyAwesomeRoom");
Settings.MaxPlayers = 4;
Settings.bIsLAN = false;
Settings.MapName = TEXT("/Game/Maps/MainMap");
Settings.GameMode = TEXT("FFA");

SessionManager->OnCreateSessionComplete.AddDynamic(this, &AMyGameMode::OnSessionCreated);
SessionManager->CreateSession(Settings);
```

### 2. セッションの検索

```cpp
// C++
SessionManager->OnFindSessionsComplete.AddDynamic(this, &AMyGameMode::OnSessionsFound);
SessionManager->FindSessions(10, false); // 最大10件、非LANモード
```

```cpp
// 検索結果の処理
void AMyGameMode::OnSessionsFound(const TArray<FSessionRoomInfo>& Sessions, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        for (const FSessionRoomInfo& Session : Sessions)
        {
            UE_LOG(LogTemp, Log, TEXT("Found session: %s (%d/%d players)"),
                *Session.RoomName, Session.CurrentPlayers, Session.MaxPlayers);
        }
    }
}
```

### 3. セッションへの参加

```cpp
// C++
SessionManager->OnJoinSessionComplete.AddDynamic(this, &AMyGameMode::OnSessionJoined);
SessionManager->JoinSession(0); // 検索結果の最初のセッションに参加
```

### 4. セッションからの退出

```cpp
// C++
SessionManager->OnDestroySessionComplete.AddDynamic(this, &AMyGameMode::OnSessionLeft);
SessionManager->LeaveSession();
```

## Blueprint での使用例

### セッション作成フロー

1. **セッション設定の作成**
   - `Make Session Settings` ノードを使用
   - Room Name, Max Players などを設定

2. **セッションマネージャーの取得**
   - `Get Session Manager` ノードを使用

3. **セッションの作成**
   - `Create Session` ノードに設定を接続
   - `On Create Session Complete` デリゲートをバインド

4. **完了イベントの処理**
   - 成功時: ゲームを開始
   - 失敗時: エラーメッセージを表示

### セッション検索フロー

1. **セッション検索の開始**
   - `Quick Find Sessions` または `Find Sessions` を使用
   - Max Results と Is LAN を設定

2. **検索結果の処理**
   - `On Find Sessions Complete` イベントをバインド
   - Session Results 配列を UI に表示

3. **セッションへの参加**
   - ユーザーが選択したセッションのインデックスを取得
   - `Join Session` を呼び出し

## 高度な使用例

### ホスト移行の実装

```cpp
// ホストの切断を検知
void AMyGameMode::OnHostDisconnected()
{
    UMultiplayerSessionManager* SessionManager = GetGameInstance()->GetSubsystem<UMultiplayerSessionManager>();

    if (!SessionManager->IsHost())
    {
        // クライアントの場合、ホスト移行を待機
        SessionManager->OnHostMigrationComplete.AddDynamic(this, &AMyGameMode::OnHostMigrationCompleted);
    }
    else
    {
        // 新しいホストの場合、移行処理を開始
        SessionManager->InitiateHostMigration();
    }
}

void AMyGameMode::OnHostMigrationCompleted(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Host migration successful"));
        // 新しいホストに接続
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Host migration failed"));
        // セッションから退出
    }
}
```

### プレイヤー参加/退出の監視

```cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    UMultiplayerSessionManager* SessionManager = GetGameInstance()->GetSubsystem<UMultiplayerSessionManager>();

    SessionManager->OnPlayerJoined.AddDynamic(this, &AMyGameMode::OnPlayerJoinedSession);
    SessionManager->OnPlayerLeft.AddDynamic(this, &AMyGameMode::OnPlayerLeftSession);
}

void AMyGameMode::OnPlayerJoinedSession(const FString& PlayerName, int32 PlayerCount)
{
    UE_LOG(LogTemp, Log, TEXT("%s joined the session. Total players: %d"), *PlayerName, PlayerCount);
    // UI を更新
    UpdatePlayerListUI();
}

void AMyGameMode::OnPlayerLeftSession(const FString& PlayerName, int32 PlayerCount)
{
    UE_LOG(LogTemp, Log, TEXT("%s left the session. Total players: %d"), *PlayerName, PlayerCount);
    // UI を更新
    UpdatePlayerListUI();
}
```

### カスタムマッチメイキング

```cpp
void AMyGameMode::CreateCustomSession()
{
    FSessionSettings Settings;
    Settings.RoomName = FString::Printf(TEXT("Room_%d"), FMath::RandRange(1000, 9999));
    Settings.MaxPlayers = 8;
    Settings.bIsLAN = false;
    Settings.bUsesPresence = true;
    Settings.bAllowInvites = true;
    Settings.MapName = TEXT("/Game/Maps/Arena");
    Settings.GameMode = TEXT("TeamDeathmatch");

    // パスワード保護の設定
    Settings.bIsPasswordProtected = true;
    Settings.Password = TEXT("secret123");

    UMultiplayerSessionManager* SessionManager = GetGameInstance()->GetSubsystem<UMultiplayerSessionManager>();
    SessionManager->CreateSession(Settings);
}
```

## トラブルシューティング

### セッションが見つからない

1. DefaultEngine.ini の OnlineSubsystem 設定を確認
2. ファイアウォール設定を確認
3. Steam などのプラットフォームサービスが有効か確認

### セッションに参加できない

1. セッションの最大プレイヤー数を確認
2. パスワード保護されていないか確認
3. ネットワーク接続を確認

### ホスト移行が失敗する

1. 少なくとも2人以上のプレイヤーがセッションにいるか確認
2. ネットワーク接続が安定しているか確認
