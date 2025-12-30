// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatTargetingComponent.generated.h"

class ACombatEnemy;

/** Delegate for target lock state changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetLockChanged, AActor*, NewTarget);

/**
 * Component that handles enemy targeting and lock-on functionality.
 * Provides enemy detection, priority-based target selection, and lock-on management.
 */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class UCombatTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/** Maximum distance to detect enemies */
	UPROPERTY(EditAnywhere, Category="Targeting", meta = (ClampMin = 0, ClampMax = 5000, Units = "cm"))
	float DetectionRadius = 1500.0f;

	/** Maximum angle from camera forward to consider an enemy for targeting (in degrees) */
	UPROPERTY(EditAnywhere, Category="Targeting", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float DetectionAngle = 60.0f;

	/** How much to prioritize closer enemies (higher = closer enemies preferred) */
	UPROPERTY(EditAnywhere, Category="Targeting", meta = (ClampMin = 0, ClampMax = 10))
	float DistancePriorityWeight = 1.0f;

	/** How much to prioritize enemies in front of camera (higher = center screen preferred) */
	UPROPERTY(EditAnywhere, Category="Targeting", meta = (ClampMin = 0, ClampMax = 10))
	float AnglePriorityWeight = 2.0f;

	/** Minimum distance before auto-unlocking target */
	UPROPERTY(EditAnywhere, Category="Targeting", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
	float MaxLockDistance = 2000.0f;

	/** If true, targeting is currently locked onto an enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	bool bIsTargetLocked = false;

	/** Current locked target */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	TObjectPtr<AActor> CurrentTarget;

	/** List of all detected enemies in range */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	TArray<TObjectPtr<AActor>> DetectedEnemies;

public:

	/** Delegate called when target lock state changes */
	UPROPERTY(BlueprintAssignable, Category="Targeting")
	FOnTargetLockChanged OnTargetLockChanged;

public:

	/** Constructor */
	UCombatTargetingComponent();

	/** Toggles target lock on/off */
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void ToggleTargetLock();

	/** Manually sets the target lock to a specific actor */
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void SetTargetLock(AActor* NewTarget);

	/** Clears the current target lock */
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void ClearTargetLock();

	/** Switches to the next available target (cycles through detected enemies) */
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void SwitchToNextTarget();

	/** Switches to the previous available target (cycles through detected enemies) */
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void SwitchToPreviousTarget();

	/** Returns the current locked target */
	UFUNCTION(BlueprintPure, Category="Targeting")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	/** Returns true if a target is currently locked */
	UFUNCTION(BlueprintPure, Category="Targeting")
	bool IsTargetLocked() const { return bIsTargetLocked; }

	/** Returns all currently detected enemies */
	UFUNCTION(BlueprintPure, Category="Targeting")
	const TArray<TObjectPtr<AActor>>& GetDetectedEnemies() const { return DetectedEnemies; }

	/** Blueprint event called when target lock changes */
	UFUNCTION(BlueprintImplementableEvent, Category="Targeting")
	void BP_OnTargetLockChanged(AActor* NewTarget);

protected:

	/** Component tick to update target tracking */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Scans for enemies in detection range */
	void ScanForEnemies();

	/** Finds the best target based on priority scoring */
	AActor* FindBestTarget() const;

	/** Calculates priority score for a potential target */
	float CalculateTargetPriority(AActor* PotentialTarget) const;

	/** Validates if the current target is still valid */
	bool IsCurrentTargetValid() const;

	/** Gets the owner's camera location and direction */
	bool GetCameraInfo(FVector& OutLocation, FVector& OutForward) const;
};
