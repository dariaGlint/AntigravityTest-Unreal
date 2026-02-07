# 使用方法詳細

## 基本的な使い方

### 1. プラグインの有効化

1. エディタを開く
2. Edit > Plugins を選択
3. "Notification System" を検索
4. チェックボックスをオンにして有効化
5. エディタを再起動

### 2. 通知ウィジェットの作成

#### Widget Blueprintの作成

1. Content Browserで右クリック
2. User Interface > Widget Blueprint を選択
3. 名前を`WBP_Notification`に設定
4. Graph Editor で、File > Reparent Blueprint を選択
5. `NotificationWidget`を選択

#### UIデザイン

推奨される構造：
```
Canvas Panel
└─ Border (Background)
    └─ Vertical Box
        ├─ Horizontal Box (Header)
        │   ├─ Image (Icon)
        │   └─ Text Block (Title)
        └─ Text Block (Message)
```

変数の作成：
- `TitleText` (Text Block) - タイトル表示用
- `MessageText` (Text Block) - メッセージ表示用
- `IconImage` (Image) - アイコン表示用
- `BackgroundBorder` (Border) - 背景用

### 3. イベントの実装

#### On Data Updated

通知データが設定されたときに呼ばれます。ここでUI要素を更新します。

```
Event On Data Updated
├─ Get Notification Data
│   ├─ Break NotificationData
│   │   ├─ Title → Set Text (TitleText)
│   │   ├─ Message → Set Text (MessageText)
│   │   └─ Type → Switch
│   │       ├─ Info: Set Color (Blue)
│   │       ├─ Success: Set Color (Green)
│   │       ├─ Warning: Set Color (Yellow)
│   │       ├─ Error: Set Color (Red)
│   │       └─ Achievement: Set Color (Gold)
```

#### On Fade In

フェードイン中に毎フレーム呼ばれます。Alpha値（0→1）を使ってアニメーション制御します。

```
Event On Fade In
├─ Set Render Opacity (Alpha)
└─ Set Render Scale (Make Vector2D: Alpha, Alpha)
```

#### On Fade Out

フェードアウト中に毎フレーム呼ばれます。Alpha値（1→0）を使ってアニメーション制御します。

```
Event On Fade Out
├─ Set Render Opacity (Alpha)
└─ Set Render Scale (Make Vector2D: Alpha, Alpha)
```

#### On Notification Shown

通知が表示されたときに一度だけ呼ばれます。サウンド再生などに使用します。

```
Event On Notification Shown
└─ Play Sound 2D (NotificationSound)
```

#### On Notification Hidden

通知が非表示になったときに一度だけ呼ばれます。

```
Event On Notification Hidden
└─ (クリーンアップ処理)
```

### 4. NotificationManagerの設定

#### Project Settingsでの設定

1. Edit > Project Settings を開く
2. "Notification Manager" で検索
3. Default Notification Widget Class に作成したWidget Blueprintを設定

#### Blueprintでの設定

ゲーム開始時などに設定：

```
Event Begin Play
└─ Get Game Instance
    └─ Get Subsystem (Notification Manager)
        └─ Set Notification Widget Class
            └─ Widget Class: WBP_Notification
```

### 5. 通知の表示

#### シンプルな表示

```
Show Simple Notification
├─ Title: "アイテム取得"
├─ Message: "体力ポーションを入手しました"
├─ Type: Success
└─ Priority: Normal
```

#### 詳細設定

```
Make NotificationData
├─ Title: "警告"
├─ Message: "体力が少なくなっています"
├─ Type: Warning
├─ Priority: High
├─ Display Duration: 5.0
├─ Fade In Duration: 0.5
└─ Fade Out Duration: 0.5

Show Notification (上記のNotificationData)
```

## 応用例

### タイプ別のカスタマイズ

#### サウンド

```
Event On Notification Shown
└─ Get Notification Data
    └─ Break NotificationData
        └─ Type → Switch
            ├─ Info: Play Sound (InfoSound)
            ├─ Success: Play Sound (SuccessSound)
            ├─ Warning: Play Sound (WarningSound)
            ├─ Error: Play Sound (ErrorSound)
            └─ Achievement: Play Sound (AchievementSound)
```

#### アイコン

```
Event On Data Updated
└─ Get Notification Data
    └─ Break NotificationData
        ├─ Icon Texture → Is Valid
        │   └─ True: Set Brush from Texture (IconImage)
        └─ Type → Switch
            ├─ Info: Set Brush from Texture (InfoIcon)
            ├─ Success: Set Brush from Texture (SuccessIcon)
            └─ ...
```

#### アニメーション

Widget Animationを作成して使用：

