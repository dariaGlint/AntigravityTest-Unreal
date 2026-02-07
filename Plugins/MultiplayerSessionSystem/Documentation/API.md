# API リファレンス

## UMultiplayerSessionManager

Game Instance Subsystem として実装されたセッション管理クラス。

### 公開メソッド

#### CreateSession
```cpp
void CreateSession(const FSessionSettings& Settings)
```
新しいマルチプレイヤーセッションを作成します。

**パラメータ:**
- `Settings` - セッション設定

**デリゲート:**
- `OnCreateSessionComplete` - 完了時に呼び出されます

---

#### FindSessions
```cpp
void FindSessions(int32 MaxSearchResults = 10, bool bIsLAN = false)
```
利用可能なセッションを検索します。

**パラメータ:**
- `MaxSearchResults` - 検索結果の最大数
- `bIsLAN` - LAN セッションのみを検索するか

**デリゲート:**
- `OnFindSessionsComplete` - 完了時に呼び出されます

---

#### JoinSession
```cpp
void JoinSession(int32 SessionIndex)
```
指定されたインデックスのセッションに参加します。

**パラメータ:**
- `SessionIndex` - 検索結果配列内のセッションのインデックス

**デリゲート:**
- `OnJoinSessionComplete` - 完了時に呼び出されます

---

#### DestroySession
```cpp
void DestroySession()
```
現在のセッションを破棄します。

**デリゲート:**
- `OnDestroySessionComplete` - 完了時に呼び出されます

---

#### StartSession
```cpp
void StartSession()
```
セッションを開始状態にします。

**デリゲート:**
- `OnStartSessionComplete` - 完了時に呼び出されます

---

#### LeaveSession
```cpp
void LeaveSession()
```
現在のセッションから退出します。`DestroySession()` のエイリアス。

---

#### GetSessionPlayers
```cpp
TArray<FSessionPlayerInfo> GetSessionPlayers() const
```
現在のセッション内のプレイヤーリストを取得します。

**戻り値:**
- プレイヤー情報の配列

---

#### IsHost
```cpp
bool IsHost() const
```
現在のプレイヤーがセッションのホストかどうかを確認します。

**戻り値:**
- ホストの場合は true

---

#### GetCurrentState
```cpp
ESessionState GetCurrentState() const
```
現在のセッション状態を取得します。

**戻り値:**
- 現在の `ESessionState`

---

#### InitiateHostMigration
```cpp
void InitiateHostMigration()
```
ホスト移行プロセスを開始します。

**デリゲート:**
- `OnHostMigrationComplete` - 完了時に呼び出されます

---

### デリゲート

#### OnCreateSessionComplete
```cpp
FOnCreateSessionComplete OnCreateSessionComplete
```
セッション作成が完了したときに呼び出されます。

**パラメータ:**
- `bool bWasSuccessful` - 作成が成功したかどうか

---

#### OnFindSessionsComplete
```cpp
FOnFindSessionsComplete OnFindSessionsComplete
```
セッション検索が完了したときに呼び出されます。

**パラメータ:**
- `const TArray<FSessionRoomInfo>& SessionResults` - 見つかったセッションの配列
- `bool bWasSuccessful` - 検索が成功したかどうか

---

#### OnJoinSessionComplete
```cpp
FOnJoinSessionComplete OnJoinSessionComplete
```
セッション参加が完了したときに呼び出されます。

**パラメータ:**
- `bool bWasSuccessful` - 参加が成功したかどうか

---

#### OnDestroySessionComplete
```cpp
FOnDestroySessionComplete OnDestroySessionComplete
```
セッション破棄が完了したときに呼び出されます。

---

#### OnStartSessionComplete
```cpp
FOnStartSessionComplete OnStartSessionComplete
```
セッション開始が完了したときに呼び出されます。

**パラメータ:**
- `bool bWasSuccessful` - 開始が成功したかどうか

---

#### OnPlayerJoined
```cpp
FOnPlayerJoined OnPlayerJoined
```
プレイヤーがセッションに参加したときに呼び出されます。

**パラメータ:**
- `const FString& PlayerName` - プレイヤー名
- `int32 PlayerCount` - 現在のプレイヤー数

---

#### OnPlayerLeft
```cpp
FOnPlayerLeft OnPlayerLeft
```
プレイヤーがセッションから退出したときに呼び出されます。

**パラメータ:**
- `const FString& PlayerName` - プレイヤー名
- `int32 PlayerCount` - 現在のプレイヤー数

---

#### OnHostMigrationComplete
```cpp
FOnHostMigrationComplete OnHostMigrationComplete
```
ホスト移行が完了したときに呼び出されます。

**パラメータ:**
- `bool bWasSuccessful` - 移行が成功したかどうか

---

## USessionBlueprintLibrary

Blueprint から簡単にアクセスするための関数ライブラリ。

