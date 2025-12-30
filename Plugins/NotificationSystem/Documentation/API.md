# API リファレンス

## NotificationManager

`UNotificationManager`は`GameInstanceSubsystem`として実装されており、ゲーム全体で通知を管理します。

### 取得方法

**Blueprint:**
```
Get Game Instance
└─ Get Subsystem (Class: NotificationManager)
```

**C++:**
```cpp
UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
```

### メソッド

#### ShowNotification

```cpp
void ShowNotification(const FNotificationData& NotificationData)
```

通知をキューに追加して表示します。

**パラメータ:**
- `NotificationData`: 表示する通知のデータ

**Blueprint:**
```
Show Notification
└─ Notification Data: (FNotificationData)
```

**C++:**
```cpp
FNotificationData Data;
Data.Title = FText::FromString(TEXT("タイトル"));
Data.Message = FText::FromString(TEXT("メッセージ"));
Manager->ShowNotification(Data);
```

#### ShowSimpleNotification

```cpp
void ShowSimpleNotification(
    const FText& Title,
    const FText& Message,
    ENotificationType Type = ENotificationType::Info,
    ENotificationPriority Priority = ENotificationPriority::Normal
)
```

簡易的な通知表示。基本的なパラメータのみで通知を表示できます。

**パラメータ:**
- `Title`: 通知のタイトル
- `Message`: 通知のメッセージ
- `Type`: 通知タイプ（デフォルト: Info）
- `Priority`: 優先度（デフォルト: Normal）

**Blueprint:**
```
Show Simple Notification
├─ Title: "タイトル"
├─ Message: "メッセージ"
├─ Type: Info
└─ Priority: Normal
```

**C++:**
```cpp
Manager->ShowSimpleNotification(
    FText::FromString(TEXT("タイトル")),
    FText::FromString(TEXT("メッセージ")),
    ENotificationType::Success,
    ENotificationPriority::High
);
```

#### ClearAllNotifications

```cpp
void ClearAllNotifications()
```

すべての通知（表示中とキュー内）をクリアします。

**Blueprint:**
```
Clear All Notifications
```

**C++:**
```cpp
Manager->ClearAllNotifications();
```

#### GetCurrentNotification

```cpp
UNotificationWidget* GetCurrentNotification() const
```

現在表示中の通知ウィジェットを取得します。表示中でない場合は`nullptr`を返します。

**戻り値:**
- 現在表示中の`UNotificationWidget`、または`nullptr`

**Blueprint:**
```
Get Current Notification
└─ Is Valid
    ├─ True: (現在表示中)
    └─ False: (表示中でない)
```

**C++:**
```cpp
UNotificationWidget* Current = Manager->GetCurrentNotification();
if (Current)
{
    // 現在表示中の通知がある
}
```

#### GetQueuedNotificationCount

```cpp
int32 GetQueuedNotificationCount() const
```

キュー内の通知数を取得します（表示中の通知は含まれません）。

**戻り値:**
- キュー内の通知数

**Blueprint:**
```
Get Queued Notification Count
└─ Print String
```

**C++:**
```cpp
int32 Count = Manager->GetQueuedNotificationCount();
UE_LOG(LogTemp, Log, TEXT("Queued notifications: %d"), Count);
```

#### SetNotificationWidgetClass

```cpp
void SetNotificationWidgetClass(TSubclassOf<UNotificationWidget> WidgetClass)
```

使用する通知ウィジェットクラスを設定します。

**パラメータ:**
- `WidgetClass`: `UNotificationWidget`を継承したWidget Blueprintクラス

**Blueprint:**
```
Set Notification Widget Class
└─ Widget Class: WBP_Notification
```

**C++:**
```cpp
Manager->SetNotificationWidgetClass(UWBP_Notification::StaticClass());
```

#### SetMaxSimultaneousNotifications

```cpp
void SetMaxSimultaneousNotifications(int32 MaxCount)
```

同時に表示する最大通知数を設定します（現在は1のみサポート）。

**パラメータ:**
- `MaxCount`: 最大同時表示数（最小値: 1）

**Blueprint:**
```
Set Max Simultaneous Notifications
└─ Max Count: 1
```

