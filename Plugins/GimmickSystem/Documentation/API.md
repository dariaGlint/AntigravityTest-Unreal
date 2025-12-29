# Gimmick System API Reference

## Core Classes

### AInteractableBase

インタラクション可能オブジェクトの基底クラス

#### Public Methods

- `bool CanInteract(AActor* Interactor)` - インタラクション可能かチェック
- `void Interact(AActor* Interactor)` - インタラクションを実行
- `void OnInteract(AActor* Interactor)` - インタラクション処理（オーバーライド用）
- `FText GetInteractionPrompt()` - インタラクション表示名を取得
- `void AddCondition(UGimmickConditionBase* Condition)` - 条件を追加
- `bool CheckAllConditions(AActor* Interactor)` - すべての条件をチェック
- `bool IsInRange(AActor* Interactor)` - 範囲内チェック

#### Properties

- `FText InteractionPrompt` - インタラクション表示テキスト
- `bool bIsEnabled` - 有効/無効
- `bool bCanBeUsedMultipleTimes` - 繰り返し使用可能か
- `int32 UsageCount` - 使用回数
- `TArray<UGimmickConditionBase*> Conditions` - 条件リスト
- `float InteractionRange` - インタラクション範囲

#### Delegates

- `FOnInteractionStarted OnInteractionStarted` - インタラクション開始時
- `FOnInteractionCompleted OnInteractionCompleted` - インタラクション完了時
- `FOnInteractionFailed OnInteractionFailed` - インタラクション失敗時
- `FOnStateChanged OnStateChanged` - 状態変更時

---

### ADoorGimmick

ドアギミッククラス

#### Public Methods

- `void OpenDoor(AActor* Opener)` - ドアを開く
- `void CloseDoor(AActor* Closer)` - ドアを閉じる
- `void LockDoor()` - ドアを施錠
- `void UnlockDoor()` - ドアを解錠
- `EDoorState GetDoorState()` - 現在の状態を取得
- `void SetDoorState(EDoorState NewState)` - 状態を設定
- `bool IsOpen()` - ドアが開いているか
- `bool IsClosed()` - ドアが閉じているか
- `bool IsLocked()` - ドアが施錠されているか

#### Blueprint Events

- `PlayOpenAnimation()` - 開くアニメーション再生
- `PlayCloseAnimation()` - 閉じるアニメーション再生
- `PlayLockAnimation()` - 施錠アニメーション再生
- `PlayUnlockAnimation()` - 解錠アニメーション再生

アニメーション完了時に以下を呼び出す必要があります：
- `OnOpenAnimationComplete()` - 開くアニメーション完了
- `OnCloseAnimationComplete()` - 閉じるアニメーション完了

#### Properties

- `EDoorState CurrentState` - 現在の状態
- `EDoorState InitialState` - 初期状態
- `bool bToggleOnInteract` - インタラクション時にトグルするか
- `bool bAutoClose` - 自動で閉じるか
- `float AutoCloseDelay` - 自動で閉じるまでの時間

#### Delegates

- `FOnDoorOpened OnDoorOpened` - ドアが開いた時
- `FOnDoorClosed OnDoorClosed` - ドアが閉じた時
- `FOnDoorLocked OnDoorLocked` - ドアが施錠された時
- `FOnDoorUnlocked OnDoorUnlocked` - ドアが解錠された時

#### Enums

```cpp
enum class EDoorState : uint8
{
    Closed,   // 閉じている
    Open,     // 開いている
    Locked,   // 施錠されている
    Opening,  // 開いている途中
    Closing   // 閉じている途中
};
```

---

### APuzzleBase

パズルの基底クラス

#### Public Methods

- `bool IsSolved()` - パズルが解かれたかチェック
- `void ResetPuzzle()` - パズルをリセット
- `void SolvePuzzle()` - パズルを解決済みにする
- `bool ValidateSolution()` - パズルの検証（オーバーライド用）

#### Properties

- `bool bIsSolved` - 解決済みフラグ
- `bool bCanReset` - リセット可能か

#### Delegates

- `FOnPuzzleSolved OnPuzzleSolved` - パズルが解かれた時
- `FOnPuzzleReset OnPuzzleReset` - パズルがリセットされた時

---

## Condition Classes

### UGimmickConditionBase

ギミック条件の基底クラス

#### Public Methods

- `bool IsMet(AActor* Interactor)` - 条件が満たされているかチェック（オーバーライド用）
- `FText GetConditionDescription()` - 条件の説明を取得

---

### UKeyCondition

鍵所持チェック条件

#### Properties

- `FGameplayTag RequiredKeyTag` - 必要な鍵のタグ

#### Public Methods

- `bool HasKey(AActor* Actor)` - アクターが鍵を持っているかチェック

---

### UPuzzleCondition

パズル解決チェック条件

#### Properties

- `APuzzleBase* TargetPuzzle` - 参照するパズル

---

## Interfaces

### IInventoryInterface

インベントリインターフェース

#### Methods

- `bool HasItemWithTag(const FGameplayTag& Tag)` - 特定のタグを持つアイテムを持っているかチェック

実装例：
```cpp
bool AMyCharacter::HasItemWithTag_Implementation(const FGameplayTag& Tag) const
{
    return Inventory.Contains(Tag);
}
```

---

## Manager System

### UGimmickManager

ギミック管理サブシステム（WorldSubsystem）

#### Public Methods

- `void RegisterGimmick(AInteractableBase* Gimmick)` - ギミックを登録
- `void UnregisterGimmick(AInteractableBase* Gimmick)` - ギミックの登録を解除
- `AInteractableBase* FindGimmickByID(FName GimmickID)` - IDでギミックを検索
- `TArray<ADoorGimmick*> GetAllDoors()` - すべてのドアを取得
- `TArray<APuzzleBase*> GetAllPuzzles()` - すべてのパズルを取得
- `AInteractableBase* GetNearestInteractable(AActor* Interactor, float MaxDistance)` - 最寄りのインタラクション可能オブジェクトを取得
- `void SaveAllGimmickStates()` - すべてのギミック状態を保存
- `void LoadAllGimmickStates()` - すべてのギミック状態を読み込み

使用例：
```cpp
UGimmickManager* Manager = GetWorld()->GetSubsystem<UGimmickManager>();
AInteractableBase* Nearest = Manager->GetNearestInteractable(PlayerCharacter, 500.0f);
if (Nearest && Nearest->CanInteract(PlayerCharacter))
{
    Nearest->Interact(PlayerCharacter);
}
```
