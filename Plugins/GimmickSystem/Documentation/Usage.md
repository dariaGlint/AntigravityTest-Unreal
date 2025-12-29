# Gimmick System 使用例 / Usage Examples

## 基本的な使い方 / Basic Usage

### 1. シンプルなドアの作成 / Creating a Simple Door

Blueprintでドアを作成する場合：

1. Content Browserで右クリック → Blueprint Class → DoorGimmick
2. Blueprintエディタを開く
3. DoorMeshにスタティックメッシュを設定
4. アニメーション実装（オプション）

```cpp
// C++での作成例
ADoorGimmick* Door = GetWorld()->SpawnActor<ADoorGimmick>(
    ADoorGimmick::StaticClass(),
    FVector(0, 0, 0),
    FRotator::ZeroRotator
);

Door->SetDoorState(EDoorState::Closed);
```

### 2. 施錠されたドアの作成 / Creating a Locked Door

```cpp
// ドアを施錠状態で作成
Door->SetDoorState(EDoorState::Locked);

// 鍵条件を追加
UKeyCondition* KeyCondition = NewObject<UKeyCondition>(Door);
KeyCondition->RequiredKeyTag = FGameplayTag::RequestGameplayTag("Item.Key.GoldKey");
Door->AddCondition(KeyCondition);
```

### 3. パズルで解除されるドア / Door Unlocked by Puzzle

```cpp
// パズルを作成
APuzzleBase* Puzzle = GetWorld()->SpawnActor<APuzzleBase>();

// ドアにパズル条件を追加
UPuzzleCondition* PuzzleCondition = NewObject<UPuzzleCondition>(Door);
PuzzleCondition->TargetPuzzle = Puzzle;
Door->AddCondition(PuzzleCondition);
```

## アニメーションの実装 / Implementing Animations

### Blueprint でのアニメーション実装

DoorGimmick のBlueprintで以下のイベントを実装：

1. **PlayOpenAnimation イベント**
   - タイムライン または アニメーションを再生
   - 完了時に `OnOpenAnimationComplete` を呼び出す

2. **PlayCloseAnimation イベント**
   - 閉じるアニメーションを再生
   - 完了時に `OnCloseAnimationComplete` を呼び出す

例：
```
Event PlayOpenAnimation
  → Play Timeline (0 to 90 degrees)
  → Timeline Finished
    → Call OnOpenAnimationComplete
```

### C++ でのアニメーション実装

```cpp
void AMyDoor::PlayOpenAnimation_Implementation()
{
    // タイムラインやアニメーションを開始

    // アニメーション完了時
    GetWorld()->GetTimerManager().SetTimer(
        AnimationTimer,
        this,
        &AMyDoor::OnOpenAnimationComplete,
        AnimationDuration,
        false
    );
}
```

## インベントリシステムとの連携 / Integration with Inventory System

### キャラクターにインベントリインターフェースを実装

```cpp
// MyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryInterface.h"
#include "GameplayTagContainer.h"
#include "MyCharacter.generated.h"

UCLASS()
class AMyCharacter : public ACharacter, public IInventoryInterface
{
    GENERATED_BODY()

public:
    virtual bool HasItemWithTag_Implementation(const FGameplayTag& Tag) const override;

    UFUNCTION(BlueprintCallable)
    void AddItem(const FGameplayTag& ItemTag);

protected:
    UPROPERTY()
    TArray<FGameplayTag> Inventory;
};
```

```cpp
// MyCharacter.cpp
bool AMyCharacter::HasItemWithTag_Implementation(const FGameplayTag& Tag) const
{
    return Inventory.Contains(Tag);
}

void AMyCharacter::AddItem(const FGameplayTag& ItemTag)
{
    if (!Inventory.Contains(ItemTag))
    {
        Inventory.Add(ItemTag);
    }
}
```

## インタラクションシステムの実装 / Implementing Interaction System

### プレイヤーコントローラーでのインタラクション処理

```cpp
void AMyPlayerController::OnInteractPressed()
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
        return;

    // 最寄りのインタラクション可能オブジェクトを検索
    UGimmickManager* Manager = GetWorld()->GetSubsystem<UGimmickManager>();
    AInteractableBase* Nearest = Manager->GetNearestInteractable(ControlledPawn, 300.0f);

    if (Nearest && Nearest->CanInteract(ControlledPawn))
    {
        Nearest->Interact(ControlledPawn);
    }
}
```

### UI プロンプトの表示

```cpp
void AMyPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* ControlledPawn = GetPawn();
    UGimmickManager* Manager = GetWorld()->GetSubsystem<UGimmickManager>();
    AInteractableBase* Nearest = Manager->GetNearestInteractable(ControlledPawn, 300.0f);

    if (Nearest && Nearest->CanInteract(ControlledPawn))
    {
        // UIにプロンプトを表示
        FText Prompt = Nearest->GetInteractionPrompt();
        // ShowInteractionPrompt(Prompt);
    }
    else
    {
        // プロンプトを非表示
        // HideInteractionPrompt();
    }
}
```

