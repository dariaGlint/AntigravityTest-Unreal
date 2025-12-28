#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

/**
 * 
 */
class ANTIGRAVITYTEST_API SInventoryWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryWidget)
	{}
		SLATE_ARGUMENT(TWeakObjectPtr<class UInventoryComponent>, InventoryComponent)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	// Refreshes the item list from the component
	void RefreshList();

private:
	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	TSharedPtr<class SGridPanel> ItemGrid;
};