```
Event On Notification Shown
└─ Play Animation (SlideInAnimation)

Event On Notification Hidden
└─ Play Animation (SlideOutAnimation)
```

### カスタムデータの使用

#### C++側

```cpp
FNotificationData Data;
Data.Title = FText::FromString(TEXT("レベルアップ"));
Data.Message = FText::FromString(TEXT("レベル10に到達しました"));
Data.Type = ENotificationType::Success;
Data.CustomData.Add(TEXT("Level"), TEXT("10"));
Data.CustomData.Add(TEXT("NewSkillPoints"), TEXT("5"));

UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
Manager->ShowNotification(Data);
```

#### Blueprint側

```
Event On Data Updated
└─ Get Notification Data
    └─ Break NotificationData
        └─ Custom Data
            ├─ Find (Key: "Level")
            │   └─ Set Text (LevelText)
            └─ Find (Key: "NewSkillPoints")
                └─ Set Text (SkillPointsText)
```

### イベントのバインド

GameModeやGameInstanceで通知イベントをリッスン：

```
Event Begin Play
└─ Get Game Instance
    └─ Get Subsystem (Notification Manager)
        ├─ Bind Event to On Notification Shown
        │   └─ Custom Event: OnNotificationShownHandler
        └─ Bind Event to On Notification Hidden
            └─ Custom Event: OnNotificationHiddenHandler

Custom Event: OnNotificationShownHandler
└─ Print String ("Notification shown")
└─ (統計情報の更新など)

Custom Event: OnNotificationHiddenHandler
└─ Print String ("Notification hidden")
```

### 複数の通知の管理

```
// 低優先度の通知
Make NotificationData (Priority: Low, Message: "Tip: ...")
Show Notification

// 通常優先度の通知
Make NotificationData (Priority: Normal, Message: "Item acquired")
Show Notification

// 高優先度の通知（この通知が先に表示される）
Make NotificationData (Priority: High, Message: "Warning!")
Show Notification
```

### 通知のクリア

特定のシチュエーションで全通知をクリア：

```
Event On Player Death
└─ Get Game Instance
    └─ Get Subsystem (Notification Manager)
        └─ Clear All Notifications
```

## トラブルシューティング

### 通知が表示されない場合

1. Widget Classが設定されているか確認
   ```
   Get Subsystem (Notification Manager)
   └─ Get Default Notification Widget Class
       └─ Is Valid → Print String
   ```

2. PlayerControllerが存在するか確認
   ```
   Get Player Controller (Index: 0)
   └─ Is Valid → Print String
   ```

3. ログを確認
   - Output Log で "NotificationManager" を検索
   - Warning/Error メッセージを確認

### フェードが機能しない場合

1. `On Fade In`/`On Fade Out`イベントが実装されているか確認
2. Alpha値が正しく適用されているか確認
   ```
   Event On Fade In
   └─ Print String (Alpha) // デバッグ用
   └─ Set Render Opacity (Alpha)
   ```

### 優先度が機能しない場合

1. 優先度を確認
   ```
   Make NotificationData
   └─ Priority: Critical (最高優先度)
   ```

2. キューの状態を確認
   ```
   Get Subsystem (Notification Manager)
   └─ Get Queued Notification Count
       └─ Print String
   ```

## パフォーマンス考慮事項

### Tickの最適化

デフォルトでは、NotificationWidgetはフェード中のみTickします。フェードが完了すると自動的に停止します。

カスタムアニメーションを追加する場合は、パフォーマンスに注意してください：

```cpp
// C++側でTickを制御
void UMyNotificationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // カスタムアニメーション処理
    if (bIsAnimating)
    {
        // ...
    }
}
```

### メモリ管理

通知ウィジェットは使用後に自動的に削除されます。明示的な削除は不要です。

大量の通知を短時間に表示する場合は、キューのサイズを監視：

```
Get Queued Notification Count
└─ Branch (> 10)
    └─ True: Clear All Notifications
```

## ベストプラクティス

1. **一貫性のあるデザイン**
   - すべての通知で同じレイアウトを使用
   - タイプごとに明確な色分け
   - 読みやすいフォントサイズ

2. **適切な表示時間**
   - 短いメッセージ: 3-4秒
   - 長いメッセージ: 5-7秒
   - 重要な通知: 7-10秒

3. **優先度の使い分け**
   - Critical: 致命的なエラー、即座の対応が必要
   - High: 重要な警告、イベント
   - Normal: 通常のゲームイベント
   - Low: Tips、ヒント

4. **スパム対策**
   - 同じ通知を連続して表示しない
   - 必要に応じてクールダウンを実装

5. **アクセシビリティ**
   - 色だけに頼らない（アイコンも使用）
   - 読みやすいコントラスト
   - オプションで通知のオン/オフ切り替え
