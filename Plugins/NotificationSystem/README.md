# Notification System for Unreal Engine 5

UE5用のゲーム内通知システムプラグインです。通知のキューイング、優先度管理、フェードイン/アウト制御をサポートします。

## 特徴

- **通知キューイング** - 複数の通知を優先度順に自動管理
- **優先度管理** - 4段階の優先度（低/通常/高/緊急）
- **フェード制御** - カスタマイズ可能なフェードイン/アウトタイミング
- **通知タイプ** - 情報/成功/警告/エラー/実績/カスタムの6種類
- **Blueprint完全対応** - C++とBlueprintの両方で使用可能
- **GameInstanceSubsystem** - ゲーム全体で統一的に管理
- **カスタマイズ可能** - 独自の通知UIを作成可能
- **拡張性** - カスタムデータで機能を拡張可能

## インストール

1. このプラグインフォルダをUE5プロジェクトの`Plugins`ディレクトリにコピー
2. UE5エディタを起動
3. Edit > Plugins から "Notification System" を有効化
4. エディタを再起動

## クイックスタート

### 1. 通知ウィジェットの作成

Widget Blueprintを作成し、親クラスを`NotificationWidget`に設定します。

**例: 通知ウィジェットの作成**
```
Content/UI/WBP_Notification
親クラス: NotificationWidget
```

ウィジェットデザイン例：
- テキストブロック（タイトル用）
- テキストブロック（メッセージ用）
- イメージ（アイコン用）
- ボーダー/キャンバスパネル（背景用）

### 2. NotificationManagerの設定

**Project Settings または DefaultEngine.ini:**
```ini
[/Script/NotificationSystem.NotificationManager]
DefaultNotificationWidgetClass=/Game/UI/WBP_Notification.WBP_Notification_C
```

**Blueprint での設定:**
```
Get Game Instance
  └─ Get Subsystem (Notification Manager)
      └─ Set Notification Widget Class
          └─ Widget Class: WBP_Notification
```

### 3. 通知の表示

**簡易表示（Blueprint）:**
```
Get Game Instance
  └─ Get Subsystem (Notification Manager)
      └─ Show Simple Notification
          ├─ Title: "アイテム獲得"
          ├─ Message: "体力ポーションを入手しました"
          ├─ Type: Success
          └─ Priority: Normal
```

**詳細設定（Blueprint）:**
```
Make NotificationData
  ├─ Title: "警告"
  ├─ Message: "体力が少なくなっています"
  ├─ Type: Warning
  ├─ Priority: High
  ├─ Display Duration: 5.0
  ├─ Fade In Duration: 0.5
  └─ Fade Out Duration: 0.5
  └─ Show Notification
```

**C++:**
```cpp
// シンプルな通知
UNotificationManager* Manager = GetGameInstance()->GetSubsystem<UNotificationManager>();
Manager->ShowSimpleNotification(
    FText::FromString(TEXT("タイトル")),
    FText::FromString(TEXT("メッセージ")),
    ENotificationType::Info,
    ENotificationPriority::Normal
);

// 詳細設定
FNotificationData Data;
Data.Title = FText::FromString(TEXT("実績解除"));
Data.Message = FText::FromString(TEXT("初めてのボス撃破"));
Data.Type = ENotificationType::Achievement;
Data.Priority = ENotificationPriority::High;
Data.DisplayDuration = 5.0f;
Data.FadeInDuration = 0.5f;
Data.FadeOutDuration = 0.5f;
Manager->ShowNotification(Data);
```

## 通知ウィジェットのカスタマイズ

`NotificationWidget`を継承したBlueprintで、以下のイベントをオーバーライドできます：

### イベント

- **On Data Updated** - 通知データが設定されたとき
  - タイトル、メッセージ、アイコンなどのUI要素を更新

- **On Notification Shown** - 通知が表示されたとき
  - サウンドの再生などの処理

- **On Notification Hidden** - 通知が非表示になったとき
  - クリーンアップ処理

- **On Fade In** - フェードイン中（毎フレーム）
  - Alpha値（0.0-1.0）を使ってアニメーション制御

- **On Fade Out** - フェードアウト中（毎フレーム）
  - Alpha値（1.0-0.0）を使ってアニメーション制御

### Blueprintでのフェード実装例

```
Event On Fade In
  └─ Set Render Opacity (Alpha)
  └─ Set Render Scale (Make Vector2D: Alpha, Alpha)

Event On Fade Out
  └─ Set Render Opacity (Alpha)
```

### タイプ別のスタイル変更例

```
Event On Data Updated
  └─ Switch on NotificationType
      ├─ Info: Set Color (Blue)
      ├─ Success: Set Color (Green)
      ├─ Warning: Set Color (Yellow)
      ├─ Error: Set Color (Red)
      └─ Achievement: Set Color (Gold) + Play Sound
```

## 通知タイプ

| タイプ | 用途 | 推奨色 |
|--------|------|--------|
| Info | 一般的な情報 | 青 |
| Success | 成功メッセージ | 緑 |
| Warning | 警告メッセージ | 黄 |
| Error | エラーメッセージ | 赤 |
| Achievement | 実績解除 | 金 |
| Custom | カスタム用途 | 任意 |

## 優先度

| 優先度 | 説明 | 使用例 |
|--------|------|--------|
| Low | 低優先度 | Tips、ヒント |
| Normal | 通常優先度 | アイテム取得、クエスト更新 |
| High | 高優先度 | 警告、重要なイベント |
| Critical | 緊急 | エラー、致命的な状態 |

優先度が高い通知は、キュー内で優先的に表示されます。同じ優先度の場合は、古い順に表示されます。

## 主要API

### NotificationManager

