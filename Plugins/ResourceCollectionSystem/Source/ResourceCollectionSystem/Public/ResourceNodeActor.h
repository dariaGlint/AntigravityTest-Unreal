#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceNodeActor.generated.h"

class UResourceNodeComponent;
class UStaticMeshComponent;

/**
 * Base actor class for resource nodes that can be placed in the world
 * Examples: ore deposits, trees, bushes, herb patches, etc.
 */
UCLASS(Blueprintable)
class RESOURCECOLLECTIONSYSTEM_API AResourceNodeActor : public AActor
{
	GENERATED_BODY()

public:
	AResourceNodeActor();

protected:
	virtual void BeginPlay() override;

public:
	// Visual representation of the resource node
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// Resource node component that handles harvesting logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UResourceNodeComponent> ResourceNodeComponent;

	// Mesh to use when the node is available
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Visuals")
	TObjectPtr<UStaticMesh> AvailableMesh;

	// Mesh to use when the node is depleted (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Visuals")
	TObjectPtr<UStaticMesh> DepletedMesh;

	// Material to use when the node is available
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Visuals")
	TObjectPtr<UMaterialInterface> AvailableMaterial;

	// Material to use when the node is depleted (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Visuals")
	TObjectPtr<UMaterialInterface> DepletedMaterial;

	// Whether to hide the actor when depleted instead of changing mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Node|Visuals")
	bool bHideWhenDepleted = false;

protected:
	UFUNCTION()
	void OnNodeDepleted();

	UFUNCTION()
	void OnNodeRespawned();

	/**
	 * Update the visual state of the node based on availability
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Resource Node")
	void UpdateVisualState();
};
