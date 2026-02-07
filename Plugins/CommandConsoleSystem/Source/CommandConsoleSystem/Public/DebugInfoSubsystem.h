// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugInfoSubsystem.generated.h"

/**
 * Subsystem for managing debug information display
 */
UCLASS()
class COMMANDCONSOLESYSTEM_API UDebugInfoSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Toggle FPS display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void ToggleFPS(bool bShow);

	/**
	 * Toggle memory usage display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void ToggleMemoryInfo(bool bShow);

	/**
	 * Toggle network stats display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void ToggleNetworkStats(bool bShow);

	/**
	 * Toggle AI debug display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void ToggleAIDebug(bool bShow);

	/**
	 * Toggle physics debug display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void TogglePhysicsDebug(bool bShow);

	/**
	 * Toggle collision debug display
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void ToggleCollisionDebug(bool bShow);

	/**
	 * Show all debug info
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void ShowAllDebugInfo();

	/**
	 * Hide all debug info
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void HideAllDebugInfo();

	/**
	 * Log system information
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Info")
	void LogSystemInfo();

private:
	void RegisterDebugCommands();
	void UnregisterDebugCommands();

	// Command handlers
	void HandleToggleFPS(const TArray<FString>& Args);
	void HandleToggleMemory(const TArray<FString>& Args);
	void HandleToggleNetwork(const TArray<FString>& Args);
	void HandleToggleAI(const TArray<FString>& Args);
	void HandleTogglePhysics(const TArray<FString>& Args);
	void HandleToggleCollision(const TArray<FString>& Args);
	void HandleShowAll(const TArray<FString>& Args);
	void HandleHideAll(const TArray<FString>& Args);
	void HandleSystemInfo(const TArray<FString>& Args);

	bool bShowFPS;
	bool bShowMemory;
	bool bShowNetwork;
	bool bShowAI;
	bool bShowPhysics;
	bool bShowCollision;
};
