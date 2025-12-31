// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateSnapshotSubsystem.h"
#include "ISnapshotable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"

const FString UStateSnapshotSubsystem::QuickSaveSlotName = TEXT("QuickSave");
const FString UStateSnapshotSubsystem::SnapshotFileExtension = TEXT(".snapshot");

void UStateSnapshotSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_DEBUG_STATE_SNAPSHOT
	UE_LOG(LogTemp, Log, TEXT("StateSnapshotSubsystem initialized"));
#endif
}

void UStateSnapshotSubsystem::Deinitialize()
{
#if WITH_DEBUG_STATE_SNAPSHOT
	UE_LOG(LogTemp, Log, TEXT("StateSnapshotSubsystem deinitialized"));
#endif

	Super::Deinitialize();
}

bool UStateSnapshotSubsystem::SaveSnapshot(const FString& SlotName, const FSnapshotSaveOptions& Options)
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	if (SlotName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveSnapshot: Invalid slot name"));
		return false;
	}

	FStateSnapshot Snapshot;
	Snapshot.SnapshotID = FGuid::NewGuid().ToString();
	Snapshot.SnapshotName = SlotName;
	Snapshot.Timestamp = FDateTime::Now();

	// Get current level name
	UWorld* World = GetWorld();
	if (World)
	{
		Snapshot.LevelName = World->GetMapName();
	}

	// Serialize player state
	if (Options.bIncludePlayerState)
	{
		if (!SerializePlayerState(Snapshot.PlayerStateData))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveSnapshot: Failed to serialize player state"));
		}
	}

	// Serialize enemy states
	if (Options.bIncludeEnemyStates)
	{
		if (!SerializeEnemyStates(Snapshot.EnemyStatesData))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveSnapshot: Failed to serialize enemy states"));
		}
	}

	// Serialize world state
	if (Options.bIncludeWorldState)
	{
		if (!SerializeWorldState(Snapshot.WorldStateData))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveSnapshot: Failed to serialize world state"));
		}
	}

	// Generate thumbnail
	if (Options.bGenerateThumbnail)
	{
		GenerateThumbnail(Snapshot.ThumbnailData);
	}

	// Add custom metadata
	Snapshot.Metadata = Options.CustomMetadata;

	// Save to disk
	if (!SaveSnapshotToDisk(SlotName, Snapshot))
	{
		UE_LOG(LogTemp, Error, TEXT("SaveSnapshot: Failed to save snapshot to disk"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("SaveSnapshot: Successfully saved snapshot '%s'"), *SlotName);
	return true;
#endif
}

bool UStateSnapshotSubsystem::LoadSnapshot(const FString& SlotName, const FSnapshotLoadOptions& Options)
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	if (SlotName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadSnapshot: Invalid slot name"));
		return false;
	}

	FStateSnapshot Snapshot;
	if (!LoadSnapshotFromDisk(SlotName, Snapshot))
	{
		UE_LOG(LogTemp, Error, TEXT("LoadSnapshot: Failed to load snapshot from disk"));
		return false;
	}

	// Check if we need to reload the level
	UWorld* World = GetWorld();
	if (World && Options.bReloadLevelIfNeeded)
	{
		FString CurrentLevel = World->GetMapName();
		if (!Snapshot.LevelName.IsEmpty() && CurrentLevel != Snapshot.LevelName)
		{
			// TODO: Implement level transition
			UE_LOG(LogTemp, Warning, TEXT("LoadSnapshot: Level transition not yet implemented"));
		}
	}

	// Restore player state
	if (Options.bRestorePlayerState && Snapshot.PlayerStateData.Num() > 0)
	{
		if (!DeserializePlayerState(Snapshot.PlayerStateData))
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadSnapshot: Failed to restore player state"));
		}
	}

	// Restore enemy states
	if (Options.bRestoreEnemyStates && Snapshot.EnemyStatesData.Num() > 0)
	{
		if (!DeserializeEnemyStates(Snapshot.EnemyStatesData))
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadSnapshot: Failed to restore enemy states"));
		}
	}

	// Restore world state
	if (Options.bRestoreWorldState && Snapshot.WorldStateData.Num() > 0)
	{
		if (!DeserializeWorldState(Snapshot.WorldStateData))
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadSnapshot: Failed to restore world state"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("LoadSnapshot: Successfully loaded snapshot '%s'"), *SlotName);
	return true;
#endif
}

bool UStateSnapshotSubsystem::QuickSave()
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	FSnapshotSaveOptions Options;
	Options.bGenerateThumbnail = true;

	bool bResult = SaveSnapshot(QuickSaveSlotName, Options);

	if (bResult)
	{
		// TODO: Show notification
		UE_LOG(LogTemp, Log, TEXT("Quick Save successful"));
	}

	return bResult;
