#include "MultiplayerSessionManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionManager::UMultiplayerSessionManager()
	: CurrentState(ESessionState::Idle)
	, bIsHostPlayer(false)
{
}

void UMultiplayerSessionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			// Bind delegates
			OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionManager::OnCreateSessionCompleteInternal);
			OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionManager::OnFindSessionsCompleteInternal);
			OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionManager::OnJoinSessionCompleteInternal);
			OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionManager::OnDestroySessionCompleteInternal);
			OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionManager::OnStartSessionCompleteInternal);
			OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UMultiplayerSessionManager::OnSessionUserInviteAcceptedInternal);
		}
	}
}

void UMultiplayerSessionManager::Deinitialize()
{
	if (SessionInterface.IsValid())
	{
		// Clear all delegate handles
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
	}

	Super::Deinitialize();
}

void UMultiplayerSessionManager::CreateSession(const FSessionSettings& Settings)
{
	if (!SessionInterface.IsValid())
	{
		OnCreateSessionComplete.Broadcast(false);
		return;
	}

	// Check if session already exists
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	CurrentState = ESessionState::Creating;
	LastSessionSettings = Settings;
	bIsHostPlayer = true;

	// Add the delegate
	OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	// Create session settings
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SetupSessionSettings(*SessionSettings, Settings);

	// Create the session
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		OnCreateSessionComplete.Broadcast(false);
		CurrentState = ESessionState::Error;
	}
}

void UMultiplayerSessionManager::FindSessions(int32 MaxSearchResults, bool bIsLAN)
{
	if (!SessionInterface.IsValid())
	{
		TArray<FSessionRoomInfo> EmptyResults;
		OnFindSessionsComplete.Broadcast(EmptyResults, false);
		return;
	}

	CurrentState = ESessionState::Searching;

	OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = MaxSearchResults;
	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		TArray<FSessionRoomInfo> EmptyResults;
		OnFindSessionsComplete.Broadcast(EmptyResults, false);
		CurrentState = ESessionState::Error;
	}
}

void UMultiplayerSessionManager::JoinSession(int32 SessionIndex)
{
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
	{
		OnJoinSessionComplete.Broadcast(false);
		return;
	}

	if (SessionSearch->SearchResults.Num() <= SessionIndex)
	{
		OnJoinSessionComplete.Broadcast(false);
		return;
	}

	CurrentState = ESessionState::Joining;
	bIsHostPlayer = false;

	OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[SessionIndex]))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		OnJoinSessionComplete.Broadcast(false);
		CurrentState = ESessionState::Error;
	}
}

void UMultiplayerSessionManager::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		OnDestroySessionComplete.Broadcast();
		return;
	}

	CurrentState = ESessionState::Leaving;

	OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		OnDestroySessionComplete.Broadcast();
		CurrentState = ESessionState::Error;
	}
}

void UMultiplayerSessionManager::StartSession()
{
	if (!SessionInterface.IsValid())
	{
		OnStartSessionComplete.Broadcast(false);
		return;
	}

	OnStartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		OnStartSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionManager::LeaveSession()
{
	DestroySession();
}

TArray<FSessionPlayerInfo> UMultiplayerSessionManager::GetSessionPlayers() const
{
	return SessionPlayers;
}

bool UMultiplayerSessionManager::IsHost() const
{
	return bIsHostPlayer;
}

void UMultiplayerSessionManager::InitiateHostMigration()
{
	if (!SessionInterface.IsValid())
	{
		OnHostMigrationComplete.Broadcast(false);
		return;
	}

	// Host migration logic
	// In a real implementation, this would involve:
	// 1. Selecting a new host from available players
	// 2. Transferring session ownership
	// 3. Updating all clients with new host information
	// 4. Synchronizing session state

	// For now, we'll mark the first non-host player as the new host
	bool bMigrationSuccessful = false;
	for (FSessionPlayerInfo& Player : SessionPlayers)
	{
		if (!Player.bIsHost)
		{
			Player.bIsHost = true;
			bIsHostPlayer = true;
			bMigrationSuccessful = true;
			SyncSessionData();
			break;
		}
	}

	OnHostMigrationComplete.Broadcast(bMigrationSuccessful);
}

void UMultiplayerSessionManager::OnCreateSessionCompleteInternal(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		CurrentState = ESessionState::InSession;
		UpdatePlayerList();
	}
	else
	{
		CurrentState = ESessionState::Error;
	}

	OnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionManager::OnFindSessionsCompleteInternal(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}

	TArray<FSessionRoomInfo> Results;

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FSessionRoomInfo RoomInfo;
			SearchResult.Session.SessionSettings.Get(FName("RoomName"), RoomInfo.RoomName);
			SearchResult.Session.SessionSettings.Get(FName("HostName"), RoomInfo.HostName);
			SearchResult.Session.SessionSettings.Get(FName("MapName"), RoomInfo.MapName);
			SearchResult.Session.SessionSettings.Get(FName("GameMode"), RoomInfo.GameMode);

			RoomInfo.CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
			RoomInfo.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			RoomInfo.Ping = SearchResult.PingInMs;
			RoomInfo.bIsLAN = SearchResult.Session.SessionSettings.bIsLANMatch;
			RoomInfo.bIsPasswordProtected = !SearchResult.Session.SessionSettings.Settings.FindRef(FName("Password")).Data.ToString().IsEmpty();

			Results.Add(RoomInfo);
		}
	}

	CurrentState = ESessionState::Idle;
	OnFindSessionsComplete.Broadcast(Results, bWasSuccessful);
}

