#pragma once

#include "CoreMinimal.h"
#include "SessionData.generated.h"

UENUM(BlueprintType)
enum class ESessionState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Creating UMETA(DisplayName = "Creating"),
	Searching UMETA(DisplayName = "Searching"),
	Joining UMETA(DisplayName = "Joining"),
	InSession UMETA(DisplayName = "In Session"),
	Leaving UMETA(DisplayName = "Leaving"),
	Error UMETA(DisplayName = "Error")
};

USTRUCT(BlueprintType)
struct FSessionPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FUniqueNetIdRepl UniqueId;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bIsHost;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 Ping;

	FSessionPlayerInfo()
		: PlayerName(TEXT(""))
		, bIsHost(false)
		, Ping(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FSessionRoomInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString RoomName;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString HostName;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 Ping;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bIsLAN;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bIsPasswordProtected;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString MapName;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString GameMode;

	FSessionRoomInfo()
		: RoomName(TEXT(""))
		, HostName(TEXT(""))
		, CurrentPlayers(0)
		, MaxPlayers(4)
		, Ping(0)
		, bIsLAN(false)
		, bIsPasswordProtected(false)
		, MapName(TEXT(""))
		, GameMode(TEXT(""))
	{
	}
};

USTRUCT(BlueprintType)
struct FSessionSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString RoomName;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bIsLAN;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bUsesPresence;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bAllowInvites;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bIsPasswordProtected;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString Password;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString MapName;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString GameMode;

	FSessionSettings()
		: RoomName(TEXT("DefaultRoom"))
		, MaxPlayers(4)
		, bIsLAN(false)
		, bUsesPresence(true)
		, bAllowInvites(true)
		, bIsPasswordProtected(false)
		, Password(TEXT(""))
		, MapName(TEXT(""))
		, GameMode(TEXT(""))
	{
	}
};
