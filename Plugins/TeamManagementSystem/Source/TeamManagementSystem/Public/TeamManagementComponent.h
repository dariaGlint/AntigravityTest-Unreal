#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamManagementComponent.generated.h"

class ATeamInfo;

/**
 * チーム管理を行うコンポーネント
 * PlayerStateやCharacterにアタッチして使用します
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAMMANAGEMENTSYSTEM_API UTeamManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTeamManagementComponent();

	/** このプレイヤーが所属しているチームID */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team", Replicated)
	int32 TeamId;

	/** チーム情報への参照 */
	UPROPERTY(BlueprintReadOnly, Category = "Team", Replicated)
	ATeamInfo* TeamInfoRef;

	/** プレイヤーID */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	FString PlayerId;

	/** プレイヤー名 */
	UPROPERTY(BlueprintReadWrite, Category = "Team")
	FString PlayerName;

	/** チームカラーを表示するか */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	bool bShowTeamColor;

	/** チームに参加 */
	UFUNCTION(BlueprintCallable, Category = "Team", Server, Reliable)
	void ServerJoinTeam(int32 NewTeamId, ATeamInfo* TeamInfo);

	/** チームから脱退 */
	UFUNCTION(BlueprintCallable, Category = "Team", Server, Reliable)
	void ServerLeaveTeam();

	/** 現在のチームIDを取得 */
	UFUNCTION(BlueprintPure, Category = "Team")
	int32 GetTeamId() const { return TeamId; }

	/** チームカラーを取得 */
	UFUNCTION(BlueprintPure, Category = "Team")
	FLinearColor GetTeamColor() const;

	/** 他のアクターが味方かどうかチェック */
	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsFriendly(AActor* OtherActor) const;

	/** 他のアクターが敵かどうかチェック */
	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsEnemy(AActor* OtherActor) const;

	/** フレンドリーファイアが有効かどうか */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	bool bFriendlyFireEnabled;

	/** ダメージ判定前のチェック（フレンドリーファイア制御用） */
	UFUNCTION(BlueprintCallable, Category = "Team")
	bool CanDamage(AActor* Target) const;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void ServerJoinTeam_Implementation(int32 NewTeamId, ATeamInfo* TeamInfo);
	void ServerLeaveTeam_Implementation();
};
