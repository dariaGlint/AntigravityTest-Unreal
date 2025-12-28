#include "AntigravityHUD.h"
#include "SInventoryWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Engine/Engine.h"

#include "GameFramework/PlayerController.h"
#include "AntigravityTestCharacter.h"
#include "Component/InventoryComponent.h"

void AAntigravityHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		UInventoryComponent* InvComp = nullptr;
		if (APlayerController* PC = GetOwningPlayerController())
		{
			if (AAntigravityTestCharacter* Character = Cast<AAntigravityTestCharacter>(PC->GetPawn()))
			{
				InvComp = Character->GetInventoryComponent();
			}
		}

		InventoryWidget = SNew(SInventoryWidget)
			.InventoryComponent(InvComp);
		
		// Add the widget to the viewport. 
		// We use a high Z-order to ensure it's on top of other elements if needed.
		GEngine->GameViewport->AddViewportWidgetContent(
			InventoryWidget.ToSharedRef()
		);
	}
}
