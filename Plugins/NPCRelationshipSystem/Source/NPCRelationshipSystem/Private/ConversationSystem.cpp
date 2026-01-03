#include "ConversationSystem.h"
#include "EventFlagManager.h"

bool UConversationSystem::CheckCondition(const FConversationCondition& Condition, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager) const
{
	if (!RelationshipComponent)
	{
		return false;
	}

	bool Result = false;

	switch (Condition.ConditionType)
	{
	case EConversationConditionType::None:
		Result = true;
		break;

	case EConversationConditionType::MinAffection:
		Result = RelationshipComponent->GetAffection(NPCId) >= Condition.AffectionValue;
		break;

	case EConversationConditionType::MaxAffection:
		Result = RelationshipComponent->GetAffection(NPCId) <= Condition.AffectionValue;
		break;

	case EConversationConditionType::RelationshipLevel:
		Result = RelationshipComponent->GetRelationshipLevel(NPCId) == Condition.RequiredRelationshipLevel;
		break;

	case EConversationConditionType::EventFlag:
		if (EventFlagManager && !Condition.EventFlagName.IsNone())
		{
			Result = EventFlagManager->GetEventFlag(Condition.EventFlagName);
		}
		break;

	case EConversationConditionType::CustomTag:
		if (Condition.RequiredTag.IsValid())
		{
			Result = RelationshipComponent->HasCustomTag(NPCId, Condition.RequiredTag);
		}
		break;

	case EConversationConditionType::MinConversationCount:
		{
			FNPCRelationshipData Data = RelationshipComponent->GetRelationshipData(NPCId);
			Result = Data.ConversationCount >= Condition.ConversationCountValue;
		}
		break;

	default:
		Result = false;
		break;
	}

	// 条件を反転する場合
	if (Condition.bInvert)
	{
		Result = !Result;
	}

	return Result;
}

bool UConversationSystem::CheckAllConditions(const TArray<FConversationCondition>& Conditions, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager) const
{
	// 条件がない場合はtrue
	if (Conditions.Num() == 0)
	{
		return true;
	}

	// すべての条件をチェック（AND条件）
	for (const FConversationCondition& Condition : Conditions)
	{
		if (!CheckCondition(Condition, RelationshipComponent, NPCId, EventFlagManager))
		{
			return false;
		}
	}

	return true;
}

FConversationNode UConversationSystem::GetConversationNode(const FConversationTree& ConversationTree, FName NodeId) const
{
	for (const FConversationNode& Node : ConversationTree.Nodes)
	{
		if (Node.NodeId == NodeId)
		{
			return Node;
		}
	}

	return FConversationNode();
}

TArray<FDialogueOption> UConversationSystem::GetAvailableOptions(const FConversationNode& Node, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager) const
{
	TArray<FDialogueOption> AvailableOptions;

	for (const FDialogueOption& Option : Node.Options)
	{
		if (CheckAllConditions(Option.Conditions, RelationshipComponent, NPCId, EventFlagManager))
		{
			AvailableOptions.Add(Option);
		}
	}

	return AvailableOptions;
}

void UConversationSystem::ProcessDialogueOption(const FDialogueOption& Option, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager)
{
	if (!RelationshipComponent)
	{
		return;
	}

	// 好感度を変更
	if (Option.AffectionDelta != 0.0f)
	{
		RelationshipComponent->ModifyAffection(NPCId, Option.AffectionDelta);
	}

	// イベントフラグをセット
	if (EventFlagManager)
	{
		for (const FName& FlagName : Option.EventFlagsToSet)
		{
			if (!FlagName.IsNone())
			{
				EventFlagManager->SetEventFlag(FlagName, true);
			}
		}
	}
}

void UConversationSystem::ProcessConversationNode(const FConversationNode& Node, UNPCRelationshipComponent* RelationshipComponent, FName NPCId, UEventFlagManager* EventFlagManager)
{
	if (!RelationshipComponent)
	{
		return;
	}

	// 好感度を変更
	if (Node.AffectionDelta != 0.0f)
	{
		RelationshipComponent->ModifyAffection(NPCId, Node.AffectionDelta);
	}

	// イベントフラグをセット
	if (EventFlagManager)
	{
		for (const FName& FlagName : Node.EventFlagsToSet)
		{
			if (!FlagName.IsNone())
			{
				EventFlagManager->SetEventFlag(FlagName, true);
			}
		}
	}

	// 会話回数を増加
	RelationshipComponent->IncrementConversationCount(NPCId);
}