**C++:**
```cpp
Manager->SetMaxSimultaneousNotifications(1);
```

### イベント

#### OnNotificationShown

```cpp
FOnNotificationShown OnNotificationShown
```

通知が表示されたときに発火するイベント。

**パラメータ:**
- `NotificationData`: 表示された通知のデータ

**Blueprint:**
```
Event On Notification Shown
└─ Notification Data: (FNotificationData)
```

**C++:**
```cpp
Manager->OnNotificationShown.AddDynamic(this, &AMyClass::OnNotificationShownHandler);

void AMyClass::OnNotificationShownHandler(const FNotificationData& Data)
{
    UE_LOG(LogTemp, Log, TEXT("Notification shown: %s"), *Data.Title.ToString());
}
```

#### OnNotificationHidden

```cpp
FOnNotificationHidden OnNotificationHidden
```

通知が非表示になったときに発火するイベント。

**パラメータ:**
- `NotificationData`: 非表示になった通知のデータ

**Blueprint:**
```
Event On Notification Hidden
└─ Notification Data: (FNotificationData)
```

**C++:**
```cpp
Manager->OnNotificationHidden.AddDynamic(this, &AMyClass::OnNotificationHiddenHandler);

void AMyClass::OnNotificationHiddenHandler(const FNotificationData& Data)
{
    UE_LOG(LogTemp, Log, TEXT("Notification hidden: %s"), *Data.Title.ToString());
}
```

---

## NotificationWidget

`UNotificationWidget`は通知UIの基底クラスです。このクラスを継承してカスタム通知ウィジェットを作成します。

### メソッド

#### GetNotificationData

```cpp
const FNotificationData& GetNotificationData() const
```

現在の通知データを取得します。

**戻り値:**
- 通知データの参照

**Blueprint:**
```
Get Notification Data
└─ Break NotificationData
```

**C++:**
```cpp
const FNotificationData& Data = GetNotificationData();
```

#### StartFadeIn

```cpp
void StartFadeIn()
```

フェードインを開始します（通常は自動的に呼ばれます）。

**Blueprint:**
```
Start Fade In
```

**C++:**
```cpp
StartFadeIn();
```

#### StartFadeOut

```cpp
void StartFadeOut()
```

フェードアウトを開始します（通常は自動的に呼ばれます）。

**Blueprint:**
```
Start Fade Out
```

**C++:**
```cpp
StartFadeOut();
```

### イベント（Blueprintで実装）

#### OnNotificationShown

```cpp
void OnNotificationShown()
```

通知が表示されたときに呼ばれるイベント。サウンド再生などに使用します。

**Blueprint:**
```
Event On Notification Shown
└─ Play Sound 2D (NotificationSound)
```

#### OnNotificationHidden

```cpp
void OnNotificationHidden()
```

通知が非表示になったときに呼ばれるイベント。

**Blueprint:**
```
Event On Notification Hidden
└─ (クリーンアップ処理)
```

#### OnFadeIn

```cpp
void OnFadeIn(float Alpha)
```

フェードイン中に毎フレーム呼ばれるイベント。

**パラメータ:**
- `Alpha`: フェード進行度（0.0 → 1.0）

**Blueprint:**
```
Event On Fade In
├─ Set Render Opacity (Alpha)
└─ Set Render Scale (Make Vector2D: Alpha, Alpha)
```

#### OnFadeOut

```cpp
void OnFadeOut(float Alpha)
```

フェードアウト中に毎フレーム呼ばれるイベント。

**パラメータ:**
- `Alpha`: フェード進行度（1.0 → 0.0）

**Blueprint:**
```
Event On Fade Out
├─ Set Render Opacity (Alpha)
└─ Set Render Scale (Make Vector2D: Alpha, Alpha)
```

#### OnDataUpdated

```cpp
void OnDataUpdated(const FNotificationData& Data)
```

通知データが設定されたときに呼ばれるイベント。UI要素を更新します。

**パラメータ:**
- `Data`: 新しい通知データ

**Blueprint:**
```
Event On Data Updated
├─ Break NotificationData
│   ├─ Title → Set Text (TitleText)
│   ├─ Message → Set Text (MessageText)
│   └─ Type → Switch (色の設定など)
```