### GetSessionManager
```cpp
static UMultiplayerSessionManager* GetSessionManager(const UObject* WorldContextObject)
```
現在の Game Instance から Session Manager を取得します。

**パラメータ:**
- `WorldContextObject` - ワールドコンテキスト

**戻り値:**
- Session Manager インスタンス

---

### QuickCreateSession
```cpp
static void QuickCreateSession(const UObject* WorldContextObject, const FString& RoomName, int32 MaxPlayers = 4, bool bIsLAN = false)
```
簡易的なセッション作成関数。

**パラメータ:**
- `WorldContextObject` - ワールドコンテキスト
- `RoomName` - ルーム名
- `MaxPlayers` - 最大プレイヤー数
- `bIsLAN` - LAN モードかどうか

---

### QuickFindSessions
```cpp
static void QuickFindSessions(const UObject* WorldContextObject, int32 MaxResults = 10, bool bIsLAN = false)
```
簡易的なセッション検索関数。

**パラメータ:**
- `WorldContextObject` - ワールドコンテキスト
- `MaxResults` - 最大検索結果数
- `bIsLAN` - LAN モードかどうか

---

### MakeSessionSettings
```cpp
static FSessionSettings MakeSessionSettings(
    const FString& RoomName,
    int32 MaxPlayers = 4,
    bool bIsLAN = false,
    bool bUsesPresence = true,
    bool bAllowInvites = true,
    const FString& MapName = TEXT(""),
    const FString& GameMode = TEXT("")
)
```
セッション設定構造体を作成します。

**パラメータ:**
- `RoomName` - ルーム名
- `MaxPlayers` - 最大プレイヤー数
- `bIsLAN` - LAN モードかどうか
- `bUsesPresence` - プレゼンス機能を使用するか
- `bAllowInvites` - 招待を許可するか
- `MapName` - マップ名
- `GameMode` - ゲームモード

**戻り値:**
- 設定された `FSessionSettings` 構造体

---

## データ構造

### ESessionState

セッションの状態を表す列挙型。

```cpp
enum class ESessionState : uint8
{
    Idle,           // アイドル状態
    Creating,       // セッション作成中
    Searching,      // セッション検索中
    Joining,        // セッション参加中
    InSession,      // セッション内
    Leaving,        // セッション退出中
    Error           // エラー状態
};
```

---

### FSessionSettings

セッション作成時の設定を保持する構造体。

```cpp
struct FSessionSettings
{
    FString RoomName;               // ルーム名
    int32 MaxPlayers;               // 最大プレイヤー数
    bool bIsLAN;                    // LAN モードかどうか
    bool bUsesPresence;             // プレゼンス機能を使用するか
    bool bAllowInvites;             // 招待を許可するか
    bool bIsPasswordProtected;      // パスワード保護されているか
    FString Password;               // パスワード
    FString MapName;                // マップ名
    FString GameMode;               // ゲームモード
};
```

---

### FSessionRoomInfo

検索結果のセッション情報を保持する構造体。

```cpp
struct FSessionRoomInfo
{
    FString RoomName;               // ルーム名
    FString HostName;               // ホスト名
    int32 CurrentPlayers;           // 現在のプレイヤー数
    int32 MaxPlayers;               // 最大プレイヤー数
    int32 Ping;                     // Ping 値
    bool bIsLAN;                    // LAN モードかどうか
    bool bIsPasswordProtected;      // パスワード保護されているか
    FString MapName;                // マップ名
    FString GameMode;               // ゲームモード
};
```

---

### FSessionPlayerInfo

セッション内のプレイヤー情報を保持する構造体。

```cpp
struct FSessionPlayerInfo
{
    FString PlayerName;             // プレイヤー名
    FUniqueNetIdRepl UniqueId;      // ユニーク ID
    bool bIsHost;                   // ホストかどうか
    int32 Ping;                     // Ping 値
};
```

---

## 使用例

### C++ での基本的な使用

```cpp
#include "MultiplayerSessionManager.h"

void AMyGameMode::CreateMultiplayerSession()
{
    UMultiplayerSessionManager* SessionManager = GetGameInstance()->GetSubsystem<UMultiplayerSessionManager>();

    if (SessionManager)
    {
        FSessionSettings Settings;
        Settings.RoomName = TEXT("MyRoom");
        Settings.MaxPlayers = 4;

        SessionManager->OnCreateSessionComplete.AddDynamic(this, &AMyGameMode::OnSessionCreated);
        SessionManager->CreateSession(Settings);
    }
}

void AMyGameMode::OnSessionCreated(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session created successfully!"));
    }
}
```

### Blueprint での基本的な使用

1. `Get Session Manager` ノードを使用してマネージャーを取得
2. `Make Session Settings` でセッション設定を作成
3. `Create Session` でセッションを作成
4. `On Create Session Complete` イベントをバインド
