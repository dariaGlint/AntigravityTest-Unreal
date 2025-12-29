#include "TeamInfo.h"
#include "Net/UnrealNetwork.h"

ATeamInfo::ATeamInfo()
{
	TeamId = 0;
	TeamName = TEXT("Team");
	TeamColor = FLinearColor::White;
	TeamScore = 0;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ATeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamInfo, TeamId);
	DOREPLIFETIME(ATeamInfo, TeamName);
	DOREPLIFETIME(ATeamInfo, TeamColor);
	DOREPLIFETIME(ATeamInfo, TeamScore);
	DOREPLIFETIME(ATeamInfo, TeamMembers);
}

void ATeamInfo::AddTeamMember(const FString& PlayerId, const FString& PlayerName)
{
	if (!IsMember(PlayerId))
	{
		FTeamMemberInfo NewMember;
		NewMember.PlayerId = PlayerId;
		NewMember.PlayerName = PlayerName;
		TeamMembers.Add(NewMember);
	}
}

void ATeamInfo::RemoveTeamMember(const FString& PlayerId)
{
	TeamMembers.RemoveAll([&PlayerId](const FTeamMemberInfo& Member)
	{
		return Member.PlayerId == PlayerId;
	});
}

void ATeamInfo::UpdateMemberScore(const FString& PlayerId, int32 ScoreDelta)
{
	if (FTeamMemberInfo* Member = FindMember(PlayerId))
	{
		Member->IndividualScore += ScoreDelta;
		AddTeamScore(ScoreDelta);
	}
}

void ATeamInfo::RecordMemberKill(const FString& PlayerId)
{
	if (FTeamMemberInfo* Member = FindMember(PlayerId))
	{
		Member->Kills++;
	}
}

void ATeamInfo::RecordMemberDeath(const FString& PlayerId)
{
	if (FTeamMemberInfo* Member = FindMember(PlayerId))
	{
		Member->Deaths++;
	}
}

void ATeamInfo::RecordMemberAssist(const FString& PlayerId)
{
	if (FTeamMemberInfo* Member = FindMember(PlayerId))
	{
		Member->Assists++;
	}
}

void ATeamInfo::AddTeamScore(int32 ScoreDelta)
{
	TeamScore += ScoreDelta;
}

bool ATeamInfo::IsMember(const FString& PlayerId) const
{
	return TeamMembers.ContainsByPredicate([&PlayerId](const FTeamMemberInfo& Member)
	{
		return Member.PlayerId == PlayerId;
	});
}

void ATeamInfo::ResetTeam()
{
	TeamScore = 0;
	TeamMembers.Empty();
}

FTeamMemberInfo* ATeamInfo::FindMember(const FString& PlayerId)
{
	return TeamMembers.FindByPredicate([&PlayerId](const FTeamMemberInfo& Member)
	{
		return Member.PlayerId == PlayerId;
	});
}
