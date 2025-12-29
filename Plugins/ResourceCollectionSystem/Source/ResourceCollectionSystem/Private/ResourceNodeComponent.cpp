#include "ResourceNodeComponent.h"
#include "ResourceDropTable.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

UResourceNodeComponent::UResourceNodeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceNodeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize state
	bIsAvailable = true;
	CurrentHarvestCount = 0;
}

bool UResourceNodeComponent::TryHarvest(AActor* Harvester, float SkillModifier, TArray<TPair<UItemDefinition*, int32>>& OutDrops)
{
	OutDrops.Empty();

	// Check if node is available
	if (!bIsAvailable)
	{
		return false;
	}

	// Check if harvester meets requirements
	if (!CanHarvest(Harvester))
	{
		return false;
	}

	// Check if we have a drop table
	if (!DropTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResourceNodeComponent: No drop table assigned to %s"), *GetOwner()->GetName());
		return false;
	}

	// Roll for drops
	DropTable->RollDrops(OutDrops, SkillModifier);

	// Increment harvest count
	CurrentHarvestCount++;

	// Broadcast harvest event
	OnHarvested.Broadcast(Harvester);

	// Play feedback
	if (HarvestParticleEffect && GetOwner())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HarvestParticleEffect, GetOwner()->GetActorLocation());
	}

	if (HarvestSound && GetOwner())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HarvestSound, GetOwner()->GetActorLocation());
	}

	// Check if node is depleted
	if (MaxHarvestCount > 0 && CurrentHarvestCount >= MaxHarvestCount)
	{
		bIsAvailable = false;
		OnNodeDepleted();

		// Start respawn timer if respawn is enabled
		if (RespawnTime > 0.0f)
		{
			StartRespawnTimer();
		}
	}

	return true;
}

void UResourceNodeComponent::ForceRespawn()
{
	// Clear any existing respawn timer
	if (GetWorld() && RespawnTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	}

	HandleRespawn();
}

bool UResourceNodeComponent::CanHarvest(AActor* Harvester) const
{
	if (!bIsAvailable || !Harvester)
	{
		return false;
	}

	// If no tags are required, anyone can harvest
	if (RequiredHarvesterTags.IsEmpty())
	{
		return true;
	}

	// Check if harvester has the required tags
	// This is a simple implementation - you may want to implement IGameplayTagAssetInterface
	// on your character class for a more robust solution
	return true;
}

void UResourceNodeComponent::OnNodeDepleted_Implementation()
{
	OnDepleted.Broadcast();
}

void UResourceNodeComponent::OnNodeRespawned_Implementation()
{
	OnRespawned.Broadcast();
}

void UResourceNodeComponent::StartRespawnTimer()
{
	if (!GetWorld())
	{
		return;
	}

	// Calculate respawn time with variance
	float ActualRespawnTime = RespawnTime;
	if (RespawnTimeVariance > 0.0f)
	{
		ActualRespawnTime += FMath::RandRange(-RespawnTimeVariance, RespawnTimeVariance);
		ActualRespawnTime = FMath::Max(0.1f, ActualRespawnTime); // Ensure minimum time
	}

	// Set timer for respawn
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&UResourceNodeComponent::HandleRespawn,
		ActualRespawnTime,
		false
	);
}

void UResourceNodeComponent::HandleRespawn()
{
	// Reset state
	bIsAvailable = true;
	CurrentHarvestCount = 0;

	OnNodeRespawned();
}
