# Screen Transition System - APIリファレンス

このドキュメントでは、Screen Transition Systemの完全なAPIリファレンスを提供します。

## 目次

1. [UScreenTransitionManager](#uscreentransitionmanager)
2. [UScreenBase](#uscreenbase)
3. [UTransitionEffect](#utransitioneffect)
4. [Enums](#enums)
5. [Structs](#structs)
6. [Delegates](#delegates)

---

## UScreenTransitionManager

**継承**: `UGameInstanceSubsystem`

画面遷移を管理する中央システム。GameInstanceSubsystemとして実装されており、ゲーム全体で単一のインスタンスが存在します。

### 取得方法

```cpp
// C++
UGameInstance* GI = GetGameInstance();
UScreenTransitionManager* Manager = GI->GetSubsystem<UScreenTransitionManager>();

// Blueprint
Get Game Instance -> Get Subsystem (ScreenTransitionManager)
```

### メソッド

#### TransitionToScreen

画面を完全に切り替えます。現在のスタックをクリアし、新しい画面に遷移します。

```cpp
UFUNCTION(BlueprintCallable, Category = "Screen Transition")
void TransitionToScreen(
    TSubclassOf<UScreenBase> ScreenClass,
    bool bUseTransition = true,
    TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr
);
```

**パラメータ:**
- `ScreenClass`: 遷移先の画面クラス
- `bUseTransition`: トランジションエフェクトを使用するか
- `TransitionEffectClass`: 使用するトランジションエフェクトクラス（nullptrの場合はデフォルトを使用）

**使用例:**
```cpp
Manager->TransitionToScreen(UWBP_TitleScreen::StaticClass(), true);
```

---

#### PushScreen

画面をスタックにプッシュします。現在の画面は保持され、後で戻ることができます。

```cpp
UFUNCTION(BlueprintCallable, Category = "Screen Transition")
void PushScreen(
    TSubclassOf<UScreenBase> ScreenClass,
    bool bAsModal = false,
    bool bUseTransition = true,
    TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr
);
```

**パラメータ:**
- `ScreenClass`: プッシュする画面クラス
- `bAsModal`: モーダルウィンドウとして表示するか
- `bUseTransition`: トランジションエフェクトを使用するか
- `TransitionEffectClass`: 使用するトランジションエフェクトクラス

**使用例:**
```cpp
// 通常のプッシュ
Manager->PushScreen(UWBP_SettingsScreen::StaticClass());

// モーダルとしてプッシュ
Manager->PushScreen(UWBP_DialogScreen::StaticClass(), true);
```

---

#### PopScreen

スタックから画面をポップし、前の画面に戻ります。

```cpp
UFUNCTION(BlueprintCallable, Category = "Screen Transition")
bool PopScreen(
    bool bUseTransition = true,
    TSubclassOf<UTransitionEffect> TransitionEffectClass = nullptr
);
```

**戻り値:**
- `true`: ポップ成功
- `false`: スタックが空、または遷移中のため失敗

**パラメータ:**
- `bUseTransition`: トランジションエフェクトを使用するか
- `TransitionEffectClass`: 使用するトランジションエフェクトクラス

**使用例:**
```cpp
if (Manager->PopScreen())
{
    UE_LOG(LogTemp, Log, TEXT("Successfully returned to previous screen"));
}
```

---

#### ClearScreenStack

画面スタックをすべてクリアします。スタック内のすべての画面が破棄されます。

```cpp
UFUNCTION(BlueprintCallable, Category = "Screen Transition")
void ClearScreenStack();
```

**使用例:**
```cpp
Manager->ClearScreenStack();
```

---

#### GetCurrentScreen

現在アクティブな画面を取得します。

```cpp
UFUNCTION(BlueprintPure, Category = "Screen Transition")
UScreenBase* GetCurrentScreen() const;
```

**戻り値:**
- 現在の画面（nullptrの場合は画面が表示されていない）

**使用例:**
```cpp
UScreenBase* CurrentScreen = Manager->GetCurrentScreen();
if (CurrentScreen)
{
    UE_LOG(LogTemp, Log, TEXT("Current screen: %s"), *CurrentScreen->GetName());
}
```

---

#### GetScreenStackDepth

画面スタックの深さを取得します。

```cpp
UFUNCTION(BlueprintPure, Category = "Screen Transition")
int32 GetScreenStackDepth() const;
```

**戻り値:**
- スタック内の画面数

**使用例:**
```cpp
int32 Depth = Manager->GetScreenStackDepth();
UE_LOG(LogTemp, Log, TEXT("Stack depth: %d"), Depth);
```

---

#### IsTransitioning

現在トランジション中かどうかを確認します。

```cpp
UFUNCTION(BlueprintPure, Category = "Screen Transition")
bool IsTransitioning() const;
```

**戻り値:**
- `true`: トランジション中
- `false`: トランジション中ではない

**使用例:**
```cpp
if (!Manager->IsTransitioning())
{
    // トランジション中でなければ入力を処理
    HandleInput();
}
```

---

#### SetDefaultTransitionEffect

デフォルトのトランジションエフェクトを設定します。

```cpp
UFUNCTION(BlueprintCallable, Category = "Screen Transition")
void SetDefaultTransitionEffect(TSubclassOf<UTransitionEffect> TransitionEffectClass);
```

**パラメータ:**
- `TransitionEffectClass`: デフォルトとして使用するエフェクトクラス

**使用例:**
```cpp
// Game Instance初期化時
void UMyGameInstance::Init()
{
    Super::Init();

    UScreenTransitionManager* Manager = GetSubsystem<UScreenTransitionManager>();
    Manager->SetDefaultTransitionEffect(UMyFadeTransition::StaticClass());
}
```

---

### プロパティ

#### OnScreenChanged

画面が変更されたときに発火するデリゲート。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
FOnScreenChanged OnScreenChanged;
```

**シグネチャ:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnScreenChanged,
    UScreenBase*, OldScreen,
    UScreenBase*, NewScreen
);
```

---

#### OnTransitionStarted

トランジションが開始されたときに発火するデリゲート。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
FOnTransitionStarted OnTransitionStarted;
```

**シグネチャ:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTransitionStarted,
    UScreenBase*, TargetScreen
);
```

---

#### OnTransitionCompleted

トランジションが完了したときに発火するデリゲート。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen Transition")
FOnTransitionCompleted OnTransitionCompleted;
```

**シグネチャ:**
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTransitionCompleted,
    UScreenBase*, CurrentScreen
);
```

---

## UScreenBase

**継承**: `UUserWidget`

すべての画面の基底クラス。画面のライフサイクルと入力設定を管理します。

### ライフサイクルメソッド

#### OnEnter

画面が表示されるときに呼ばれます。

```cpp
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
void OnEnter();
```

**オーバーライド例:**
```cpp
void UMyScreen::OnEnter_Implementation()
{
    Super::OnEnter_Implementation();

    // 画面表示時の処理
    PlayIntroAnimation();
}
```

---

#### OnExit

画面が非表示になるときに呼ばれます。

```cpp
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
void OnExit();
```

**オーバーライド例:**
```cpp
void UMyScreen::OnExit_Implementation()
{
    Super::OnExit_Implementation();

    // 画面終了時の処理
    StopBackgroundMusic();
}
```

---

#### OnPause

画面が一時停止されるときに呼ばれます（モーダルウィンドウ表示時など）。

```cpp
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
void OnPause();
```

---

#### OnResume

画面が再開されるときに呼ばれます。

```cpp
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Screen")
void OnResume();
```

---

### プロパティ

#### bShowMouseCursor

この画面でマウスカーソルを表示するかどうか。

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Screen")
bool bShowMouseCursor;
```

**デフォルト値:** `true`

---

#### bEnableClickEvents

この画面でクリックイベントを有効にするかどうか。

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Screen")
bool bEnableClickEvents;
```

**デフォルト値:** `true`

---

#### ZOrder

画面のZ順序。大きい値ほど手前に表示されます。

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Screen")
int32 ZOrder;
```

**デフォルト値:** `0`

---

#### bIsActive

画面が現在アクティブかどうか。

```cpp
UPROPERTY(BlueprintReadOnly, Category = "Screen")
bool bIsActive;
```

**読み取り専用**

---

### デリゲート

#### OnScreenEnterDelegate

OnEnterが呼ばれたときに発火。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen")
FOnScreenEnter OnScreenEnterDelegate;
```

---

#### OnScreenExitDelegate

OnExitが呼ばれたときに発火。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen")
FOnScreenExit OnScreenExitDelegate;
```

---

#### OnScreenPauseDelegate

OnPauseが呼ばれたときに発火。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen")
FOnScreenPause OnScreenPauseDelegate;
```

---

#### OnScreenResumeDelegate

OnResumeが呼ばれたときに発火。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Screen")
FOnScreenResume OnScreenResumeDelegate;
```

---

## UTransitionEffect

**継承**: `UObject`

画面遷移時のビジュアルエフェクトを管理します。

### メソッド

#### StartTransition

トランジションを開始します。

```cpp
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Transition")
void StartTransition(UScreenBase* FromScreen, UScreenBase* ToScreen);
```

**パラメータ:**
- `FromScreen`: 遷移元の画面（nullptrの可能性あり）
- `ToScreen`: 遷移先の画面

---

#### CompleteTransition

トランジションを完了します。内部で自動的に呼ばれますが、手動で呼ぶこともできます。

```cpp
UFUNCTION(BlueprintCallable, Category = "Transition")
void CompleteTransition();
```

---

#### TickTransition

トランジションの各フレームで呼ばれます。カスタムエフェクトを実装する際にオーバーライドします。

```cpp
UFUNCTION(BlueprintNativeEvent, Category = "Transition")
void TickTransition(float Alpha);
```

**パラメータ:**
- `Alpha`: 進行度（0.0 ～ 1.0）。イージング適用済み。

**オーバーライド例:**
```cpp
void UCustomTransition::TickTransition_Implementation(float Alpha)
{
    Super::TickTransition_Implementation(Alpha);

    if (ToScreenRef)
    {
        // カスタムアニメーション
        float Scale = FMath::Lerp(0.8f, 1.0f, Alpha);
        ToScreenRef->SetRenderScale(FVector2D(Scale, Scale));
    }
}
```

---

### プロパティ

#### TransitionType

トランジションのタイプ。

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transition")
ETransitionType TransitionType;
```

**デフォルト値:** `ETransitionType::Fade`

---

#### Duration

トランジションの長さ（秒）。

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transition")
float Duration;
```

**デフォルト値:** `0.5f`

---

#### SlideDirection

スライド方向（TransitionTypeがSlideの場合のみ有効）。

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transition",
    meta = (EditCondition = "TransitionType == ETransitionType::Slide"))
ESlideDirection SlideDirection;
```

**デフォルト値:** `ESlideDirection::Left`

---

#### OnTransitionComplete

トランジション完了時に発火するデリゲート。

```cpp
UPROPERTY(BlueprintAssignable, Category = "Transition")
FOnTransitionComplete OnTransitionComplete;
```

---

## Enums

### ETransitionType

トランジションエフェクトのタイプ。

```cpp
UENUM(BlueprintType)
enum class ETransitionType : uint8
{
    Fade    UMETA(DisplayName = "Fade"),    // フェードイン/アウト
    Slide   UMETA(DisplayName = "Slide"),   // スライド
    Custom  UMETA(DisplayName = "Custom")   // カスタム
};
```

---

### ESlideDirection

スライドトランジションの方向。

```cpp
UENUM(BlueprintType)
enum class ESlideDirection : uint8
{
    Left    UMETA(DisplayName = "Left"),    // 左へスライド
    Right   UMETA(DisplayName = "Right"),   // 右へスライド
    Up      UMETA(DisplayName = "Up"),      // 上へスライド
    Down    UMETA(DisplayName = "Down")     // 下へスライド
};
```

---

## Structs

### FScreenStackEntry

画面スタックのエントリ。

```cpp
USTRUCT(BlueprintType)
struct FScreenStackEntry
{
    GENERATED_BODY()

    UPROPERTY()
    UScreenBase* Screen;

    UPROPERTY()
    bool bIsModal;
};
```

**メンバー:**
- `Screen`: スタックに保存されている画面
- `bIsModal`: モーダルウィンドウかどうか

---

## Delegates

### FOnScreenChanged

画面が変更されたときのデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnScreenChanged,
    UScreenBase*, OldScreen,
    UScreenBase*, NewScreen
);
```

---

### FOnTransitionStarted

トランジション開始時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTransitionStarted,
    UScreenBase*, TargetScreen
);
```

---

### FOnTransitionCompleted

トランジション完了時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTransitionCompleted,
    UScreenBase*, CurrentScreen
);
```

---

### FOnTransitionComplete

TransitionEffect完了時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransitionComplete);
```

---

### FOnScreenEnter

画面Enter時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenEnter);
```

---

### FOnScreenExit

画面Exit時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenExit);
```

---

### FOnScreenPause

画面Pause時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenPause);
```

---

### FOnScreenResume

画面Resume時のデリゲート。

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenResume);
```

---

次のドキュメント: [サンプル集](Examples.md)
