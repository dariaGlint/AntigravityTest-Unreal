#include "TeamManagementComponent.h"
#include "TeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UTeamManagementComponent::UTeamManagementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TeamId = 0;
	TeamInfoRef = nullptr;
	bShowTeamColor = true;
	bFriendlyFireEnabled = false;

	SetIsReplicatedByDefault(true);
}

void UTeamManagementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTeamManagementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTeamManagementComponent, TeamId);
	DOREPLIFETIME(UTeamManagementComponent, TeamInfoRef);
}

void UTeamManagementComponent::ServerJoinTeam_Implementation(int32 NewTeamId, ATeamInfo* TeamInfo)
{
	// 既存のチームから脱退
	if (TeamInfoRef && TeamInfoRef->TeamId != NewTeamId)
	{
		TeamInfoRef->RemoveTeamMember(PlayerId);
	}

	// 新しいチームに参加
	TeamId = NewTeamId;
	TeamInfoRef = TeamInfo;

	if (TeamInfoRef)
	{
		TeamInfoRef->AddTeamMember(PlayerId, PlayerName);
	}
}

void UTeamManagementComponent::ServerLeaveTeam_Implementation()
{
	if (TeamInfoRef)
	{
		TeamInfoRef->RemoveTeamMember(PlayerId);
		TeamInfoRef = nullptr;
	}

	TeamId = 0;
}

FLinearColor UTeamManagementComponent::GetTeamColor() const
{
	if (TeamInfoRef)
	{
		return TeamInfoRef->TeamColor;
	}

	return FLinearColor::White;
}

bool UTeamManagementComponent::IsFriendly(AActor* OtherActor) const
{
	if (!OtherActor || TeamId == 0)
	{
		return false;
	}

	UTeamManagementComponent* OtherTeamComp = OtherActor->FindComponentByClass<UTeamManagementComponent>();
	if (OtherTeamComp)
	{
		return TeamId == OtherTeamComp->TeamId;
	}

	return false;
}

bool UTeamManagementComponent::IsEnemy(AActor* OtherActor) const
{
	if (!OtherActor || TeamId == 0)
	{
		return false;
	}

	UTeamManagementComponent* OtherTeamComp = OtherActor->FindComponentByClass<UTeamManagementComponent>();
	if (OtherTeamComp)
	{
		return TeamId != OtherTeamComp->TeamId && OtherTeamComp->TeamId != 0;
	}

	return false;
}

bool UTeamManagementComponent::CanDamage(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	// フレンドリーファイアが無効で、味方の場合はダメージを与えられない
	if (!bFriendlyFireEnabled && IsFriendly(Target))
	{
		return false;
	}

	return true;
}