### デリゲート

#### OnFadeInComplete

```cpp
FOnFadeInComplete OnFadeInComplete
```

フェードイン完了時に発火するデリゲート。

**パラメータ:**
- `Widget`: 完了した通知ウィジェット

**C++:**
```cpp
Widget->OnFadeInComplete.AddDynamic(this, &AMyClass::OnFadeInCompleteHandler);
```

#### OnFadeOutComplete

```cpp
FOnFadeOutComplete OnFadeOutComplete
```

フェードアウト完了時に発火するデリゲート。

**パラメータ:**
- `Widget`: 完了した通知ウィジェット

**C++:**
```cpp
Widget->OnFadeOutComplete.AddDynamic(this, &AMyClass::OnFadeOutCompleteHandler);
```

---

## FNotificationData

通知データを保持する構造体。

### プロパティ

#### Title

```cpp
FText Title
```

通知のタイトル。

**Blueprint:** 直接設定可能
**C++:** `Data.Title = FText::FromString(TEXT("タイトル"));`

#### Message

```cpp
FText Message
```

通知のメッセージ本文。

**Blueprint:** 直接設定可能
**C++:** `Data.Message = FText::FromString(TEXT("メッセージ"));`

#### Type

```cpp
ENotificationType Type
```

通知タイプ。

**値:**
- `Info`: 情報
- `Success`: 成功
- `Warning`: 警告
- `Error`: エラー
- `Achievement`: 実績
- `Custom`: カスタム

**Blueprint:** 直接設定可能
**C++:** `Data.Type = ENotificationType::Success;`

#### Priority

```cpp
ENotificationPriority Priority
```

通知優先度。

**値:**
- `Low`: 低
- `Normal`: 通常
- `High`: 高
- `Critical`: 緊急

**Blueprint:** 直接設定可能
**C++:** `Data.Priority = ENotificationPriority::High;`

#### DisplayDuration

```cpp
float DisplayDuration
```

表示時間（秒）。デフォルト: 3.0

**Blueprint:** 直接設定可能
**C++:** `Data.DisplayDuration = 5.0f;`

#### FadeInDuration

```cpp
float FadeInDuration
```

フェードイン時間（秒）。デフォルト: 0.3

**Blueprint:** 直接設定可能
**C++:** `Data.FadeInDuration = 0.5f;`

#### FadeOutDuration

```cpp
float FadeOutDuration
```

フェードアウト時間（秒）。デフォルト: 0.3

**Blueprint:** 直接設定可能
**C++:** `Data.FadeOutDuration = 0.5f;`

#### IconTexture

```cpp
UTexture2D* IconTexture
```

アイコンテクスチャ（オプション）。

**Blueprint:** 直接設定可能
**C++:** `Data.IconTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/Icon"));`

#### CustomData

```cpp
TMap<FString, FString> CustomData
```

カスタムデータ用のキー・バリューマップ。

**Blueprint:**
```
Add (Custom Data)
├─ Key: "Level"
└─ Value: "10"
```

**C++:**
```cpp
Data.CustomData.Add(TEXT("Level"), TEXT("10"));
Data.CustomData.Add(TEXT("Experience"), TEXT("1000"));
```

---

## ENotificationType

通知タイプの列挙型。

### 値

| 値 | 説明 | 推奨用途 |
|----|------|----------|
| `Info` | 情報 | 一般的な情報、Tips |
| `Success` | 成功 | アイテム取得、クエスト完了 |
| `Warning` | 警告 | 注意喚起、軽度の問題 |
| `Error` | エラー | エラー、失敗 |
| `Achievement` | 実績 | 実績解除、マイルストーン |
| `Custom` | カスタム | 独自の用途 |

---

## ENotificationPriority

通知優先度の列挙型。

### 値

| 値 | 説明 | 推奨用途 |
|----|------|----------|
| `Low` | 低 | Tips、ヒント |
| `Normal` | 通常 | 通常のゲームイベント |
| `High` | 高 | 重要なイベント、警告 |
| `Critical` | 緊急 | 致命的なエラー、即座の対応が必要 |

優先度が高いほど、キュー内で先に表示されます。同じ優先度の場合は、古い順に表示されます。
