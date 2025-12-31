// Copyright Epic Games, Inc. All Rights Reserved.

#include "SnapshotManagerWidget.h"
#include "StateSnapshotSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USnapshotManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get snapshot subsystem
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		SnapshotSubsystem = GameInstance->GetSubsystem<UStateSnapshotSubsystem>();
	}

	RefreshSnapshotList();
}

void USnapshotManagerWidget::RefreshSnapshotList()
{
	if (!SnapshotSubsystem)
	{
		return;
	}

	TArray<FString> SlotNames = SnapshotSubsystem->GetSnapshotSlots();
	BP_OnSnapshotListRefreshed(SlotNames);
}

void USnapshotManagerWidget::OnSaveSnapshot(const FString& SlotName)
{
	if (!SnapshotSubsystem)
	{
		BP_OnSnapshotOperationComplete(false, TEXT("Snapshot subsystem not available"));
		return;
	}

	FSnapshotSaveOptions Options;
	Options.bGenerateThumbnail = true;

	bool bSuccess = SnapshotSubsystem->SaveSnapshot(SlotName, Options);
	FString Message = bSuccess ? TEXT("Snapshot saved successfully") : TEXT("Failed to save snapshot");

	BP_OnSnapshotOperationComplete(bSuccess, Message);

	if (bSuccess)
	{
		RefreshSnapshotList();
	}
}

void USnapshotManagerWidget::OnLoadSnapshot(const FString& SlotName)
{
	if (!SnapshotSubsystem)
	{
		BP_OnSnapshotOperationComplete(false, TEXT("Snapshot subsystem not available"));
		return;
	}

	FSnapshotLoadOptions Options;
	bool bSuccess = SnapshotSubsystem->LoadSnapshot(SlotName, Options);
	FString Message = bSuccess ? TEXT("Snapshot loaded successfully") : TEXT("Failed to load snapshot");

	BP_OnSnapshotOperationComplete(bSuccess, Message);
}

void USnapshotManagerWidget::OnDeleteSnapshot(const FString& SlotName)
{
	if (!SnapshotSubsystem)
	{
		BP_OnSnapshotOperationComplete(false, TEXT("Snapshot subsystem not available"));
		return;
	}

	bool bSuccess = SnapshotSubsystem->DeleteSnapshot(SlotName);
	FString Message = bSuccess ? TEXT("Snapshot deleted successfully") : TEXT("Failed to delete snapshot");

	BP_OnSnapshotOperationComplete(bSuccess, Message);

	if (bSuccess)
	{
		RefreshSnapshotList();
	}
}

void USnapshotManagerWidget::OnExportSnapshot(const FString& SlotName, const FString& FilePath)
{
	if (!SnapshotSubsystem)
	{
		BP_OnSnapshotOperationComplete(false, TEXT("Snapshot subsystem not available"));
		return;
	}

	bool bSuccess = SnapshotSubsystem->ExportSnapshot(SlotName, FilePath);
	FString Message = bSuccess ? TEXT("Snapshot exported successfully") : TEXT("Failed to export snapshot");

	BP_OnSnapshotOperationComplete(bSuccess, Message);
}

void USnapshotManagerWidget::OnImportSnapshot(const FString& FilePath, const FString& SlotName)
{
	if (!SnapshotSubsystem)
	{
		BP_OnSnapshotOperationComplete(false, TEXT("Snapshot subsystem not available"));
		return;
	}

	bool bSuccess = SnapshotSubsystem->ImportSnapshot(FilePath, SlotName);
	FString Message = bSuccess ? TEXT("Snapshot imported successfully") : TEXT("Failed to import snapshot");

	BP_OnSnapshotOperationComplete(bSuccess, Message);

	if (bSuccess)
	{
		RefreshSnapshotList();
	}
}

void USnapshotManagerWidget::ToggleVisibility()
{
	SetVisibility(GetVisibility() == ESlateVisibility::Visible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

bool USnapshotManagerWidget::GetSnapshotPreviewInfo(const FString& SlotName, FStateSnapshot& OutInfo)
{
	if (!SnapshotSubsystem)
	{
		return false;
	}

	return SnapshotSubsystem->GetSnapshotInfo(SlotName, OutInfo);
}
