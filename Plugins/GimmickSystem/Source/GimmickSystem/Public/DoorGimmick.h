#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "DoorGimmick.generated.h"

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Closed		UMETA(DisplayName = "Closed"),
	Open		UMETA(DisplayName = "Open"),
	Locked		UMETA(DisplayName = "Locked"),
	Opening		UMETA(DisplayName = "Opening"),
	Closing		UMETA(DisplayName = "Closing")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpened, AActor*, Opener);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorClosed, AActor*, Closer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorLocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorUnlocked);

/**
 * ドアギミッククラス
 * Door gimmick with state management and animation support
 */
UCLASS()
class GIMMICKSYSTEM_API ADoorGimmick : public AInteractableBase
{
	GENERATED_BODY()

public:
	ADoorGimmick();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;

	/**
	 * ドアを開く
	 * Open the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor(AActor* Opener);

	/**
	 * ドアを閉じる
	 * Close the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor(AActor* Closer);

	/**
	 * ドアを施錠
	 * Lock the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void LockDoor();

	/**
	 * ドアを解錠
	 * Unlock the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void UnlockDoor();

	/**
	 * 現在の状態を取得
	 * Get current door state
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door")
	EDoorState GetDoorState() const { return CurrentState; }

	/**
	 * 状態を設定
	 * Set door state
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetDoorState(EDoorState NewState);

	/**
	 * ドアが開いているか
	 * Check if door is open
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door")
	bool IsOpen() const { return CurrentState == EDoorState::Open; }

	/**
	 * ドアが閉じているか
	 * Check if door is closed
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door")
	bool IsClosed() const { return CurrentState == EDoorState::Closed; }

	/**
	 * ドアが施錠されているか
	 * Check if door is locked
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door")
	bool IsLocked() const { return CurrentState == EDoorState::Locked; }

	/**
	 * アニメーション再生（Blueprintでオーバーライド）
	 * Play animation (override in Blueprint)
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Door|Animation")
	void PlayOpenAnimation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Door|Animation")
	void PlayCloseAnimation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Door|Animation")
	void PlayLockAnimation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Door|Animation")
	void PlayUnlockAnimation();

	/**
	 * アニメーション完了時のコールバック
	 * Callback when animation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Door|Animation")
	void OnOpenAnimationComplete();

	UFUNCTION(BlueprintCallable, Category = "Door|Animation")
	void OnCloseAnimationComplete();

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorOpened OnDoorOpened;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorClosed OnDoorClosed;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorLocked OnDoorLocked;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorUnlocked OnDoorUnlocked;

protected:
	/** 現在の状態 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	EDoorState CurrentState;

	/** 初期状態 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	EDoorState InitialState;

	/** インタラクション時の動作（トグル or 開く） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bToggleOnInteract;

	/** 自動で閉じる */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bAutoClose;

	/** 自動で閉じるまでの時間 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "bAutoClose"))
	float AutoCloseDelay;

	/** ドアメッシュコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

private:
	FTimerHandle AutoCloseTimerHandle;

	void StartAutoCloseTimer();
};
