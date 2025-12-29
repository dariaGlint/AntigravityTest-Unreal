#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "TeamManager.generated.h"

class ATeamInfo;

/**
 * チームの自動割り当てモード
 */
UENUM(BlueprintType)
enum class ETeamAssignmentMode : uint8
{
	Manual UMETA(DisplayName = "Manual"),
	RoundRobin UMETA(DisplayName = "Round Robin"),
	BalanceByCount UMETA(DisplayName = "Balance By Count"),
	BalanceByScore UMETA(DisplayName = "Balance By Score")
};

/**
 * ゲーム全体のチーム管理を行うマネージャークラス
 * GameModeやGameStateにて1つだけ生成して使用します
 */
UCLASS(Blueprintable, BlueprintType)
class TEAMMANAGEMENTSYSTEM_API ATeamManager : public AInfo
{
	GENERATED_BODY()

public:
	ATeamManager();

	/** 管理しているチーム情報のリスト */
	UPROPERTY(BlueprintReadWrite, Category = "Team", Replicated)
	TArray<ATeamInfo*> Teams;

	/** チームの最大数 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	int32 MaxTeams;

	/** 1チームあたりの最大人数 (0で無制限) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	int32 MaxPlayersPerTeam;

	/** チーム自動割り当てモード */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	ETeamAssignmentMode AssignmentMode;

	/** デフォルトのチームカラーリスト */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	TArray<FLinearColor> DefaultTeamColors;

	/** チームを作成 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	ATeamInfo* CreateTeam(int32 TeamId, const FString& TeamName, FLinearColor TeamColor);

	/** チームを削除 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void RemoveTeam(int32 TeamId);

	/** TeamIDからチーム情報を取得 */
	UFUNCTION(BlueprintPure, Category = "Team")
	ATeamInfo* GetTeamById(int32 TeamId) const;

	/** プレイヤーをチームに割り当て */
	UFUNCTION(BlueprintCallable, Category = "Team")
	ATeamInfo* AssignPlayerToTeam(AActor* Player, int32 TeamId);

	/** プレイヤーを自動的にチームに割り当て */
	UFUNCTION(BlueprintCallable, Category = "Team")
	ATeamInfo* AutoAssignPlayerToTeam(AActor* Player);

	/** 全チームをリセット */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void ResetAllTeams();

	/** チームスコアでソートされたチームリストを取得 */
	UFUNCTION(BlueprintPure, Category = "Team")
	TArray<ATeamInfo*> GetTeamsSortedByScore() const;

	/** デフォルトチームの初期化 */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void InitializeDefaultTeams(int32 TeamCount);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/** ラウンドロビン方式で次のチームIDを取得 */
	int32 GetNextRoundRobinTeamId();

	/** プレイヤー数でバランスを取ったチームIDを取得 */
	int32 GetBalancedTeamIdByCount() const;

	/** スコアでバランスを取ったチームIDを取得 */
	int32 GetBalancedTeamIdByScore() const;

	/** 次のラウンドロビンインデックス */
	int32 RoundRobinIndex;
};