void UMultiplayerSessionManager::OnJoinSessionCompleteInternal(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}

	bool bWasSuccessful = (Result == EOnJoinSessionCompleteResult::Success);

	if (bWasSuccessful)
	{
		CurrentState = ESessionState::InSession;
		UpdatePlayerList();

		// Travel to the session
		FString ConnectInfo;
		if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectInfo))
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(ConnectInfo, TRAVEL_Absolute);
			}
		}
	}
	else
	{
		CurrentState = ESessionState::Error;
	}

	OnJoinSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionManager::OnDestroySessionCompleteInternal(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	CurrentState = ESessionState::Idle;
	SessionPlayers.Empty();
	bIsHostPlayer = false;

	OnDestroySessionComplete.Broadcast();
}

void UMultiplayerSessionManager::OnStartSessionCompleteInternal(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
	}

	OnStartSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionManager::OnSessionUserInviteAcceptedInternal(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful && SessionInterface.IsValid())
	{
		// Join the session from invite
		SessionInterface->JoinSession(ControllerId, NAME_GameSession, InviteResult);
	}
}

void UMultiplayerSessionManager::SetupSessionSettings(FOnlineSessionSettings& SessionSettings, const FSessionSettings& Settings)
{
	SessionSettings.bIsLANMatch = Settings.bIsLAN;
	SessionSettings.NumPublicConnections = Settings.MaxPlayers;
	SessionSettings.NumPrivateConnections = 0;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowInvites = Settings.bAllowInvites;
	SessionSettings.bUsesPresence = Settings.bUsesPresence;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings.bUseLobbiesIfAvailable = true;

	// Set custom settings
	SessionSettings.Set(FName("RoomName"), Settings.RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("HostName"), FString("Host"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("MapName"), Settings.MapName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("GameMode"), Settings.GameMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (Settings.bIsPasswordProtected)
	{
		SessionSettings.Set(FName("Password"), Settings.Password, EOnlineDataAdvertisementType::ViaOnlineService);
	}
}

void UMultiplayerSessionManager::UpdatePlayerList()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	SessionPlayers.Empty();

	FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
	if (Session)
	{
		// Add host player
		FSessionPlayerInfo HostInfo;
		HostInfo.PlayerName = FString("Host");
		HostInfo.bIsHost = true;
		HostInfo.Ping = 0;
		SessionPlayers.Add(HostInfo);

		// In a real implementation, this would get actual player data from the session
		// For now, we'll create placeholder data based on connected players
		int32 ConnectedPlayers = Session->SessionSettings.NumPublicConnections - Session->NumOpenPublicConnections;
		for (int32 i = 1; i < ConnectedPlayers; i++)
		{
			FSessionPlayerInfo PlayerInfo;
			PlayerInfo.PlayerName = FString::Printf(TEXT("Player_%d"), i);
			PlayerInfo.bIsHost = false;
			PlayerInfo.Ping = 0;
			SessionPlayers.Add(PlayerInfo);
		}
	}

	SyncSessionData();
}

void UMultiplayerSessionManager::SyncSessionData()
{
	// In a real implementation, this would synchronize session data across all clients
	// This could involve:
	// 1. Replicating player lists
	// 2. Synchronizing game state
	// 3. Updating session settings
	// 4. Broadcasting updates to all connected clients

	// For now, we'll just broadcast player count changes
	if (SessionPlayers.Num() > 0)
	{
		// Notify about player changes
		// This is a simplified version - in production you'd track specific joins/leaves
	}
}
