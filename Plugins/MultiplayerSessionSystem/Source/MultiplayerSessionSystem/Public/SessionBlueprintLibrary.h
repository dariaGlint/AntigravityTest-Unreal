#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SessionData.h"
#include "SessionBlueprintLibrary.generated.h"

class UMultiplayerSessionManager;

UCLASS()
class MULTIPLAYERSESSIONSYSTEM_API USessionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Multiplayer Session", meta = (WorldContext = "WorldContextObject"))
	static UMultiplayerSessionManager* GetSessionManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session", meta = (WorldContext = "WorldContextObject"))
	static void QuickCreateSession(const UObject* WorldContextObject, const FString& RoomName, int32 MaxPlayers = 4, bool bIsLAN = false);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Session", meta = (WorldContext = "WorldContextObject"))
	static void QuickFindSessions(const UObject* WorldContextObject, int32 MaxResults = 10, bool bIsLAN = false);

	UFUNCTION(BlueprintPure, Category = "Multiplayer Session")
	static FSessionSettings MakeSessionSettings(
		const FString& RoomName,
		int32 MaxPlayers = 4,
		bool bIsLAN = false,
		bool bUsesPresence = true,
		bool bAllowInvites = true,
		const FString& MapName = TEXT(""),
		const FString& GameMode = TEXT("")
	);
};
