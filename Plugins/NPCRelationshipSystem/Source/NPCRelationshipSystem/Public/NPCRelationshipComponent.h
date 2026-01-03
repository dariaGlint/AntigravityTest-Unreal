#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NPCRelationshipComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRelationshipChanged, FName, NPCId, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRelationshipLevelChanged, FName, NPCId, int32, NewLevel);

UENUM(BlueprintType)
enum class ERelationshipLevel : uint8
{
	Hostile = 0		UMETA(DisplayName = "敵対"),
	Unfriendly = 1	UMETA(DisplayName = "非友好的"),
	Neutral = 2		UMETA(DisplayName = "中立"),
	Friendly = 3	UMETA(DisplayName = "友好的"),
	Close = 4		UMETA(DisplayName = "親密"),
	Devoted = 5		UMETA(DisplayName = "献身的")
};

USTRUCT(BlueprintType)
struct FNPCRelationshipData
{
	GENERATED_BODY()

	// NPCの識別子
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	FName NPCId;

	// 好感度 (-100 to 100)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	float Affection = 0.0f;

	// 関係レベル
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	ERelationshipLevel RelationshipLevel = ERelationshipLevel::Neutral;

	// NPCとの会話回数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	int32 ConversationCount = 0;

	// NPCに贈ったプレゼント数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	int32 GiftsGiven = 0;

	// カスタムタグ（特殊な状態の管理）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	FGameplayTagContainer CustomTags;

	FNPCRelationshipData()
		: NPCId(NAME_None)
		, Affection(0.0f)
		, RelationshipLevel(ERelationshipLevel::Neutral)
		, ConversationCount(0)
		, GiftsGiven(0)
	{}
};

/**
 * NPC好感度と関係値を管理するコンポーネント
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPCRELATIONSHIPSYSTEM_API UNPCRelationshipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNPCRelationshipComponent();

	// デリゲート
	UPROPERTY(BlueprintAssignable, Category = "Relationship")
	FOnRelationshipChanged OnRelationshipChanged;

	UPROPERTY(BlueprintAssignable, Category = "Relationship")
	FOnRelationshipLevelChanged OnRelationshipLevelChanged;

	// 好感度の追加・変更
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	void ModifyAffection(FName NPCId, float Delta);

	// 好感度の設定
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	void SetAffection(FName NPCId, float NewAffection);

	// 好感度の取得
	UFUNCTION(BlueprintPure, Category = "Relationship")
	float GetAffection(FName NPCId) const;

	// 関係レベルの取得
	UFUNCTION(BlueprintPure, Category = "Relationship")
	ERelationshipLevel GetRelationshipLevel(FName NPCId) const;

	// 会話回数の増加
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	void IncrementConversationCount(FName NPCId);

	// プレゼント数の増加
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	void IncrementGiftsGiven(FName NPCId);

	// カスタムタグの追加
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	void AddCustomTag(FName NPCId, FGameplayTag Tag);

	// カスタムタグの削除
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	void RemoveCustomTag(FName NPCId, FGameplayTag Tag);

	// カスタムタグの確認
	UFUNCTION(BlueprintPure, Category = "Relationship")
	bool HasCustomTag(FName NPCId, FGameplayTag Tag) const;

	// 関係データの取得
	UFUNCTION(BlueprintPure, Category = "Relationship")
	FNPCRelationshipData GetRelationshipData(FName NPCId) const;

	// すべての関係データの取得
	UFUNCTION(BlueprintPure, Category = "Relationship")
	TArray<FNPCRelationshipData> GetAllRelationships() const;

protected:
	virtual void BeginPlay() override;

	// 好感度から関係レベルを計算
	ERelationshipLevel CalculateRelationshipLevel(float Affection) const;

	// NPCデータの取得または作成
	FNPCRelationshipData& GetOrCreateNPCData(FName NPCId);

private:
	// NPC関係データのマップ
	UPROPERTY(SaveGame)
	TMap<FName, FNPCRelationshipData> RelationshipMap;

	// 関係レベルのしきい値
	UPROPERTY(EditAnywhere, Category = "Relationship|Settings")
	float HostileThreshold = -60.0f;

	UPROPERTY(EditAnywhere, Category = "Relationship|Settings")
	float UnfriendlyThreshold = -20.0f;

	UPROPERTY(EditAnywhere, Category = "Relationship|Settings")
	float NeutralThreshold = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Relationship|Settings")
	float FriendlyThreshold = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Relationship|Settings")
	float CloseThreshold = 80.0f;
};
