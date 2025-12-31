#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatEventListener.generated.h"

class UEventLogSubsystem;

/**
 * Component that listens to combat events and logs them
 * Add this to combat characters to automatically log combat events
 */
UCLASS(ClassGroup = (Debug), meta = (BlueprintSpawnableComponent))
class DEBUGEVENTLOGVIEWER_API UCombatEventListener : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatEventListener();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Log a damage event
	 * Call this when the actor takes or deals damage
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void LogDamageEvent(float DamageAmount, AActor* DamageCauser, const FVector& DamageLocation);

	/**
	 * Log a death event
	 * Call this when the actor dies
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void LogDeathEvent(AActor* Killer);

	/**
	 * Log an attack event
	 * Call this when the actor performs an attack
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void LogAttackEvent(const FString& AttackType, AActor* Target);

	/**
	 * Log a heal event
	 * Call this when the actor is healed
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Event Log")
	void LogHealEvent(float HealAmount, AActor* Healer);

protected:
	UPROPERTY()
	TObjectPtr<UEventLogSubsystem> EventLogSubsystem;
};
