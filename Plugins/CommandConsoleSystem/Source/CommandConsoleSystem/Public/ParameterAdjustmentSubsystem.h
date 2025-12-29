// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ParameterAdjustmentSubsystem.generated.h"

/**
 * Subsystem for adjusting game parameters via console commands
 */
UCLASS()
class COMMANDCONSOLESYSTEM_API UParameterAdjustmentSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Set player movement speed multiplier
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetSpeedMultiplier(float Multiplier);

	/**
	 * Set gravity scale
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetGravityScale(float Scale);

	/**
	 * Set jump height multiplier
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetJumpHeightMultiplier(float Multiplier);

	/**
	 * Set damage multiplier
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetDamageMultiplier(float Multiplier);

	/**
	 * Set experience gain multiplier
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetExpMultiplier(float Multiplier);

	/**
	 * Set render distance
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetRenderDistance(float Distance);

	/**
	 * Set FOV (Field of View)
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void SetFOV(float FOV);

	/**
	 * Reset all parameters to default
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void ResetAllParameters();

	/**
	 * List all current parameter values
	 */
	UFUNCTION(BlueprintCallable, Category = "Parameter Adjustment")
	void ListParameters();

private:
	void RegisterParameterCommands();
	void UnregisterParameterCommands();

	// Command handlers
	void HandleSetSpeed(const TArray<FString>& Args);
	void HandleSetGravity(const TArray<FString>& Args);
	void HandleSetJump(const TArray<FString>& Args);
	void HandleSetDamage(const TArray<FString>& Args);
	void HandleSetExp(const TArray<FString>& Args);
	void HandleSetRenderDistance(const TArray<FString>& Args);
	void HandleSetFOV(const TArray<FString>& Args);
	void HandleResetParams(const TArray<FString>& Args);
	void HandleListParams(const TArray<FString>& Args);

	// Current parameter values
	float SpeedMultiplier;
	float GravityScale;
	float JumpHeightMultiplier;
	float DamageMultiplier;
	float ExpMultiplier;
	float RenderDistance;
	float CurrentFOV;
};