## カスタムギミックの作成 / Creating Custom Gimmicks

### カスタムインタラクション可能オブジェクト

```cpp
// LeverGimmick.h
#pragma once

#include "InteractableBase.h"
#include "LeverGimmick.generated.h"

UCLASS()
class ALeverGimmick : public AInteractableBase
{
    GENERATED_BODY()

public:
    ALeverGimmick();

    virtual void OnInteract_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintImplementableEvent)
    void OnLeverPulled();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsActivated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<AActor*> TargetActors;
};
```

```cpp
// LeverGimmick.cpp
#include "LeverGimmick.h"

ALeverGimmick::ALeverGimmick()
{
    bIsActivated = false;
    InteractionPrompt = FText::FromString(TEXT("Pull Lever"));
}

void ALeverGimmick::OnInteract_Implementation(AActor* Interactor)
{
    Super::OnInteract_Implementation(Interactor);

    bIsActivated = !bIsActivated;

    OnLeverPulled();

    // ターゲットアクターに何か処理を実行
    for (AActor* Target : TargetActors)
    {
        // 例：ドアを開く
        if (ADoorGimmick* Door = Cast<ADoorGimmick>(Target))
        {
            if (bIsActivated)
                Door->UnlockDoor();
            else
                Door->LockDoor();
        }
    }
}
```

## カスタム条件の作成 / Creating Custom Conditions

### レベル条件の例

```cpp
// LevelCondition.h
#pragma once

#include "GimmickConditionBase.h"
#include "LevelCondition.generated.h"

UCLASS()
class ULevelCondition : public UGimmickConditionBase
{
    GENERATED_BODY()

public:
    ULevelCondition();

    virtual bool IsMet_Implementation(AActor* Interactor) const override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredLevel;
};
```

```cpp
// LevelCondition.cpp
#include "LevelCondition.h"
#include "MyCharacter.h"

ULevelCondition::ULevelCondition()
{
    RequiredLevel = 1;
    ConditionDescription = FText::FromString(TEXT("Requires Level"));
}

bool ULevelCondition::IsMet_Implementation(AActor* Interactor) const
{
    AMyCharacter* Character = Cast<AMyCharacter>(Interactor);
    if (!Character)
        return false;

    return Character->GetLevel() >= RequiredLevel;
}
```

## ギミックチェーンの実装 / Implementing Gimmick Chains

複数のギミックを連携させる例：

```cpp
void AMyGameMode::SetupGimmickChain()
{
    // 3つのレバーを作成
    ALeverGimmick* Lever1 = SpawnLever(FVector(0, 0, 0));
    ALeverGimmick* Lever2 = SpawnLever(FVector(100, 0, 0));
    ALeverGimmick* Lever3 = SpawnLever(FVector(200, 0, 0));

    // すべてのレバーが引かれたらドアを開く
    ADoorGimmick* Door = SpawnDoor(FVector(300, 0, 0));

    // 各レバーにイベントをバインド
    Lever1->OnInteractionCompleted.AddDynamic(this, &AMyGameMode::CheckAllLeversActivated);
    Lever2->OnInteractionCompleted.AddDynamic(this, &AMyGameMode::CheckAllLeversActivated);
    Lever3->OnInteractionCompleted.AddDynamic(this, &AMyGameMode::CheckAllLeversActivated);
}

void AMyGameMode::CheckAllLeversActivated(AActor* Interactor)
{
    // すべてのレバーがアクティブかチェック
    bool bAllActivated = true;
    // ... チェックロジック

    if (bAllActivated)
    {
        Door->UnlockDoor();
        Door->OpenDoor(nullptr);
    }
}
```

## デバッグとテスト / Debugging and Testing

### ギミックマネージャーでのデバッグ

```cpp
void AMyPlayerController::DebugGimmicks()
{
    UGimmickManager* Manager = GetWorld()->GetSubsystem<UGimmickManager>();

    TArray<ADoorGimmick*> Doors = Manager->GetAllDoors();
    for (ADoorGimmick* Door : Doors)
    {
        UE_LOG(LogTemp, Log, TEXT("Door: %s, State: %d"),
            *Door->GetName(),
            (int32)Door->GetDoorState());
    }
}
```

### コンソールコマンド

```cpp
// MyPlayerController.h に追加
UFUNCTION(Exec)
void UnlockAllDoors();

// MyPlayerController.cpp
void AMyPlayerController::UnlockAllDoors()
{
    UGimmickManager* Manager = GetWorld()->GetSubsystem<UGimmickManager>();
    TArray<ADoorGimmick*> Doors = Manager->GetAllDoors();

    for (ADoorGimmick* Door : Doors)
    {
        Door->UnlockDoor();
    }
}
```

これでコンソールから `UnlockAllDoors` と入力してすべてのドアを解錠できます。
