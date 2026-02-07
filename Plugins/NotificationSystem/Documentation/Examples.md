# サンプルコード集

このドキュメントでは、NotificationSystemの実用的なサンプルコードを紹介します。

## 基本的な使用例

### シンプルな通知

**Blueprint:**
```
Get Game Instance
└─ Get Subsystem (Notification Manager)
    └─ Show Simple Notification
        ├─ Title: "通知"
        ├─ Message: "これはシンプルな通知です"
        ├─ Type: Info
        └─ Priority: Normal
```

**C++:**
```cpp
void AMyPlayerController::ShowSimpleNotification()
{
    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->ShowSimpleNotification(
            FText::FromString(TEXT("通知")),
            FText::FromString(TEXT("これはシンプルな通知です")),
            ENotificationType::Info,
            ENotificationPriority::Normal
        );
    }
}
```

### 詳細設定の通知

**Blueprint:**
```
Make NotificationData
├─ Title: "重要な通知"
├─ Message: "この通知は5秒間表示されます"
├─ Type: Warning
├─ Priority: High
├─ Display Duration: 5.0
├─ Fade In Duration: 0.5
└─ Fade Out Duration: 0.5

Show Notification
```

**C++:**
```cpp
void AMyPlayerController::ShowDetailedNotification()
{
    FNotificationData Data;
    Data.Title = FText::FromString(TEXT("重要な通知"));
    Data.Message = FText::FromString(TEXT("この通知は5秒間表示されます"));
    Data.Type = ENotificationType::Warning;
    Data.Priority = ENotificationPriority::High;
    Data.DisplayDuration = 5.0f;
    Data.FadeInDuration = 0.5f;
    Data.FadeOutDuration = 0.5f;

    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->ShowNotification(Data);
    }
}
```

## ゲームイベントでの使用例

### アイテム取得

**Blueprint:**
```
Event On Item Picked Up
├─ Get Item Name
└─ Get Game Instance
    └─ Get Subsystem (Notification Manager)
        └─ Show Simple Notification
            ├─ Title: "アイテム取得"
            ├─ Message: Format Text ("{0}を入手しました", Item Name)
            ├─ Type: Success
            └─ Priority: Normal
```

**C++:**
```cpp
void AMyCharacter::OnItemPickedUp(const FString& ItemName)
{
    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        FText Message = FText::Format(
            FText::FromString(TEXT("{0}を入手しました")),
            FText::FromString(ItemName)
        );

        Manager->ShowSimpleNotification(
            FText::FromString(TEXT("アイテム取得")),
            Message,
            ENotificationType::Success,
            ENotificationPriority::Normal
        );
    }
}
```

### レベルアップ

**Blueprint:**
```
Event On Level Up
├─ Get New Level
└─ Make NotificationData
    ├─ Title: "レベルアップ!"
    ├─ Message: Format Text ("レベル {0} に到達しました", New Level)
    ├─ Type: Achievement
    ├─ Priority: High
    ├─ Display Duration: 5.0
    └─ Custom Data
        ├─ Add (Key: "Level", Value: ToString(New Level))
        └─ Add (Key: "Event", Value: "LevelUp")
    └─ Show Notification
```

**C++:**
```cpp
void AMyCharacter::OnLevelUp(int32 NewLevel)
{
    FNotificationData Data;
    Data.Title = FText::FromString(TEXT("レベルアップ!"));
    Data.Message = FText::Format(
        FText::FromString(TEXT("レベル {0} に到達しました")),
        FText::AsNumber(NewLevel)
    );
    Data.Type = ENotificationType::Achievement;
    Data.Priority = ENotificationPriority::High;
    Data.DisplayDuration = 5.0f;
    Data.CustomData.Add(TEXT("Level"), FString::FromInt(NewLevel));
    Data.CustomData.Add(TEXT("Event"), TEXT("LevelUp"));

    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->ShowNotification(Data);
    }
}
```

### 体力低下警告

**Blueprint:**
```
Event On Health Changed
├─ Get Health Percentage
└─ Branch (< 0.25)
    └─ True:
        └─ Make NotificationData
            ├─ Title: "警告"
            ├─ Message: "体力が少なくなっています!"
            ├─ Type: Warning
            ├─ Priority: High
            └─ Show Notification
```

