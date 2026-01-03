#include "NPCRelationshipComponent.h"

UNPCRelationshipComponent::UNPCRelationshipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNPCRelationshipComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNPCRelationshipComponent::ModifyAffection(FName NPCId, float Delta)
{
	if (NPCId.IsNone())
	{
		return;
	}

	FNPCRelationshipData& Data = GetOrCreateNPCData(NPCId);
	float OldAffection = Data.Affection;
	ERelationshipLevel OldLevel = Data.RelationshipLevel;

	// 好感度を変更（-100 to 100の範囲に制限）
	Data.Affection = FMath::Clamp(Data.Affection + Delta, -100.0f, 100.0f);

	// 関係レベルを更新
	Data.RelationshipLevel = CalculateRelationshipLevel(Data.Affection);

	// デリゲートを呼び出し
	OnRelationshipChanged.Broadcast(NPCId, OldAffection, Data.Affection);

	if (OldLevel != Data.RelationshipLevel)
	{
		OnRelationshipLevelChanged.Broadcast(NPCId, static_cast<int32>(Data.RelationshipLevel));
	}
}

void UNPCRelationshipComponent::SetAffection(FName NPCId, float NewAffection)
{
	if (NPCId.IsNone())
	{
		return;
	}

	FNPCRelationshipData& Data = GetOrCreateNPCData(NPCId);
	float OldAffection = Data.Affection;
	ERelationshipLevel OldLevel = Data.RelationshipLevel;

	// 好感度を設定（-100 to 100の範囲に制限）
	Data.Affection = FMath::Clamp(NewAffection, -100.0f, 100.0f);

	// 関係レベルを更新
	Data.RelationshipLevel = CalculateRelationshipLevel(Data.Affection);

	// デリゲートを呼び出し
	OnRelationshipChanged.Broadcast(NPCId, OldAffection, Data.Affection);

	if (OldLevel != Data.RelationshipLevel)
	{
		OnRelationshipLevelChanged.Broadcast(NPCId, static_cast<int32>(Data.RelationshipLevel));
	}
}

float UNPCRelationshipComponent::GetAffection(FName NPCId) const
{
	if (const FNPCRelationshipData* Data = RelationshipMap.Find(NPCId))
	{
		return Data->Affection;
	}
	return 0.0f;
}

ERelationshipLevel UNPCRelationshipComponent::GetRelationshipLevel(FName NPCId) const
{
	if (const FNPCRelationshipData* Data = RelationshipMap.Find(NPCId))
	{
		return Data->RelationshipLevel;
	}
	return ERelationshipLevel::Neutral;
}

void UNPCRelationshipComponent::IncrementConversationCount(FName NPCId)
{
	if (NPCId.IsNone())
	{
		return;
	}

	FNPCRelationshipData& Data = GetOrCreateNPCData(NPCId);
	Data.ConversationCount++;
}

void UNPCRelationshipComponent::IncrementGiftsGiven(FName NPCId)
{
	if (NPCId.IsNone())
	{
		return;
	}

	FNPCRelationshipData& Data = GetOrCreateNPCData(NPCId);
	Data.GiftsGiven++;
}

void UNPCRelationshipComponent::AddCustomTag(FName NPCId, FGameplayTag Tag)
{
	if (NPCId.IsNone() || !Tag.IsValid())
	{
		return;
	}

	FNPCRelationshipData& Data = GetOrCreateNPCData(NPCId);
	Data.CustomTags.AddTag(Tag);
}

void UNPCRelationshipComponent::RemoveCustomTag(FName NPCId, FGameplayTag Tag)
{
	if (NPCId.IsNone() || !Tag.IsValid())
	{
		return;
	}

	if (FNPCRelationshipData* Data = RelationshipMap.Find(NPCId))
	{
		Data->CustomTags.RemoveTag(Tag);
	}
}

bool UNPCRelationshipComponent::HasCustomTag(FName NPCId, FGameplayTag Tag) const
{
	if (const FNPCRelationshipData* Data = RelationshipMap.Find(NPCId))
	{
		return Data->CustomTags.HasTag(Tag);
	}
	return false;
}

FNPCRelationshipData UNPCRelationshipComponent::GetRelationshipData(FName NPCId) const
{
	if (const FNPCRelationshipData* Data = RelationshipMap.Find(NPCId))
	{
		return *Data;
	}
	return FNPCRelationshipData();
}

TArray<FNPCRelationshipData> UNPCRelationshipComponent::GetAllRelationships() const
{
	TArray<FNPCRelationshipData> Result;
	RelationshipMap.GenerateValueArray(Result);
	return Result;
}

ERelationshipLevel UNPCRelationshipComponent::CalculateRelationshipLevel(float Affection) const
{
	if (Affection >= CloseThreshold)
	{
		return ERelationshipLevel::Devoted;
	}
	else if (Affection >= FriendlyThreshold)
	{
		return ERelationshipLevel::Close;
	}
	else if (Affection >= NeutralThreshold)
	{
		return ERelationshipLevel::Friendly;
	}
	else if (Affection >= UnfriendlyThreshold)
	{
		return ERelationshipLevel::Neutral;
	}
	else if (Affection >= HostileThreshold)
	{
		return ERelationshipLevel::Unfriendly;
	}
	else
	{
		return ERelationshipLevel::Hostile;
	}
}

FNPCRelationshipData& UNPCRelationshipComponent::GetOrCreateNPCData(FName NPCId)
{
	if (!RelationshipMap.Contains(NPCId))
	{
		FNPCRelationshipData NewData;
		NewData.NPCId = NPCId;
		RelationshipMap.Add(NPCId, NewData);
	}
	return RelationshipMap[NPCId];
}
