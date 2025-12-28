# Screen Transition System - 使用ガイド

このドキュメントでは、Screen Transition Systemの詳細な使用方法を説明します。

## 目次

1. [基本概念](#基本概念)
2. [画面の作成](#画面の作成)
3. [画面遷移の実装](#画面遷移の実装)
4. [トランジションエフェクト](#トランジションエフェクト)
5. [画面スタック管理](#画面スタック管理)
6. [カスタマイズ](#カスタマイズ)
7. [ベストプラクティス](#ベストプラクティス)

---

## 基本概念

### アーキテクチャ

Screen Transition Systemは以下の3つの主要コンポーネントで構成されています：

1. **ScreenTransitionManager** - GameInstanceSubsystem
   - 画面遷移の中央管理
   - 画面スタックの管理
   - トランジションの実行

2. **ScreenBase** - UUserWidget派生クラス
   - すべての画面の基底クラス
   - ライフサイクル管理
   - 入力設定の管理

3. **TransitionEffect** - UObject派生クラス
   - 画面遷移時のビジュアルエフェクト
   - アニメーション処理

### 画面のライフサイクル

```
[作成] → OnEnter → [アクティブ] → OnExit → [破棄]
             ↓                        ↑
             └─ OnPause → OnResume ──┘
```

---

## 画面の作成

### Blueprintでの作成

1. **Widget Blueprintを作成**
   ```
   Content Browser > Right Click > User Interface > Widget Blueprint
   名前: WBP_YourScreenName
   ```

2. **親クラスをScreenBaseに変更**
   ```
   File > Reparent Blueprint > ScreenBase
   ```

3. **UIをデザイン**
   - Canvas Panel を追加
   - ボタン、テキストなどを配置

4. **ライフサイクルイベントをオーバーライド**
   ```
   Event Graph:
   - Event On Enter
   - Event On Exit
   - Event On Pause
   - Event On Resume
   ```

### C++での作成

```cpp
// YourScreen.h
#pragma once

#include "CoreMinimal.h"
#include "ScreenBase.h"
#include "YourScreen.generated.h"

UCLASS()
class YOURPROJECT_API UYourScreen : public UScreenBase
{
    GENERATED_BODY()

protected:
    virtual void OnEnter_Implementation() override;
    virtual void OnExit_Implementation() override;
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    class UButton* StartButton;

    UFUNCTION()
    void OnStartButtonClicked();
};
```

```cpp
// YourScreen.cpp
#include "YourScreen.h"
#include "Components/Button.h"

void UYourScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UYourScreen::OnStartButtonClicked);
    }
}

void UYourScreen::OnEnter_Implementation()
{
    Super::OnEnter_Implementation();

    // 画面が表示されたときの処理
    UE_LOG(LogTemp, Log, TEXT("Screen Entered"));
}

void UYourScreen::OnExit_Implementation()
{
    Super::OnExit_Implementation();

    // 画面が非表示になるときの処理
    UE_LOG(LogTemp, Log, TEXT("Screen Exited"));
}

void UYourScreen::OnStartButtonClicked()
{
    // ボタンクリック処理
}
```

---

## 画面遷移の実装

### Blueprint

#### 基本的な遷移

```
[Get Game Instance]
    └─ [Get Subsystem: ScreenTransitionManager]
        └─ [Transition To Screen]
            ├─ Screen Class: WBP_TargetScreen
            ├─ Use Transition: true
            └─ Transition Effect Class: None (デフォルト使用)
```

#### トランジション付き遷移

```
[Get Game Instance]
    └─ [Get Subsystem: ScreenTransitionManager]
        └─ [Transition To Screen]
            ├─ Screen Class: WBP_GameScreen
            ├─ Use Transition: true
            └─ Transition Effect Class: BP_FadeTransition
```

### C++

#### GameModeから遷移

```cpp
void AYourGameMode::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            // タイトル画面に遷移
            Manager->TransitionToScreen(UWBP_TitleScreen::StaticClass(), true);
        }
    }
}
```

#### Widget内から遷移

```cpp
void UYourScreen::OnStartButtonClicked()
{
    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            // ゲーム画面に遷移
            Manager->TransitionToScreen(UWBP_GameScreen::StaticClass(), true);
        }
    }
}
```

---

## トランジションエフェクト

### デフォルトエフェクトの設定

```cpp
// Game Instance初期化時
void UYourGameInstance::Init()
{
    Super::Init();

    UScreenTransitionManager* Manager = GetSubsystem<UScreenTransitionManager>();
    if (Manager)
    {
        // デフォルトのトランジションエフェクトを設定
        Manager->SetDefaultTransitionEffect(UTransitionEffect::StaticClass());
    }
}
```

### エフェクトタイプの選択

**Blueprint:**

TransitionEffectを継承したBlueprint Classを作成し、以下のプロパティを設定：

- **Transition Type**: Fade / Slide / Custom
- **Duration**: 遷移時間（秒）
- **Slide Direction**: Left / Right / Up / Down (Slideの場合)

**C++:**

```cpp
// カスタムトランジションエフェクト
UCLASS()
class UCustomFadeTransition : public UTransitionEffect
{
    GENERATED_BODY()

public:
    UCustomFadeTransition()
    {
        TransitionType = ETransitionType::Fade;
        Duration = 0.5f;
    }
};
```

### カスタムトランジションの作成

Blueprint:

```
1. TransitionEffect を親クラスとして Blueprint Class を作成
2. Event Graph で "Event Tick Transition" をオーバーライド
3. Alpha 値（0.0～1.0）を使用してカスタムアニメーション実装
```

C++:

```cpp
UCLASS()
class UCustomTransitionEffect : public UTransitionEffect
{
    GENERATED_BODY()

protected:
    virtual void TickTransition_Implementation(float Alpha) override
    {
        Super::TickTransition_Implementation(Alpha);

        // カスタムアニメーション処理
        if (ToScreenRef)
        {
            // 例: スケールアニメーション
            float Scale = FMath::Lerp(0.5f, 1.0f, Alpha);
            ToScreenRef->SetRenderScale(FVector2D(Scale, Scale));
        }
    }
};
```

---

## 画面スタック管理

### 基本的なスタック操作

画面スタックを使用すると、前の画面に戻る機能を簡単に実装できます。

```cpp
// 画面をスタックにプッシュ（現在の画面を保持）
Manager->PushScreen(UWBP_SettingsScreen::StaticClass(), false, true);

// 前の画面に戻る
Manager->PopScreen(true);

// スタックの深さを取得
int32 Depth = Manager->GetScreenStackDepth();

// スタックをクリア
Manager->ClearScreenStack();
```

### モーダル画面

モーダル画面は、背景の画面を維持したまま、前面に表示される画面です。

```cpp
// モーダルとして画面をプッシュ
Manager->PushScreen(UWBP_DialogScreen::StaticClass(), true, true);
```

### 使用例: 設定画面

```cpp
// メインゲーム画面から設定画面を開く
void UGameScreen::OpenSettings()
{
    UScreenTransitionManager* Manager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
    if (Manager)
    {
        // 設定画面をモーダルでプッシュ
        Manager->PushScreen(UWBP_SettingsScreen::StaticClass(), true, true);
    }
}

// 設定画面から戻る
void USettingsScreen::CloseSettings()
{
    UScreenTransitionManager* Manager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
    if (Manager)
    {
        // 前の画面（ゲーム画面）に戻る
        Manager->PopScreen(true);
    }
}
```

---

## カスタマイズ

### 入力設定のカスタマイズ

ScreenBaseでは、各画面ごとにマウスカーソルとクリックイベントを制御できます。

```cpp
UCLASS()
class UYourScreen : public UScreenBase
{
    GENERATED_BODY()

public:
    UYourScreen(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
    {
        // マウスカーソルを表示
        bShowMouseCursor = true;

        // クリックイベントを有効化
        bEnableClickEvents = true;

        // Z順序を設定
        ZOrder = 10;
    }
};
```

### イベントのリスニング

```cpp
void UYourGameMode::BeginPlay()
{
    Super::BeginPlay();

    UScreenTransitionManager* Manager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
    if (Manager)
    {
        // 画面変更イベントをリスン
        Manager->OnScreenChanged.AddDynamic(this, &UYourGameMode::OnScreenChanged);

        // トランジション開始イベントをリスン
        Manager->OnTransitionStarted.AddDynamic(this, &UYourGameMode::OnTransitionStarted);

        // トランジション完了イベントをリスン
        Manager->OnTransitionCompleted.AddDynamic(this, &UYourGameMode::OnTransitionCompleted);
    }
}

void UYourGameMode::OnScreenChanged(UScreenBase* OldScreen, UScreenBase* NewScreen)
{
    UE_LOG(LogTemp, Log, TEXT("Screen changed"));
}

void UYourGameMode::OnTransitionStarted(UScreenBase* TargetScreen)
{
    UE_LOG(LogTemp, Log, TEXT("Transition started"));
}

void UYourGameMode::OnTransitionCompleted(UScreenBase* CurrentScreen)
{
    UE_LOG(LogTemp, Log, TEXT("Transition completed"));
}
```

---

## ベストプラクティス

### 1. 画面の責務を明確にする

各画面は単一の責務を持つべきです。

```
良い例:
- WBP_TitleScreen (タイトル画面)
- WBP_MainGame (メインゲーム)
- WBP_Settings (設定)
- WBP_Pause (ポーズメニュー)

悪い例:
- WBP_Everything (すべてを1つの画面に詰め込む)
```

### 2. TransitionToScreen vs PushScreen

- **TransitionToScreen**: 完全に新しい画面に切り替える（戻る必要がない）
  - タイトル → メインゲーム
  - ゲーム → リザルト

- **PushScreen**: 前の画面に戻る可能性がある
  - ゲーム → 設定
  - ゲーム → ポーズ

### 3. メモリ管理

不要になった画面は自動的に破棄されますが、大きなアセットを保持する場合は注意が必要です。

```cpp
void UYourScreen::OnExit_Implementation()
{
    Super::OnExit_Implementation();

    // 大きなアセットを明示的に解放
    LargeTexture = nullptr;
    VideoPlayer->Stop();
}
```

### 4. トランジション中の入力を無効化

トランジション中はユーザー入力を無効化することを推奨します。

```cpp
if (!Manager->IsTransitioning())
{
    // 入力処理
}
```

### 5. デフォルトトランジションの設定

Game Instanceの初期化時にデフォルトのトランジションエフェクトを設定すると便利です。

```cpp
void UYourGameInstance::Init()
{
    Super::Init();

    UScreenTransitionManager* Manager = GetSubsystem<UScreenTransitionManager>();
    if (Manager)
    {
        Manager->SetDefaultTransitionEffect(UYourDefaultTransition::StaticClass());
    }
}
```

### 6. 画面固有のロジックは画面内に

画面固有のロジックはScreenBase派生クラス内に実装し、Managerには依存しないようにします。

```cpp
// 良い例
void UTitleScreen::OnPlayButtonClicked()
{
    // タイトル画面固有のロジック
    SavePlayerPreferences();

    // 遷移
    TransitionToGame();
}

// 悪い例
void UGameMode::HandleTitleScreenLogic()
{
    // GameModeがタイトル画面のロジックを知っている（密結合）
}
```

---

## トラブルシューティング

### 画面が表示されない

1. ScreenBaseを親クラスとして設定しているか確認
2. TransitionToScreenまたはPushScreenが正しく呼ばれているか確認
3. ScreenTransitionManagerが正しく取得できているか確認

```cpp
UScreenTransitionManager* Manager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
if (!Manager)
{
    UE_LOG(LogTemp, Error, TEXT("ScreenTransitionManager not found!"));
}
```

### トランジションが動作しない

1. TransitionEffectClassが正しく設定されているか確認
2. Durationが0より大きいか確認
3. デフォルトエフェクトが設定されているか確認

### スタックがクリアされてしまう

TransitionToScreenを使用するとスタックがクリアされます。スタックを維持したい場合はPushScreenを使用してください。

---

次のドキュメント: [APIリファレンス](API.md)