#endif
}

bool UStateSnapshotSubsystem::QuickLoad()
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	if (!DoesSnapshotExist(QuickSaveSlotName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Quick Load: No quick save found"));
		return false;
	}

	FSnapshotLoadOptions Options;
	bool bResult = LoadSnapshot(QuickSaveSlotName, Options);

	if (bResult)
	{
		// TODO: Show notification
		UE_LOG(LogTemp, Log, TEXT("Quick Load successful"));
	}

	return bResult;
#endif
}

TArray<FString> UStateSnapshotSubsystem::GetSnapshotSlots() const
{
	TArray<FString> Slots;

#if WITH_DEBUG_STATE_SNAPSHOT
	FString SaveDir = GetSaveDirectory();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.DirectoryExists(*SaveDir))
	{
		TArray<FString> Files;
		PlatformFile.FindFiles(Files, *SaveDir, *SnapshotFileExtension);

		for (const FString& File : Files)
		{
			FString SlotName = FPaths::GetBaseFilename(File);
			Slots.Add(SlotName);
		}
	}
#endif

	return Slots;
}

bool UStateSnapshotSubsystem::GetSnapshotInfo(const FString& SlotName, FStateSnapshot& OutSnapshot) const
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	return LoadSnapshotFromDisk(SlotName, OutSnapshot);
#endif
}

bool UStateSnapshotSubsystem::DeleteSnapshot(const FString& SlotName)
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	FString FilePath = GetSnapshotFilePath(SlotName);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*FilePath))
	{
		if (PlatformFile.DeleteFile(*FilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("DeleteSnapshot: Successfully deleted '%s'"), *SlotName);
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("DeleteSnapshot: Failed to delete '%s'"), *SlotName);
	return false;
#endif
}

bool UStateSnapshotSubsystem::ExportSnapshot(const FString& SlotName, const FString& FilePath)
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	FStateSnapshot Snapshot;
	if (!LoadSnapshotFromDisk(SlotName, Snapshot))
	{
		return false;
	}

	// Serialize to JSON
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);

	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("SnapshotID"), Snapshot.SnapshotID);
	Writer->WriteValue(TEXT("SnapshotName"), Snapshot.SnapshotName);
	Writer->WriteValue(TEXT("Timestamp"), Snapshot.Timestamp.ToString());
	Writer->WriteValue(TEXT("LevelName"), Snapshot.LevelName);
	// TODO: Serialize binary data as base64
	Writer->WriteObjectEnd();
	Writer->Close();

	return FFileHelper::SaveStringToFile(JsonString, *FilePath);
#endif
}

bool UStateSnapshotSubsystem::ImportSnapshot(const FString& FilePath, const FString& SlotName)
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	// TODO: Implement import from JSON
	UE_LOG(LogTemp, Warning, TEXT("ImportSnapshot: Not yet implemented"));
	return false;
#endif
}

bool UStateSnapshotSubsystem::DoesSnapshotExist(const FString& SlotName) const
{
#if !WITH_DEBUG_STATE_SNAPSHOT
	return false;
#else
	FString FilePath = GetSnapshotFilePath(SlotName);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	return PlatformFile.FileExists(*FilePath);
#endif
}

bool UStateSnapshotSubsystem::SaveSnapshotToDisk(const FString& SlotName, const FStateSnapshot& Snapshot)
{
	FString FilePath = GetSnapshotFilePath(SlotName);

	// Ensure directory exists
	FString SaveDir = GetSaveDirectory();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*SaveDir))
	{
		PlatformFile.CreateDirectory(*SaveDir);
	}

	// Serialize snapshot
	FMemoryWriter MemoryWriter(const_cast<TArray<uint8>&>(Snapshot.PlayerStateData), true);
	// TODO: Properly serialize the FStateSnapshot struct

	// For now, use a simple binary format
	TArray<uint8> SaveData;
	FMemoryWriter Writer(SaveData);

	// Write header
	Writer << const_cast<FString&>(Snapshot.SnapshotID);
	Writer << const_cast<FString&>(Snapshot.SnapshotName);
	Writer << const_cast<FString&>(Snapshot.LevelName);

	// Write data arrays
	Writer << const_cast<TArray<uint8>&>(Snapshot.PlayerStateData);
	Writer << const_cast<TArray<uint8>&>(Snapshot.EnemyStatesData);
	Writer << const_cast<TArray<uint8>&>(Snapshot.WorldStateData);
	Writer << const_cast<TArray<uint8>&>(Snapshot.ThumbnailData);

	return FFileHelper::SaveArrayToFile(SaveData, *FilePath);
}