```cpp
// 通知を表示
void ShowNotification(const FNotificationData& NotificationData);

// シンプルな通知表示
void ShowSimpleNotification(const FText& Title, const FText& Message, ENotificationType Type, ENotificationPriority Priority);

// すべての通知をクリア
void ClearAllNotifications();

// 現在の通知を取得
UNotificationWidget* GetCurrentNotification() const;

// キュー内の通知数を取得
int32 GetQueuedNotificationCount() const;

// 通知ウィジェットクラスを設定
void SetNotificationWidgetClass(TSubclassOf<UNotificationWidget> WidgetClass);
```

### NotificationData

```cpp
FText Title;                        // タイトル
FText Message;                      // メッセージ
ENotificationType Type;             // タイプ
ENotificationPriority Priority;     // 優先度
float DisplayDuration;              // 表示時間（秒）
float FadeInDuration;               // フェードイン時間（秒）
float FadeOutDuration;              // フェードアウト時間（秒）
UTexture2D* IconTexture;            // アイコン（オプション）
TMap<FString, FString> CustomData;  // カスタムデータ
```

### NotificationWidget

```cpp
// イベント
void OnNotificationShown();         // 表示時
void OnNotificationHidden();        // 非表示時
void OnFadeIn(float Alpha);         // フェードイン中
void OnFadeOut(float Alpha);        // フェードアウト中
void OnDataUpdated(const FNotificationData& Data);  // データ更新時

// メソッド
void StartFadeIn();                 // フェードイン開始
void StartFadeOut();                // フェードアウト開始
const FNotificationData& GetNotificationData() const;  // データ取得
```

## カスタムデータの使用

通知データには`CustomData`フィールドがあり、追加情報を格納できます：

```cpp
FNotificationData Data;
Data.Title = FText::FromString(TEXT("レベルアップ"));
Data.Message = FText::FromString(TEXT("レベル10に到達しました"));
Data.Type = ENotificationType::Success;
Data.CustomData.Add(TEXT("Level"), TEXT("10"));
Data.CustomData.Add(TEXT("Experience"), TEXT("1000"));

Manager->ShowNotification(Data);
```

Blueprint側で`CustomData`を読み取って、追加のUI要素を表示できます。

## イベント通知

NotificationManagerは以下のイベントを提供します：

```cpp
// 通知が表示されたとき
OnNotificationShown(const FNotificationData& NotificationData);

// 通知が非表示になったとき
OnNotificationHidden(const FNotificationData& NotificationData);
```

これらのイベントをバインドして、通知の表示/非表示時に追加の処理を実行できます。

## ベストプラクティス

1. **通知ウィジェットの設計**
   - シンプルで読みやすいデザインを心がける
   - 画面の邪魔にならない位置に配置（上部中央など）
   - タイプごとに色を変えて視認性を向上

2. **表示時間の設定**
   - 重要な通知は長めに（5-7秒）
   - 一般的な通知は短めに（3-4秒）
   - フェード時間は0.3-0.5秒が推奨

3. **優先度の使い分け**
   - Critical: プレイヤーが即座に対応すべき状況
   - High: 重要だが緊急ではない情報
   - Normal: 通常のゲームイベント
   - Low: Tips、ヒントなど

4. **スパム対策**
   - 連続して同じ通知を出さないようにする
   - 必要に応じて`ClearAllNotifications()`でリセット

## トラブルシューティング

### 通知が表示されない

1. `DefaultNotificationWidgetClass`が設定されているか確認
2. Widget Blueprintの親クラスが`NotificationWidget`になっているか確認
3. ログを確認（`LogTemp`カテゴリ）

### フェードが動作しない

1. Widget Blueprintで`On Fade In`/`On Fade Out`イベントを実装しているか確認
2. `Set Render Opacity`などでAlpha値を適用しているか確認

### 優先度が機能しない

- 優先度が正しく設定されているか確認（高い値ほど優先）
- キューに複数の通知がある状態でテスト

## 拡張例

### サウンド付き通知

```
Event On Notification Shown
  └─ Switch on NotificationType
      ├─ Success: Play Sound (SuccessSound)
      ├─ Error: Play Sound (ErrorSound)
      └─ Achievement: Play Sound (AchievementSound)
```

### アニメーション付き通知

```
Event On Notification Shown
  └─ Play Animation (SlideIn)

Event On Notification Hidden
  └─ Play Animation (SlideOut)
```

## ディレクトリ構造

```
Plugins/NotificationSystem/
├── README.md                           # このファイル
├── NotificationSystem.uplugin          # プラグイン定義
├── Source/NotificationSystem/
│   ├── NotificationSystem.Build.cs     # ビルド設定
│   ├── Public/
│   │   ├── NotificationSystem.h        # モジュールヘッダー
│   │   ├── NotificationManager.h       # メインマネージャー
│   │   ├── NotificationWidget.h        # ウィジェット基底クラス
│   │   └── NotificationData.h          # データ構造体
│   └── Private/
│       ├── NotificationSystem.cpp
│       ├── NotificationManager.cpp
│       └── NotificationWidget.cpp
├── Content/
│   ├── UI/                             # UIアセット用
│   └── Icons/                          # アイコン用
└── Documentation/
    ├── Usage.md                        # 詳細な使用方法
    ├── API.md                          # APIリファレンス
    └── Examples.md                     # サンプルコード
```

## 必要要件

- Unreal Engine 5.0 以上
- C++プロジェクト（またはソースコードをコンパイル可能な環境）

## ライセンス

このプラグインは自由に使用、改変、配布できます。

## サポート

問題が発生した場合は、Issueを作成してください。

## 関連ドキュメント

- [詳細な使用方法](Documentation/Usage.md)
- [APIリファレンス](Documentation/API.md)
- [サンプル集](Documentation/Examples.md)
