#include "NPCRelationshipBlueprintLibrary.h"
#include "Engine/World.h"

UEventFlagManager* UNPCRelationshipBlueprintLibrary::GetEventFlagManager(const UObject* WorldContextObject)
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

	return GameInstance->GetSubsystem<UEventFlagManager>();
}

UNPCRelationshipComponent* UNPCRelationshipBlueprintLibrary::GetRelationshipComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UNPCRelationshipComponent>();
}

UConversationSystem* UNPCRelationshipBlueprintLibrary::CreateConversationSystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	return NewObject<UConversationSystem>();
}

FString UNPCRelationshipBlueprintLibrary::RelationshipLevelToString(ERelationshipLevel Level)
{
	switch (Level)
	{
	case ERelationshipLevel::Hostile:
		return TEXT("Hostile");
	case ERelationshipLevel::Unfriendly:
		return TEXT("Unfriendly");
	case ERelationshipLevel::Neutral:
		return TEXT("Neutral");
	case ERelationshipLevel::Friendly:
		return TEXT("Friendly");
	case ERelationshipLevel::Close:
		return TEXT("Close");
	case ERelationshipLevel::Devoted:
		return TEXT("Devoted");
	default:
		return TEXT("Unknown");
	}
}

FText UNPCRelationshipBlueprintLibrary::RelationshipLevelToText(ERelationshipLevel Level)
{
	switch (Level)
	{
	case ERelationshipLevel::Hostile:
		return FText::FromString(TEXT("敵対"));
	case ERelationshipLevel::Unfriendly:
		return FText::FromString(TEXT("非友好的"));
	case ERelationshipLevel::Neutral:
		return FText::FromString(TEXT("中立"));
	case ERelationshipLevel::Friendly:
		return FText::FromString(TEXT("友好的"));
	case ERelationshipLevel::Close:
		return FText::FromString(TEXT("親密"));
	case ERelationshipLevel::Devoted:
		return FText::FromString(TEXT("献身的"));
	default:
		return FText::FromString(TEXT("不明"));
	}
}
