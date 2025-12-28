# Screen Transition System - サンプル集

このドキュメントでは、Screen Transition Systemの実践的なサンプルコードを提供します。

## 目次

1. [基本的な画面遷移](#基本的な画面遷移)
2. [ゲームフローの実装](#ゲームフローの実装)
3. [設定画面の実装](#設定画面の実装)
4. [ダイアログシステム](#ダイアログシステム)
5. [ローディング画面](#ローディング画面)
6. [カスタムトランジション](#カスタムトランジション)
7. [複雑な画面スタック管理](#複雑な画面スタック管理)

---

## 基本的な画面遷移

### タイトル画面からゲーム画面への遷移

**C++実装:**

```cpp
// TitleScreen.h
#pragma once

#include "CoreMinimal.h"
#include "ScreenBase.h"
#include "TitleScreen.generated.h"

UCLASS()
class YOURPROJECT_API UTitleScreen : public UScreenBase
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* QuitButton;

    UFUNCTION()
    void OnPlayButtonClicked();

    UFUNCTION()
    void OnQuitButtonClicked();
};
```

```cpp
// TitleScreen.cpp
#include "TitleScreen.h"
#include "Components/Button.h"
#include "ScreenTransitionManager.h"
#include "Kismet/KismetSystemLibrary.h"

void UTitleScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UTitleScreen::OnPlayButtonClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UTitleScreen::OnQuitButtonClicked);
    }
}

void UTitleScreen::OnPlayButtonClicked()
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

void UTitleScreen::OnQuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
```

**Blueprint実装:**

Widget Blueprintのイベントグラフ:
```
Event On Play Button Clicked
    └─ Get Game Instance
        └─ Get Subsystem (ScreenTransitionManager)
            └─ Transition To Screen
                ├─ Screen Class: WBP_GameScreen
                ├─ Use Transition: true
                └─ Transition Effect Class: None
```

---

## ゲームフローの実装

### タイトル → ゲーム → リザルト → タイトルの完全なフロー

```cpp
// GameFlowManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFlowManager.generated.h"

UCLASS()
class YOURPROJECT_API UGameFlowManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void ShowTitleScreen();

    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void ShowResultScreen(int32 Score, bool bVictory);

    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void RestartGame();

    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void ReturnToTitle();

private:
    UPROPERTY()
    class UScreenTransitionManager* TransitionManager;
};
```

```cpp
// GameFlowManager.cpp
#include "GameFlowManager.h"
#include "ScreenTransitionManager.h"

void UGameFlowManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    TransitionManager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();

    // ゲーム開始時にタイトル画面を表示
    ShowTitleScreen();
}

void UGameFlowManager::ShowTitleScreen()
{
    if (TransitionManager)
    {
        TransitionManager->TransitionToScreen(UWBP_TitleScreen::StaticClass(), true);
    }
}

void UGameFlowManager::StartGame()
{
    if (TransitionManager)
    {
        TransitionManager->TransitionToScreen(UWBP_GameScreen::StaticClass(), true);
    }
}

void UGameFlowManager::ShowResultScreen(int32 Score, bool bVictory)
{
    if (TransitionManager)
    {
        TransitionManager->TransitionToScreen(UWBP_ResultScreen::StaticClass(), true);

        // リザルト画面にスコアを渡す
        UScreenBase* ResultScreen = TransitionManager->GetCurrentScreen();
        if (ResultScreen)
        {
            UWBP_ResultScreen* TypedResultScreen = Cast<UWBP_ResultScreen>(ResultScreen);
            if (TypedResultScreen)
            {
                TypedResultScreen->SetResult(Score, bVictory);
            }
        }
    }
}

void UGameFlowManager::RestartGame()
{
    StartGame();
}

void UGameFlowManager::ReturnToTitle()
{
    ShowTitleScreen();
}
```

---

## 設定画面の実装

### ゲーム中から設定画面を開く（スタック使用）

```cpp
// GameScreen.h
#pragma once

#include "CoreMinimal.h"
#include "ScreenBase.h"
#include "GameScreen.generated.h"

UCLASS()
class YOURPROJECT_API UGameScreen : public UScreenBase
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* SettingsButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* PauseButton;

    UFUNCTION()
    void OnSettingsButtonClicked();

    UFUNCTION()
    void OnPauseButtonClicked();
};
```

```cpp
// GameScreen.cpp
#include "GameScreen.h"
#include "Components/Button.h"
#include "ScreenTransitionManager.h"

void UGameScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UGameScreen::OnSettingsButtonClicked);
    }

    if (PauseButton)
    {
        PauseButton->OnClicked.AddDynamic(this, &UGameScreen::OnPauseButtonClicked);
    }
}

void UGameScreen::OnSettingsButtonClicked()
{
    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            // 設定画面をスタックにプッシュ（戻るボタンでゲーム画面に戻れる）
            Manager->PushScreen(UWBP_SettingsScreen::StaticClass(), false, true);
        }
    }
}

void UGameScreen::OnPauseButtonClicked()
{
    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            // ポーズ画面をモーダルでプッシュ
            Manager->PushScreen(UWBP_PauseScreen::StaticClass(), true, true);
        }
    }
}
```

```cpp
// SettingsScreen.h
#pragma once

#include "CoreMinimal.h"
#include "ScreenBase.h"
#include "SettingsScreen.generated.h"

UCLASS()
class YOURPROJECT_API USettingsScreen : public UScreenBase
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* BackButton;

    UPROPERTY(meta = (BindWidget))
    class USlider* VolumeSlider;

    UFUNCTION()
    void OnBackButtonClicked();

    UFUNCTION()
    void OnVolumeChanged(float Value);
};
```

```cpp
// SettingsScreen.cpp
#include "SettingsScreen.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "ScreenTransitionManager.h"

void USettingsScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &USettingsScreen::OnBackButtonClicked);
    }

    if (VolumeSlider)
    {
        VolumeSlider->OnValueChanged.AddDynamic(this, &USettingsScreen::OnVolumeChanged);
    }
}

void USettingsScreen::OnBackButtonClicked()
{
    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            // 前の画面（ゲーム画面）に戻る
            Manager->PopScreen(true);
        }
    }
}

void USettingsScreen::OnVolumeChanged(float Value)
{
    // ボリューム設定を適用
    UE_LOG(LogTemp, Log, TEXT("Volume changed to: %f"), Value);
}
```

---

## ダイアログシステム

### 確認ダイアログの実装

```cpp
// DialogScreen.h
#pragma once

#include "CoreMinimal.h"
#include "ScreenBase.h"
#include "DialogScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogResult, bool, bConfirmed);

UCLASS()
class YOURPROJECT_API UDialogScreen : public UScreenBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Dialog")
    void ShowDialog(const FText& Title, const FText& Message);

    UPROPERTY(BlueprintAssignable, Category = "Dialog")
    FOnDialogResult OnDialogResult;

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TitleText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MessageText;

    UPROPERTY(meta = (BindWidget))
    class UButton* ConfirmButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* CancelButton;

    UFUNCTION()
    void OnConfirmButtonClicked();

    UFUNCTION()
    void OnCancelButtonClicked();

private:
    void CloseDialog(bool bConfirmed);
};
```

```cpp
// DialogScreen.cpp
#include "DialogScreen.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "ScreenTransitionManager.h"

void UDialogScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (ConfirmButton)
    {
        ConfirmButton->OnClicked.AddDynamic(this, &UDialogScreen::OnConfirmButtonClicked);
    }

    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UDialogScreen::OnCancelButtonClicked);
    }
}

void UDialogScreen::ShowDialog(const FText& Title, const FText& Message)
{
    if (TitleText)
    {
        TitleText->SetText(Title);
    }

    if (MessageText)
    {
        MessageText->SetText(Message);
    }
}

void UDialogScreen::OnConfirmButtonClicked()
{
    CloseDialog(true);
}

void UDialogScreen::OnCancelButtonClicked()
{
    CloseDialog(false);
}

void UDialogScreen::CloseDialog(bool bConfirmed)
{
    OnDialogResult.Broadcast(bConfirmed);

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            Manager->PopScreen(true);
        }
    }
}
```

**使用例:**

```cpp
void UGameScreen::OnQuitButtonClicked()
{
    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();
        if (Manager)
        {
            // ダイアログを表示
            Manager->PushScreen(UWBP_DialogScreen::StaticClass(), true, true);

            UDialogScreen* Dialog = Cast<UDialogScreen>(Manager->GetCurrentScreen());
            if (Dialog)
            {
                Dialog->ShowDialog(
                    FText::FromString(TEXT("確認")),
                    FText::FromString(TEXT("ゲームを終了しますか？"))
                );

                // ダイアログの結果をリスン
                Dialog->OnDialogResult.AddDynamic(this, &UGameScreen::OnQuitDialogResult);
            }
        }
    }
}

void UGameScreen::OnQuitDialogResult(bool bConfirmed)
{
    if (bConfirmed)
    {
        // ゲームを終了
        UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
    }
}
```

---

## ローディング画面

### 非同期ロード中にローディング画面を表示

```cpp
// LoadingScreen.h
#pragma once

#include "CoreMinimal.h"
#include "ScreenBase.h"
#include "LoadingScreen.generated.h"

UCLASS()
class YOURPROJECT_API ULoadingScreen : public UScreenBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void SetLoadingProgress(float Progress);

    UFUNCTION(BlueprintCallable, Category = "Loading")
    void SetLoadingMessage(const FText& Message);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MessageText;
};
```

```cpp
// LoadingScreen.cpp
#include "LoadingScreen.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ULoadingScreen::SetLoadingProgress(float Progress)
{
    if (ProgressBar)
    {
        ProgressBar->SetPercent(Progress);
    }
}

void ULoadingScreen::SetLoadingMessage(const FText& Message)
{
    if (MessageText)
    {
        MessageText->SetText(Message);
    }
}
```

```cpp
// GameFlowManager.cpp (非同期ロードの例)
void UGameFlowManager::LoadGameLevel()
{
    if (!TransitionManager)
    {
        return;
    }

    // ローディング画面を表示
    TransitionManager->TransitionToScreen(UWBP_LoadingScreen::StaticClass(), true);

    ULoadingScreen* LoadingScreen = Cast<ULoadingScreen>(TransitionManager->GetCurrentScreen());
    if (LoadingScreen)
    {
        LoadingScreen->SetLoadingMessage(FText::FromString(TEXT("レベルをロード中...")));
        LoadingScreen->SetLoadingProgress(0.0f);
    }

    // 非同期でレベルをロード
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
    LatentInfo.UUID = FMath::Rand();
    LatentInfo.Linkage = 0;

    UGameplayStatics::LoadStreamLevel(
        this,
        FName("GameLevel"),
        true,
        true,
        LatentInfo
    );
}

void UGameFlowManager::OnLevelLoaded()
{
    // レベルロード完了後、ゲーム画面に遷移
    if (TransitionManager)
    {
        TransitionManager->TransitionToScreen(UWBP_GameScreen::StaticClass(), true);
    }
}
```

---

## カスタムトランジション

### カスタムズームトランジションの実装

```cpp
// ZoomTransition.h
#pragma once

#include "CoreMinimal.h"
#include "TransitionEffect.h"
#include "ZoomTransition.generated.h"

UCLASS()
class YOURPROJECT_API UZoomTransition : public UTransitionEffect
{
    GENERATED_BODY()

public:
    UZoomTransition();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom Transition")
    float StartScale;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom Transition")
    float EndScale;

protected:
    virtual void TickTransition_Implementation(float Alpha) override;
};
```

```cpp
// ZoomTransition.cpp
#include "ZoomTransition.h"
#include "ScreenBase.h"

UZoomTransition::UZoomTransition()
{
    TransitionType = ETransitionType::Custom;
    Duration = 0.6f;
    StartScale = 0.5f;
    EndScale = 1.0f;
}

void UZoomTransition::TickTransition_Implementation(float Alpha)
{
    Super::TickTransition_Implementation(Alpha);

    if (FromScreenRef)
    {
        // 旧画面をフェードアウト
        FromScreenRef->SetRenderOpacity(1.0f - Alpha);
    }

    if (ToScreenRef)
    {
        // 新画面をズームイン + フェードイン
        float Scale = FMath::Lerp(StartScale, EndScale, Alpha);
        ToScreenRef->SetRenderScale(FVector2D(Scale, Scale));
        ToScreenRef->SetRenderOpacity(Alpha);
    }
}
```

**使用例:**

```cpp
Manager->TransitionToScreen(
    UWBP_GameScreen::StaticClass(),
    true,
    UZoomTransition::StaticClass()
);
```

---

## 複雑な画面スタック管理

### 深い階層のナビゲーション

```cpp
// NavigationManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NavigationManager.generated.h"

UCLASS()
class YOURPROJECT_API UNavigationManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void NavigateToMainMenu();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void NavigateToCharacterSelection();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void NavigateToCharacterCustomization();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void NavigateBack();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void NavigateToRoot();

    UFUNCTION(BlueprintPure, Category = "Navigation")
    bool CanNavigateBack() const;

private:
    UPROPERTY()
    class UScreenTransitionManager* TransitionManager;
};
```

```cpp
// NavigationManager.cpp
#include "NavigationManager.h"
#include "ScreenTransitionManager.h"

void UNavigationManager::NavigateToMainMenu()
{
    if (!TransitionManager)
    {
        TransitionManager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
    }

    if (TransitionManager)
    {
        // ルート画面に遷移
        TransitionManager->TransitionToScreen(UWBP_MainMenuScreen::StaticClass(), true);
    }
}

void UNavigationManager::NavigateToCharacterSelection()
{
    if (TransitionManager)
    {
        // キャラクター選択画面をプッシュ
        TransitionManager->PushScreen(UWBP_CharacterSelectionScreen::StaticClass(), false, true);
    }
}

void UNavigationManager::NavigateToCharacterCustomization()
{
    if (TransitionManager)
    {
        // カスタマイズ画面をプッシュ
        TransitionManager->PushScreen(UWBP_CharacterCustomizationScreen::StaticClass(), false, true);
    }
}

void UNavigationManager::NavigateBack()
{
    if (TransitionManager)
    {
        TransitionManager->PopScreen(true);
    }
}

void UNavigationManager::NavigateToRoot()
{
    if (TransitionManager)
    {
        // すべてのスタックをクリアしてメインメニューへ
        TransitionManager->ClearScreenStack();
        NavigateToMainMenu();
    }
}

bool UNavigationManager::CanNavigateBack() const
{
    if (TransitionManager)
    {
        return TransitionManager->GetScreenStackDepth() > 0;
    }
    return false;
}
```

**使用フロー:**

```
Main Menu (Root)
  └─ Character Selection (Push)
      └─ Character Customization (Push)
          └─ Back → Character Selection
              └─ Back → Main Menu
```

---

## まとめ

これらのサンプルを参考に、プロジェクトに合わせた画面遷移システムを構築してください。

主なポイント:
- **TransitionToScreen**: ルート画面の切り替え
- **PushScreen**: 戻る機能が必要な画面
- **PopScreen**: 前の画面に戻る
- **モーダル**: 背景を保持したまま表示
- **カスタムトランジション**: 独自のエフェクトを実装

詳細は[使用ガイド](Usage.md)と[APIリファレンス](API.md)を参照してください。
