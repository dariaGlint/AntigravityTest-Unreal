#include "TeamManager.h"
#include "TeamInfo.h"
#include "TeamManagementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

ATeamManager::ATeamManager()
{
	MaxTeams = 4;
	MaxPlayersPerTeam = 0;
	AssignmentMode = ETeamAssignmentMode::BalanceByCount;
	RoundRobinIndex = 0;

	bReplicates = true;
	bAlwaysRelevant = true;

	// デフォルトのチームカラー
	DefaultTeamColors.Add(FLinearColor::Red);      // チーム1: 赤
	DefaultTeamColors.Add(FLinearColor::Blue);     // チーム2: 青
	DefaultTeamColors.Add(FLinearColor::Green);    // チーム3: 緑
	DefaultTeamColors.Add(FLinearColor::Yellow);   // チーム4: 黄
}

void ATeamManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATeamManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamManager, Teams);
}

ATeamInfo* ATeamManager::CreateTeam(int32 TeamId, const FString& TeamName, FLinearColor TeamColor)
{
	// 既存のチームをチェック
	if (GetTeamById(TeamId))
	{
		return nullptr;
	}

	// 新しいチームを生成
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	ATeamInfo* NewTeam = GetWorld()->SpawnActor<ATeamInfo>(ATeamInfo::StaticClass(), SpawnParams);

	if (NewTeam)
	{
		NewTeam->TeamId = TeamId;
		NewTeam->TeamName = TeamName;
		NewTeam->TeamColor = TeamColor;
		Teams.Add(NewTeam);
	}

	return NewTeam;
}

void ATeamManager::RemoveTeam(int32 TeamId)
{
	ATeamInfo* Team = GetTeamById(TeamId);
	if (Team)
	{
		Teams.Remove(Team);
		Team->Destroy();
	}
}

ATeamInfo* ATeamManager::GetTeamById(int32 TeamId) const
{
	for (ATeamInfo* Team : Teams)
	{
		if (Team && Team->TeamId == TeamId)
		{
			return Team;
		}
	}

	return nullptr;
}

ATeamInfo* ATeamManager::AssignPlayerToTeam(AActor* Player, int32 TeamId)
{
	if (!Player)
	{
		return nullptr;
	}

	ATeamInfo* Team = GetTeamById(TeamId);
	if (!Team)
	{
		return nullptr;
	}

	// チームの人数制限チェック
	if (MaxPlayersPerTeam > 0 && Team->GetTeamMemberCount() >= MaxPlayersPerTeam)
	{
		return nullptr;
	}

	// プレイヤーのTeamManagementComponentを取得
	UTeamManagementComponent* TeamComp = Player->FindComponentByClass<UTeamManagementComponent>();
	if (TeamComp)
	{
		TeamComp->ServerJoinTeam(TeamId, Team);
	}

	return Team;
}

ATeamInfo* ATeamManager::AutoAssignPlayerToTeam(AActor* Player)
{
	if (!Player || Teams.Num() == 0)
	{
		return nullptr;
	}

	int32 AssignedTeamId = 0;

	switch (AssignmentMode)
	{
	case ETeamAssignmentMode::Manual:
		// マニュアルモードでは何もしない
		return nullptr;

	case ETeamAssignmentMode::RoundRobin:
		AssignedTeamId = GetNextRoundRobinTeamId();
		break;

	case ETeamAssignmentMode::BalanceByCount:
		AssignedTeamId = GetBalancedTeamIdByCount();
		break;

	case ETeamAssignmentMode::BalanceByScore:
		AssignedTeamId = GetBalancedTeamIdByScore();
		break;
	}

	return AssignPlayerToTeam(Player, AssignedTeamId);
}

void ATeamManager::ResetAllTeams()
{
	for (ATeamInfo* Team : Teams)
	{
		if (Team)
		{
			Team->ResetTeam();
		}
	}
}

TArray<ATeamInfo*> ATeamManager::GetTeamsSortedByScore() const
{
	TArray<ATeamInfo*> SortedTeams = Teams;

	SortedTeams.Sort([](const ATeamInfo& A, const ATeamInfo& B)
	{
		return A.TeamScore > B.TeamScore;
	});

	return SortedTeams;
}

void ATeamManager::InitializeDefaultTeams(int32 TeamCount)
{
	// 既存のチームをクリア
	for (ATeamInfo* Team : Teams)
	{
		if (Team)
		{
			Team->Destroy();
		}
	}
	Teams.Empty();

	// 指定された数のチームを作成
	TeamCount = FMath::Min(TeamCount, MaxTeams);

	for (int32 i = 0; i < TeamCount; ++i)
	{
		int32 TeamId = i + 1;
		FString TeamName = FString::Printf(TEXT("Team %d"), TeamId);

		// デフォルトカラーを使用、足りない場合はランダム
		FLinearColor TeamColor;
		if (i < DefaultTeamColors.Num())
		{
			TeamColor = DefaultTeamColors[i];
		}
		else
		{
			TeamColor = FLinearColor::MakeRandomColor();
		}

		CreateTeam(TeamId, TeamName, TeamColor);
	}
}

int32 ATeamManager::GetNextRoundRobinTeamId()
{
	if (Teams.Num() == 0)
	{
		return 0;
	}

	RoundRobinIndex = (RoundRobinIndex + 1) % Teams.Num();
	return Teams[RoundRobinIndex]->TeamId;
}

int32 ATeamManager::GetBalancedTeamIdByCount() const
{
	if (Teams.Num() == 0)
	{
		return 0;
	}

	// 最も人数が少ないチームを見つける
	ATeamInfo* SmallestTeam = Teams[0];
	for (ATeamInfo* Team : Teams)
	{
		if (Team && Team->GetTeamMemberCount() < SmallestTeam->GetTeamMemberCount())
		{
			SmallestTeam = Team;
		}
	}

	return SmallestTeam ? SmallestTeam->TeamId : 0;
}

int32 ATeamManager::GetBalancedTeamIdByScore() const
{
	if (Teams.Num() == 0)
	{
		return 0;
	}

	// 最もスコアが低いチームを見つける
	ATeamInfo* LowestScoreTeam = Teams[0];
	for (ATeamInfo* Team : Teams)
	{
		if (Team && Team->TeamScore < LowestScoreTeam->TeamScore)
		{
			LowestScoreTeam = Team;
		}
	}

	return LowestScoreTeam ? LowestScoreTeam->TeamId : 0;
}
