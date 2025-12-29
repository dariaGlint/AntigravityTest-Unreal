#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "TeamInfo.generated.h"

/**
 * チームメンバー情報を保持する構造体
 */
USTRUCT(BlueprintType)
struct FTeamMemberInfo
{
	GENERATED_BODY()

	/** プレイヤーの一意のID */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	FString PlayerId;

	/** プレイヤー名 */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	FString PlayerName;

	/** 個人スコア */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	int32 IndividualScore;

	/** キル数 */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	int32 Kills;

	/** デス数 */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	int32 Deaths;

	/** アシスト数 */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	int32 Assists;

	FTeamMemberInfo()
		: PlayerId(TEXT(""))
		, PlayerName(TEXT(""))
		, IndividualScore(0)
		, Kills(0)
		, Deaths(0)
		, Assists(0)
	{
	}
};

/**
 * チーム情報を管理するActorクラス
 */
UCLASS(Blueprintable, BlueprintType)
class TEAMMANAGEMENTSYSTEM_API ATeamInfo : public AInfo
{
	GENERATED_BODY()

public:
	ATeamInfo();

	/** チームID (0: チームなし, 1以上: チーム番号) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team", Replicated)
	int32 TeamId;

	/** チーム名 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team", Replicated)
	FString TeamName;

	/** チームカラー */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team", Replicated)
	FLinearColor TeamColor;

	/** チームスコア */
	UPROPERTY(BlueprintReadWrite, Category = "Team", Replicated)
	int32 TeamScore;

	/** チームメンバー情報 */
	UPROPERTY(BlueprintReadWrite, Category = "Team", Replicated)
	TArray<FTeamMemberInfo> TeamMembers;

	/** チームにメンバーを追加 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void AddTeamMember(const FString& PlayerId, const FString& PlayerName);

	/** チームからメンバーを削除 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void RemoveTeamMember(const FString& PlayerId);

	/** チームメンバーのスコアを更新 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void UpdateMemberScore(const FString& PlayerId, int32 ScoreDelta);

	/** チームメンバーのキル/デス/アシストを記録 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void RecordMemberKill(const FString& PlayerId);

	UFUNCTION(BlueprintCallable, Category = "Team")
	void RecordMemberDeath(const FString& PlayerId);

	UFUNCTION(BlueprintCallable, Category = "Team")
	void RecordMemberAssist(const FString& PlayerId);

	/** チームスコアを加算 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void AddTeamScore(int32 ScoreDelta);

	/** チームメンバー数を取得 */
	UFUNCTION(BlueprintPure, Category = "Team")
	int32 GetTeamMemberCount() const { return TeamMembers.Num(); }

	/** 特定のプレイヤーがこのチームに所属しているか確認 */
	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsMember(const FString& PlayerId) const;

	/** チーム情報をリセット */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void ResetTeam();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/** チームメンバー情報を検索 */
	FTeamMemberInfo* FindMember(const FString& PlayerId);
};