bool UStateSnapshotSubsystem::LoadSnapshotFromDisk(const FString& SlotName, FStateSnapshot& OutSnapshot) const
{
	FString FilePath = GetSnapshotFilePath(SlotName);

	TArray<uint8> LoadData;
	if (!FFileHelper::LoadFileToArray(LoadData, *FilePath))
	{
		return false;
	}

	FMemoryReader Reader(LoadData);

	// Read header
	Reader << OutSnapshot.SnapshotID;
	Reader << OutSnapshot.SnapshotName;
	Reader << OutSnapshot.LevelName;

	// Read data arrays
	Reader << OutSnapshot.PlayerStateData;
	Reader << OutSnapshot.EnemyStatesData;
	Reader << OutSnapshot.WorldStateData;
	Reader << OutSnapshot.ThumbnailData;

	return true;
}

FString UStateSnapshotSubsystem::GetSaveDirectory() const
{
	return FPaths::ProjectSavedDir() / TEXT("Snapshots");
}

FString UStateSnapshotSubsystem::GetSnapshotFilePath(const FString& SlotName) const
{
	return GetSaveDirectory() / (SlotName + SnapshotFileExtension);
}

bool UStateSnapshotSubsystem::SerializePlayerState(TArray<uint8>& OutData)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return false;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		return false;
	}

	// Check if player implements ISnapshotable
	if (PlayerPawn->Implements<USnapshotable>())
	{
		return ISnapshotable::Execute_SaveState(PlayerPawn, OutData);
	}

	// Fallback: Save basic transform data
	FMemoryWriter Writer(OutData);
	FVector Location = PlayerPawn->GetActorLocation();
	FRotator Rotation = PlayerPawn->GetActorRotation();

	Writer << Location;
	Writer << Rotation;

	return true;
}

bool UStateSnapshotSubsystem::DeserializePlayerState(const TArray<uint8>& InData)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return false;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		return false;
	}

	// Check if player implements ISnapshotable
	if (PlayerPawn->Implements<USnapshotable>())
	{
		return ISnapshotable::Execute_RestoreState(PlayerPawn, InData);
	}

	// Fallback: Restore basic transform data
	FMemoryReader Reader(const_cast<TArray<uint8>&>(InData));
	FVector Location;
	FRotator Rotation;

	Reader << Location;
	Reader << Rotation;

	PlayerPawn->SetActorLocationAndRotation(Location, Rotation);

	return true;
}

bool UStateSnapshotSubsystem::SerializeEnemyStates(TArray<uint8>& OutData)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FMemoryWriter Writer(OutData);

	TArray<AActor*> SnapshotableActors;

	// Find all actors that implement ISnapshotable
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->Implements<USnapshotable>())
		{
			// Skip player pawn (handled separately)
			APlayerController* PC = World->GetFirstPlayerController();
			if (PC && Actor == PC->GetPawn())
			{
				continue;
			}

			TArray<uint8> ActorData;
			FString ActorID = ISnapshotable::Execute_GetSnapshotID(Actor);

			if (ISnapshotable::Execute_SaveState(Actor, ActorData))
			{
				Writer << ActorID;
				Writer << ActorData;
			}
		}
	}

	return true;
}

bool UStateSnapshotSubsystem::DeserializeEnemyStates(const TArray<uint8>& InData)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FMemoryReader Reader(const_cast<TArray<uint8>&>(InData));

	// Build map of existing snapshotable actors
	TMap<FString, AActor*> SnapshotableActors;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->Implements<USnapshotable>())
		{
			FString ActorID = ISnapshotable::Execute_GetSnapshotID(Actor);
			SnapshotableActors.Add(ActorID, Actor);
		}
	}

	// Restore states
	while (!Reader.AtEnd())
	{
		FString ActorID;
		TArray<uint8> ActorData;

		Reader << ActorID;
		Reader << ActorData;

		if (AActor** FoundActor = SnapshotableActors.Find(ActorID))
		{
			ISnapshotable::Execute_RestoreState(*FoundActor, ActorData);
		}
	}

	return true;
}

bool UStateSnapshotSubsystem::SerializeWorldState(TArray<uint8>& OutData)
{
	// TODO: Implement world state serialization
	// This could include dynamic objects, collectibles, etc.
	return true;
}

bool UStateSnapshotSubsystem::DeserializeWorldState(const TArray<uint8>& InData)
{
	// TODO: Implement world state deserialization
	return true;
}

bool UStateSnapshotSubsystem::GenerateThumbnail(TArray<uint8>& OutThumbnailData)
{
	// TODO: Implement thumbnail generation
	// This would involve taking a screenshot and encoding it as PNG
	return false;
}
