#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AntigravityHUD.generated.h"

/**
 * 
 */
UCLASS()
class ANTIGRAVITYTEST_API AAntigravityHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	TSharedPtr<class SInventoryWidget> InventoryWidget;
};