**C++:**
```cpp
void AMyCharacter::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
    float HealthPercentage = CurrentHealth / MaxHealth;

    if (HealthPercentage < 0.25f && !bLowHealthWarningShown)
    {
        FNotificationData Data;
        Data.Title = FText::FromString(TEXT("警告"));
        Data.Message = FText::FromString(TEXT("体力が少なくなっています!"));
        Data.Type = ENotificationType::Warning;
        Data.Priority = ENotificationPriority::High;

        UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
        if (Manager)
        {
            Manager->ShowNotification(Data);
        }

        bLowHealthWarningShown = true;
    }
    else if (HealthPercentage >= 0.5f)
    {
        bLowHealthWarningShown = false;
    }
}
```

### クエスト完了

**Blueprint:**
```
Event On Quest Completed
├─ Get Quest Name
└─ Make NotificationData
    ├─ Title: "クエスト完了"
    ├─ Message: Format Text ("{0}を達成しました", Quest Name)
    ├─ Type: Success
    ├─ Priority: High
    ├─ Icon Texture: Quest Icon
    └─ Show Notification
```

**C++:**
```cpp
void AMyGameMode::OnQuestCompleted(const FString& QuestName, UTexture2D* QuestIcon)
{
    FNotificationData Data;
    Data.Title = FText::FromString(TEXT("クエスト完了"));
    Data.Message = FText::Format(
        FText::FromString(TEXT("{0}を達成しました")),
        FText::FromString(QuestName)
    );
    Data.Type = ENotificationType::Success;
    Data.Priority = ENotificationPriority::High;
    Data.IconTexture = QuestIcon;

    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->ShowNotification(Data);
    }
}
```

## カスタムウィジェットの実装例

### タイプ別の色とアイコン

**Blueprint (WBP_Notification):**

Event Graph:
```
Event On Data Updated
├─ Get Notification Data
│   ├─ Break NotificationData
│   │   ├─ Title → Set Text (TitleText)
│   │   ├─ Message → Set Text (MessageText)
│   │   └─ Type → Switch
│   │       ├─ Info:
│   │       │   ├─ Set Brush Tint (BackgroundBorder, Blue)
│   │       │   └─ Set Brush from Texture (IconImage, InfoIcon)
│   │       ├─ Success:
│   │       │   ├─ Set Brush Tint (BackgroundBorder, Green)
│   │       │   └─ Set Brush from Texture (IconImage, SuccessIcon)
│   │       ├─ Warning:
│   │       │   ├─ Set Brush Tint (BackgroundBorder, Yellow)
│   │       │   └─ Set Brush from Texture (IconImage, WarningIcon)
│   │       ├─ Error:
│   │       │   ├─ Set Brush Tint (BackgroundBorder, Red)
│   │       │   └─ Set Brush from Texture (IconImage, ErrorIcon)
│   │       └─ Achievement:
│   │           ├─ Set Brush Tint (BackgroundBorder, Gold)
│   │           └─ Set Brush from Texture (IconImage, AchievementIcon)
```

### フェードとスライドのアニメーション

**Blueprint (WBP_Notification):**

Event Graph:
```
Event On Fade In
├─ Set Render Opacity (Alpha)
├─ Make Vector2D (X: 1.0 - Alpha * 0.2, Y: 1.0)
│   └─ Set Render Transform Translation
└─ Set Render Scale (Make Vector2D: Alpha, Alpha)

Event On Fade Out
├─ Set Render Opacity (Alpha)
├─ Make Vector2D (X: (1.0 - Alpha) * 0.2, Y: 1.0)
│   └─ Set Render Transform Translation
└─ Set Render Scale (Make Vector2D: Alpha, Alpha)
```

### サウンド付き通知

**Blueprint (WBP_Notification):**

