#include "ResourceNodeActor.h"
#include "ResourceNodeComponent.h"
#include "Components/StaticMeshComponent.h"

AResourceNodeActor::AResourceNodeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Create resource node component
	ResourceNodeComponent = CreateDefaultSubobject<UResourceNodeComponent>(TEXT("ResourceNodeComponent"));
}

void AResourceNodeActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind to resource node events
	if (ResourceNodeComponent)
	{
		ResourceNodeComponent->OnDepleted.AddDynamic(this, &AResourceNodeActor::OnNodeDepleted);
		ResourceNodeComponent->OnRespawned.AddDynamic(this, &AResourceNodeActor::OnNodeRespawned);
	}

	// Set initial visual state
	UpdateVisualState();
}

void AResourceNodeActor::OnNodeDepleted()
{
	UpdateVisualState();
}

void AResourceNodeActor::OnNodeRespawned()
{
	UpdateVisualState();
}

void AResourceNodeActor::UpdateVisualState_Implementation()
{
	if (!MeshComponent || !ResourceNodeComponent)
	{
		return;
	}

	if (ResourceNodeComponent->bIsAvailable)
	{
		// Node is available
		SetActorHiddenInGame(false);

		if (AvailableMesh)
		{
			MeshComponent->SetStaticMesh(AvailableMesh);
		}

		if (AvailableMaterial)
		{
			MeshComponent->SetMaterial(0, AvailableMaterial);
		}
	}
	else
	{
		// Node is depleted
		if (bHideWhenDepleted)
		{
			SetActorHiddenInGame(true);
		}
		else
		{
			if (DepletedMesh)
			{
				MeshComponent->SetStaticMesh(DepletedMesh);
			}

			if (DepletedMaterial)
			{
				MeshComponent->SetMaterial(0, DepletedMaterial);
			}
		}
	}
}
