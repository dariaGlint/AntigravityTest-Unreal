// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestDataAsset.generated.h"

class UQuestObjectiveDataAsset;

/**
 * Enum for quest objective types
 */
UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	DefeatEnemies UMETA(DisplayName = "Defeat Enemies"),
	ReachLocation UMETA(DisplayName = "Reach Location"),
	CollectItems UMETA(DisplayName = "Collect Items"),
	InteractWithObject UMETA(DisplayName = "Interact With Object"),
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Enum for quest reward types
 */
UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
	Experience UMETA(DisplayName = "Experience"),
	Currency UMETA(DisplayName = "Currency"),
	Item UMETA(DisplayName = "Item"),
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Struct for quest rewards
 */
USTRUCT(BlueprintType)
struct FQuestReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EQuestRewardType RewardType = EQuestRewardType::Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Amount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Description;
};

/**
 * Data asset for quest objectives
 */
UCLASS(BlueprintType)
class QUESTSYSTEM_API UQuestObjectiveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString ObjectiveID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EQuestObjectiveType ObjectiveType = EQuestObjectiveType::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString TargetID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 RequiredCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bOptional = false;
};

/**
 * Main quest data asset
 */
UCLASS(BlueprintType)
class QUESTSYSTEM_API UQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<TObjectPtr<UQuestObjectiveDataAsset>> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<FQuestReward> Rewards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<FString> Prerequisites;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bRepeatable = false;
};