Event Graph:
```
Event On Notification Shown
└─ Get Notification Data
    └─ Break NotificationData
        └─ Type → Switch
            ├─ Info: Play Sound 2D (InfoSound)
            ├─ Success: Play Sound 2D (SuccessSound)
            ├─ Warning: Play Sound 2D (WarningSound)
            ├─ Error: Play Sound 2D (ErrorSound)
            └─ Achievement: Play Sound 2D (AchievementSound)
```

## イベントのバインド

### 統計情報の追跡

**Blueprint (GameInstance):**
```
Event Init
└─ Get Subsystem (Notification Manager)
    ├─ Bind Event to On Notification Shown
    │   └─ Event: On Notification Shown Handler
    └─ Bind Event to On Notification Hidden
        └─ Event: On Notification Hidden Handler

Custom Event: On Notification Shown Handler
├─ Increment Notification Count
└─ Print String ("Notification shown")

Custom Event: On Notification Hidden Handler
└─ Print String ("Notification hidden")
```

**C++:**
```cpp
void UMyGameInstance::Init()
{
    Super::Init();

    UNotificationManager* Manager = GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->OnNotificationShown.AddDynamic(this, &UMyGameInstance::OnNotificationShownHandler);
        Manager->OnNotificationHidden.AddDynamic(this, &UMyGameInstance::OnNotificationHiddenHandler);
    }
}

void UMyGameInstance::OnNotificationShownHandler(const FNotificationData& Data)
{
    NotificationCount++;
    UE_LOG(LogTemp, Log, TEXT("Notification shown: %s"), *Data.Title.ToString());
}

void UMyGameInstance::OnNotificationHiddenHandler(const FNotificationData& Data)
{
    UE_LOG(LogTemp, Log, TEXT("Notification hidden: %s"), *Data.Title.ToString());
}
```

## 優先度管理の例

### 緊急度による表示順序

**C++:**
```cpp
void AMyGameMode::ShowMultipleNotifications()
{
    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (!Manager) return;

    // 低優先度の通知
    Manager->ShowSimpleNotification(
        FText::FromString(TEXT("Tip")),
        FText::FromString(TEXT("Eキーでアイテムを使用できます")),
        ENotificationType::Info,
        ENotificationPriority::Low
    );

    // 通常優先度の通知
    Manager->ShowSimpleNotification(
        FText::FromString(TEXT("アイテム取得")),
        FText::FromString(TEXT("体力ポーションを入手しました")),
        ENotificationType::Success,
        ENotificationPriority::Normal
    );

    // 高優先度の通知（この通知が最初に表示される）
    Manager->ShowSimpleNotification(
        FText::FromString(TEXT("警告")),
        FText::FromString(TEXT("敵が接近しています!")),
        ENotificationType::Warning,
        ENotificationPriority::High
    );

    // 表示順序: High → Normal → Low
}
```

## カスタムデータの活用例

### プレイヤー統計の表示

**C++:**
```cpp
void AMyCharacter::ShowStatsNotification(int32 Level, int32 Experience, int32 SkillPoints)
{
    FNotificationData Data;
    Data.Title = FText::FromString(TEXT("レベルアップ"));
    Data.Message = FText::Format(
        FText::FromString(TEXT("レベル {0} に到達しました!")),
        FText::AsNumber(Level)
    );
    Data.Type = ENotificationType::Achievement;
    Data.Priority = ENotificationPriority::High;
    Data.DisplayDuration = 7.0f;

    // カスタムデータに統計情報を格納
    Data.CustomData.Add(TEXT("Level"), FString::FromInt(Level));
    Data.CustomData.Add(TEXT("Experience"), FString::FromInt(Experience));
    Data.CustomData.Add(TEXT("SkillPoints"), FString::FromInt(SkillPoints));
    Data.CustomData.Add(TEXT("ShowStats"), TEXT("true"));

    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->ShowNotification(Data);
    }
}
```

**Blueprint (WBP_Notification):**
```
Event On Data Updated
├─ Get Notification Data
│   └─ Break NotificationData
│       └─ Custom Data
│           ├─ Find (Key: "ShowStats")
│           │   └─ Branch (== "true")
│           │       └─ True:
│           │           ├─ Find (Key: "Level") → Set Text (LevelText)
│           │           ├─ Find (Key: "Experience") → Set Text (ExpText)
│           │           └─ Find (Key: "SkillPoints") → Set Text (SkillPointsText)
│           │           └─ Set Visibility (StatsPanel, Visible)
│           └─ False:
│               └─ Set Visibility (StatsPanel, Collapsed)
```

