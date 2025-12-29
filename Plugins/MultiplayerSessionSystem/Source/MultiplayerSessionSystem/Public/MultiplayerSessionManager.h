#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionData.h"
#include "MultiplayerSessionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsComplete, const TArray<FSessionRoomInfo>&, SessionResults, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroySessionComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerJoined, const FString&, PlayerName, int32, PlayerCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerLeft, const FString&, PlayerName, int32, PlayerCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHostMigrationComplete, bool, bWasSuccessful);

UCLASS()
class MULTIPLAYERSESSIONSYSTEM_API UMultiplayerSessionManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionManager();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Room Creation and Search
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void CreateSession(const FSessionSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void FindSessions(int32 MaxSearchResults = 10, bool bIsLAN = false);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void JoinSession(int32 SessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void DestroySession();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void StartSession();

	// Player Management
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void LeaveSession();

	UFUNCTION(BlueprintPure, Category = "Multiplayer Session")
	TArray<FSessionPlayerInfo> GetSessionPlayers() const;

	UFUNCTION(BlueprintPure, Category = "Multiplayer Session")
	bool IsHost() const;

	UFUNCTION(BlueprintPure, Category = "Multiplayer Session")
	ESessionState GetCurrentState() const { return CurrentState; }

	// Host Migration
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session")
	void InitiateHostMigration();

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnCreateSessionComplete OnCreateSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnFindSessionsComplete OnFindSessionsComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnJoinSessionComplete OnJoinSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnDestroySessionComplete OnDestroySessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnStartSessionComplete OnStartSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnPlayerLeft OnPlayerLeft;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Session")
	FOnHostMigrationComplete OnHostMigrationComplete;

protected:
	// Online Session Interface Callbacks
	void OnCreateSessionCompleteInternal(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleteInternal(bool bWasSuccessful);
	void OnJoinSessionCompleteInternal(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionCompleteInternal(FName SessionName, bool bWasSuccessful);
	void OnStartSessionCompleteInternal(FName SessionName, bool bWasSuccessful);
	void OnSessionUserInviteAcceptedInternal(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);

	// Session Management
	void SetupSessionSettings(FOnlineSessionSettings& SessionSettings, const FSessionSettings& Settings);
	void UpdatePlayerList();
	void SyncSessionData();

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FSessionSettings LastSessionSettings;
	ESessionState CurrentState;
	TArray<FSessionPlayerInfo> SessionPlayers;
	bool bIsHostPlayer;

	// Delegates
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	// Delegate Handles
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;
};
