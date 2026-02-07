// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CheatCommandSubsystem.generated.h"

/**
 * Subsystem for registering and managing cheat commands
 */
UCLASS()
class COMMANDCONSOLESYSTEM_API UCheatCommandSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Enable god mode (invincibility)
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void GodMode(bool bEnable);

	/**
	 * Enable no clip mode (fly through walls)
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void NoClip(bool bEnable);

	/**
	 * Teleport player to coordinates
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void Teleport(float X, float Y, float Z);

	/**
	 * Add item to inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void GiveItem(const FString& ItemName, int32 Quantity);

	/**
	 * Set player health
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void SetHealth(float Health);

	/**
	 * Kill all enemies
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void KillAllEnemies();

	/**
	 * Set time scale
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void SetTimeScale(float TimeScale);

	/**
	 * Unlock all content
	 */
	UFUNCTION(BlueprintCallable, Category = "Cheat Commands")
	void UnlockAll();

private:
	void RegisterCheatCommands();
	void UnregisterCheatCommands();

	// Command handlers
	void HandleGodMode(const TArray<FString>& Args);
	void HandleNoClip(const TArray<FString>& Args);
	void HandleTeleport(const TArray<FString>& Args);
	void HandleGiveItem(const TArray<FString>& Args);
	void HandleSetHealth(const TArray<FString>& Args);
	void HandleKillAllEnemies(const TArray<FString>& Args);
	void HandleSetTimeScale(const TArray<FString>& Args);
	void HandleUnlockAll(const TArray<FString>& Args);

	bool bGodModeEnabled;
	bool bNoClipEnabled;
};