## セーブデータとの連携

### 通知履歴の保存

**C++:**
```cpp
// 通知履歴を保存
USTRUCT()
struct FNotificationHistory
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FString> NotificationTitles;

    UPROPERTY()
    TArray<FString> NotificationMessages;

    UPROPERTY()
    int32 TotalCount = 0;
};

void UMySaveGame::SaveNotificationHistory()
{
    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->OnNotificationShown.AddDynamic(this, &UMySaveGame::OnNotificationShown);
    }
}

void UMySaveGame::OnNotificationShown(const FNotificationData& Data)
{
    NotificationHistory.NotificationTitles.Add(Data.Title.ToString());
    NotificationHistory.NotificationMessages.Add(Data.Message.ToString());
    NotificationHistory.TotalCount++;

    // 最新100件のみ保持
    if (NotificationHistory.NotificationTitles.Num() > 100)
    {
        NotificationHistory.NotificationTitles.RemoveAt(0);
        NotificationHistory.NotificationMessages.RemoveAt(0);
    }
}
```

## パフォーマンス最適化

### 連続通知のスロットリング

**C++:**
```cpp
class AMyPlayerController : public APlayerController
{
private:
    float LastNotificationTime = 0.0f;
    const float NotificationCooldown = 0.5f; // 0.5秒のクールダウン

public:
    void ShowThrottledNotification(const FText& Title, const FText& Message)
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();

        if (CurrentTime - LastNotificationTime < NotificationCooldown)
        {
            // クールダウン中は通知をスキップ
            return;
        }

        UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
        if (Manager)
        {
            Manager->ShowSimpleNotification(Title, Message);
            LastNotificationTime = CurrentTime;
        }
    }
};
```

### 同一通知の重複排除

**C++:**
```cpp
class AMyPlayerController : public APlayerController
{
private:
    TSet<FString> ActiveNotifications;

public:
    void ShowUniqueNotification(const FString& NotificationID, const FText& Title, const FText& Message)
    {
        if (ActiveNotifications.Contains(NotificationID))
        {
            // 既に表示中の通知IDなのでスキップ
            return;
        }

        UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
        if (Manager)
        {
            Manager->ShowSimpleNotification(Title, Message);
            ActiveNotifications.Add(NotificationID);

            // 一定時間後にIDを削除
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, NotificationID]()
            {
                ActiveNotifications.Remove(NotificationID);
            }, 5.0f, false);
        }
    }
};
```

## デバッグ用の機能

### 通知のテスト

**Blueprint:**
```
Event On Key Pressed (F1)
└─ Get Game Instance
    └─ Get Subsystem (Notification Manager)
        └─ Show Simple Notification
            ├─ Title: "テスト通知"
            ├─ Message: "これはテスト通知です"
            ├─ Type: Info
            └─ Priority: Normal
```

**C++:**
```cpp
void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &AMyPlayerController::TestNotification);
}

void AMyPlayerController::TestNotification()
{
    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        Manager->ShowSimpleNotification(
            FText::FromString(TEXT("テスト通知")),
            FText::FromString(TEXT("これはテスト通知です")),
            ENotificationType::Info,
            ENotificationPriority::Normal
        );
    }
}
```

### キュー状態の監視

**Blueprint:**
```
Event Tick
└─ Get Game Instance
    └─ Get Subsystem (Notification Manager)
        └─ Get Queued Notification Count
            └─ Set Text (QueueCountText, Format Text("Queue: {0}", Count))
```

**C++:**
```cpp
void AMyHUD::DrawHUD()
{
    Super::DrawHUD();

    UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
    if (Manager)
    {
        int32 QueueCount = Manager->GetQueuedNotificationCount();
        FString DebugText = FString::Printf(TEXT("Notification Queue: %d"), QueueCount);

        DrawText(
            DebugText,
            FLinearColor::White,
            10, 10
        );
    }
}
```
