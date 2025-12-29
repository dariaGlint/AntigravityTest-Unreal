#include "SessionBlueprintLibrary.h"
#include "MultiplayerSessionManager.h"
#include "Engine/GameInstance.h"

UMultiplayerSessionManager* USessionBlueprintLibrary::GetSessionManager(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UMultiplayerSessionManager>();
}

void USessionBlueprintLibrary::QuickCreateSession(const UObject* WorldContextObject, const FString& RoomName, int32 MaxPlayers, bool bIsLAN)
{
	UMultiplayerSessionManager* SessionManager = GetSessionManager(WorldContextObject);
	if (SessionManager)
	{
		FSessionSettings Settings = MakeSessionSettings(RoomName, MaxPlayers, bIsLAN);
		SessionManager->CreateSession(Settings);
	}
}

void USessionBlueprintLibrary::QuickFindSessions(const UObject* WorldContextObject, int32 MaxResults, bool bIsLAN)
{
	UMultiplayerSessionManager* SessionManager = GetSessionManager(WorldContextObject);
	if (SessionManager)
	{
		SessionManager->FindSessions(MaxResults, bIsLAN);
	}
}

FSessionSettings USessionBlueprintLibrary::MakeSessionSettings(
	const FString& RoomName,
	int32 MaxPlayers,
	bool bIsLAN,
	bool bUsesPresence,
	bool bAllowInvites,
	const FString& MapName,
	const FString& GameMode)
{
	FSessionSettings Settings;
	Settings.RoomName = RoomName;
	Settings.MaxPlayers = MaxPlayers;
	Settings.bIsLAN = bIsLAN;
	Settings.bUsesPresence = bUsesPresence;
	Settings.bAllowInvites = bAllowInvites;
	Settings.MapName = MapName;
	Settings.GameMode = GameMode;
	return Settings;
}
