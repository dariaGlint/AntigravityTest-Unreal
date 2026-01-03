#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "NPCRelationshipComponent.h"
#include "ConversationSystem.generated.h"

class UEventFlagManager;

UENUM(BlueprintType)
enum class EConversationConditionType : uint8
{
	None				UMETA(DisplayName = "なし"),
	MinAffection		UMETA(DisplayName = "最小好感度"),
	MaxAffection		UMETA(DisplayName = "最大好感度"),
	RelationshipLevel	UMETA(DisplayName = "関係レベル"),
	EventFlag			UMETA(DisplayName = "イベントフラグ"),
	CustomTag			UMETA(DisplayName = "カスタムタグ"),
	MinConversationCount UMETA(DisplayName = "最小会話回数")
};

USTRUCT(BlueprintType)
struct FConversationCondition
{
	GENERATED_BODY()

	// 条件タイプ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	EConversationConditionType ConditionType = EConversationConditionType::None;

	// 好感度の値（MinAffection/MaxAffectionの場合）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	float AffectionValue = 0.0f;

	// 関係レベル（RelationshipLevelの場合）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	ERelationshipLevel RequiredRelationshipLevel = ERelationshipLevel::Neutral;

	// イベントフラグ名（EventFlagの場合）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName EventFlagName;

	// カスタムタグ（CustomTagの場合）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FGameplayTag RequiredTag;

	// 会話回数（MinConversationCountの場合）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	int32 ConversationCountValue = 0;

	// 条件を反転するか（NOT条件）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	bool bInvert = false;

	FConversationCondition()
		: ConditionType(EConversationConditionType::None)
		, AffectionValue(0.0f)
		, RequiredRelationshipLevel(ERelationshipLevel::Neutral)
		, EventFlagName(NAME_None)
		, ConversationCountValue(0)
		, bInvert(false)
	{}
};

USTRUCT(BlueprintType)
struct FDialogueOption
{
	GENERATED_BODY()

	// 選択肢のID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName OptionId;

	// 選択肢のテキスト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FText OptionText;

	// この選択肢を表示するための条件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TArray<FConversationCondition> Conditions;

	// この選択肢を選んだ際の好感度変化
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	float AffectionDelta = 0.0f;

	// この選択肢を選んだ際にセットするイベントフラグ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TArray<FName> EventFlagsToSet;

	// 次の会話ノードID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName NextNodeId;

	FDialogueOption()
		: OptionId(NAME_None)
		, AffectionDelta(0.0f)
		, NextNodeId(NAME_None)
	{}
};

USTRUCT(BlueprintType)
struct FConversationNode
{
	GENERATED_BODY()

	// ノードID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName NodeId;

	// NPCのセリフ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FText NPCDialogue;

	// このノードを表示するための条件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TArray<FConversationCondition> Conditions;

	// プレイヤーの選択肢
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TArray<FDialogueOption> Options;

	// このノードに到達した際の好感度変化
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	float AffectionDelta = 0.0f;

	// このノードに到達した際にセットするイベントフラグ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TArray<FName> EventFlagsToSet;

	FConversationNode()
		: NodeId(NAME_None)
		, AffectionDelta(0.0f)
	{}
};

USTRUCT(BlueprintType)
struct FConversationTree
{
	GENERATED_BODY()

	// 会話ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName ConversationId;

	// NPC ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName NPCId;

	// 開始ノードID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	FName StartNodeId;

	// すべての会話ノード
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TArray<FConversationNode> Nodes;

	FConversationTree()
		: ConversationId(NAME_None)
		, NPCId(NAME_None)
		, StartNodeId(NAME_None)
	{}
};

/**
 * 会話システムを管理するクラス
 */
UCLASS(BlueprintType)
class NPCRELATIONSHIPSYSTEM_API UConversationSystem : public UObject
{
	GENERATED_BODY()

public:
	// 条件のチェック
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	bool CheckCondition(const FConversationCondition& Condition, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager) const;

	// すべての条件をチェック（AND条件）
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	bool CheckAllConditions(const TArray<FConversationCondition>& Conditions, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager) const;

	// 会話ノードの取得
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	FConversationNode GetConversationNode(const FConversationTree& ConversationTree, FName NodeId) const;

	// 利用可能な選択肢を取得（条件を満たすもののみ）
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	TArray<FDialogueOption> GetAvailableOptions(const FConversationNode& Node, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager) const;

	// 選択肢を選択した際の処理
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	void ProcessDialogueOption(const FDialogueOption& Option, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager);

	// 会話ノードに入った際の処理
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	void ProcessConversationNode(const FConversationNode& Node, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager);
};
