#include "SInventoryWidget.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Component/InventoryComponent.h"
#include "Items/ItemDefinition.h"



static TSharedRef<SWidget> CreateItemSlotWidget(const FInventoryItem* Item)
{
	FString ItemName = Item ? Item->ItemDef->ItemName.ToString() : "Empty";
	FString QuantityText = Item ? FString::Printf(TEXT("x%d"), Item->Quantity) : "";

	return SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.6f))
		.Padding(10.0f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(ItemName))
				.ColorAndOpacity(FLinearColor::White)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(QuantityText))
				.ColorAndOpacity(FLinearColor::Gray)
			]
		];
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryWidget::Construct(const FArguments& InArgs)
{
	InventoryComponent = InArgs._InventoryComponent;
	const FMargin SlotPadding = FMargin(10.0f);

	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnInventoryUpdated.AddRaw(this, &SInventoryWidget::RefreshList);
	}

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			// Semi-transparent background
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f))
			.Padding(50.0f)
			[
				SNew(SHorizontalBox)
				
				// Left Panel: Character Preview
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 20.0f, 0.0f)
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.05f, 1.0f))
					.Padding(20.0f)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString("CHARACTER"))
							.ColorAndOpacity(FLinearColor::White)
							.Justification(ETextJustify::Center)
						]
						+ SVerticalBox::Slot()
						.FillHeight(1.0f)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("[Preview Mesh]"))
							.ColorAndOpacity(FLinearColor::Gray)
						]
					]
				]

				// Right Panel: Inventory Grid
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SVerticalBox)
					
					// Title Header
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 20.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("INVENTORY"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 32))
						.ColorAndOpacity(FLinearColor(0.4f, 0.8f, 1.0f)) // Blueish tint
					]

					// Grid Area
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					[
						SNew(SBorder)
						.BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
						.Padding(0.0f)
						[

							// Grid Panel (Dynamic)
							SAssignNew(ItemGrid, SGridPanel)
							.FillColumn(0, 1.0f)
							.FillColumn(1, 1.0f)
							.FillColumn(2, 1.0f)
						]
					]
				]
			]
		]
	];

	RefreshList();
}

void SInventoryWidget::RefreshList()
{
	if (!ItemGrid.IsValid()) return;

	ItemGrid->ClearChildren();
	
	const FMargin SlotPadding = FMargin(10.0f);
	
	if (InventoryComponent.IsValid())
	{
		const TArray<FInventoryItem>& Items = InventoryComponent->GetItems();
		int32 Col = 0;
		int32 Row = 0;

		for (const FInventoryItem& Item : Items)
		{
			ItemGrid->AddSlot(Col, Row)
				.Padding(SlotPadding)
				[
					CreateItemSlotWidget(&Item)
				];

			Col++;
			if (Col > 2)
			{
				Col = 0;
				Row++;
			}
		}

		// Fill remaining empty slots up to minimal row/cols if needed
		// For now, just showing held items
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
