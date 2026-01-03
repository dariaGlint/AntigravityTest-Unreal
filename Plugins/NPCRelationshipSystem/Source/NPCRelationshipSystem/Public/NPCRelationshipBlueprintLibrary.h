#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NPCRelationshipComponent.h"
#include "ConversationSystem.h"
#include "EventFlagManager.h"
#include "NPCRelationshipBlueprintLibrary.generated.h"

/**
 * NPC関係システムのためのBlueprintファンクションライブラリ
 */
UCLASS()
class NPCRELATIONSHIPSYSTEM_API UNPCRelationshipBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// EventFlagManagerの取得
	UFUNCTION(BlueprintPure, Category = "NPC Relationship", meta = (WorldContext = "WorldContextObject"))
	static UEventFlagManager* GetEventFlagManager(const UObject* WorldContextObject);

	// NPCRelationshipComponentの取得
	UFUNCTION(BlueprintPure, Category = "NPC Relationship")
	static UNPCRelationshipComponent* GetRelationshipComponent(AActor* Actor);

	// ConversationSystemの作成
	UFUNCTION(BlueprintCallable, Category = "NPC Relationship", meta = (WorldContext = "WorldContextObject"))
	static UConversationSystem* CreateConversationSystem(const UObject* WorldContextObject);

	// 関係レベルを文字列に変換
	UFUNCTION(BlueprintPure, Category = "NPC Relationship")
	static FString RelationshipLevelToString(ERelationshipLevel Level);

	// 関係レベルをローカライズされたテキストに変換
	UFUNCTION(BlueprintPure, Category = "NPC Relationship")
	static FText RelationshipLevelToText(ERelationshipLevel Level);
};
