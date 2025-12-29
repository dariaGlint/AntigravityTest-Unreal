#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationFollowerComponent.generated.h"

class UFormationComponent;
class AAIController;

/**
 * Component for actors that follow a formation leader
 * Attach to party members to enable formation following behavior
 */
UCLASS(ClassGroup=(Formation), meta=(BlueprintSpawnableComponent))
class FORMATIONSYSTEM_API UFormationFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFormationFollowerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Set the formation leader to follow
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SetFormationLeader(AActor* Leader);

	// Get the current formation leader
	UFUNCTION(BlueprintPure, Category = "Formation")
	AActor* GetFormationLeader() const { return FormationLeader; }

	// Get the target formation position
	UFUNCTION(BlueprintPure, Category = "Formation")
	FVector GetTargetFormationPosition() const;

	// Enable or disable following
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SetFollowingEnabled(bool bEnabled);

	// Check if following is enabled
	UFUNCTION(BlueprintPure, Category = "Formation")
	bool IsFollowingEnabled() const { return bFollowingEnabled; }

	// Set the follower's index in the formation
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SetFormationIndex(int32 Index) { FormationIndex = Index; }

	// Get the follower's index in the formation
	UFUNCTION(BlueprintPure, Category = "Formation")
	int32 GetFormationIndex() const { return FormationIndex; }

	// Force immediate move to formation position (teleport if too far)
	UFUNCTION(BlueprintCallable, Category = "Formation")
	void SnapToFormationPosition();

protected:
	virtual void BeginPlay() override;

	// Update movement toward formation position
	void UpdateFollowMovement();

	// Check if we should teleport to formation position
	bool ShouldTeleportToFormation() const;

	// Move to target position using AI navigation
	void MoveToTargetPosition(const FVector& TargetPosition);

private:
	// The formation leader to follow
	UPROPERTY()
	AActor* FormationLeader;

	// Reference to the leader's formation component
	UPROPERTY()
	UFormationComponent* LeaderFormationComponent;

	// This follower's index in the formation
	UPROPERTY(EditAnywhere, Category = "Formation")
	int32 FormationIndex;

	// Whether following is enabled
	UPROPERTY(EditAnywhere, Category = "Formation")
	bool bFollowingEnabled;

	// Distance threshold to start moving to formation position
	UPROPERTY(EditAnywhere, Category = "Formation")
	float MovementThreshold;

	// Distance threshold to consider arrived at formation position
	UPROPERTY(EditAnywhere, Category = "Formation")
	float ArrivalThreshold;

	// Whether to use AI navigation for pathfinding
	UPROPERTY(EditAnywhere, Category = "Formation")
	bool bUseAINavigation;

	// Whether to teleport if too far from formation position
	UPROPERTY(EditAnywhere, Category = "Formation")
	bool bAllowTeleport;

	// Distance at which to teleport instead of walking
	UPROPERTY(EditAnywhere, Category = "Formation")
	float TeleportDistance;

	// Cached AI controller reference
	UPROPERTY()
	AAIController* CachedAIController;
};
