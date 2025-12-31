// Copyright Antigravity. All Rights Reserved.

#include "DebugCommandSubsystem.h"
#include "DebugConsoleCommandsModule.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Blueprint.h"

#if WITH_DEBUG_COMMANDS

namespace DebugCommands
{
	extern ACharacter* GetPlayerCharacter(UWorld* World);
	extern APawn* GetPlayerPawn(UWorld* World);
}

void UDebugCommandSubsystem::RegisterSpawnCommands()
{
	// SpawnActor - Spawn an actor by class name
	RegisterNativeCommand(
		TEXT("SpawnActor"),
		TEXT("Spawn an actor at player location or specified coordinates"),
		EDebugCommandCategory::Spawn,
		TEXT("SpawnActor <BlueprintPath> [X Y Z]"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: SpawnActor <BlueprintPath> [X Y Z]");
			}

			if (!World)
			{
				return TEXT("No world found");
			}

			// Get spawn location
			FVector SpawnLocation;
			if (Args.Num() >= 4)
			{
				SpawnLocation.X = FCString::Atof(*Args[1]);
				SpawnLocation.Y = FCString::Atof(*Args[2]);
				SpawnLocation.Z = FCString::Atof(*Args[3]);
			}
			else
			{
				// Spawn in front of player
				APawn* PlayerPawn = DebugCommands::GetPlayerPawn(World);
				if (PlayerPawn)
				{
					SpawnLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * 300.0f;
				}
				else
				{
					SpawnLocation = FVector::ZeroVector;
				}
			}

			// Try to load the blueprint
			FString BlueprintPath = Args[0];

			// Add Blueprint prefix if not present
			if (!BlueprintPath.Contains(TEXT(".")))
			{
				BlueprintPath = FString::Printf(TEXT("Blueprint'%s.%s'"),
					*BlueprintPath,
					*FPaths::GetBaseFilename(BlueprintPath));
			}

			UClass* ActorClass = nullptr;

			// Try to load as Blueprint
			UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *BlueprintPath);
			if (Blueprint && Blueprint->GeneratedClass)
			{
				ActorClass = Blueprint->GeneratedClass;
			}
			else
			{
				// Try to find C++ class
				ActorClass = FindObject<UClass>(nullptr, *Args[0]);
			}

			if (!ActorClass)
			{
				return FString::Printf(TEXT("Could not find class: %s"), *Args[0]);
			}

			// Spawn the actor
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

			if (SpawnedActor)
			{
				return FString::Printf(TEXT("Spawned %s at (%.1f, %.1f, %.1f)"),
					*ActorClass->GetName(),
					SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
			}
			else
			{
				return FString::Printf(TEXT("Failed to spawn %s"), *ActorClass->GetName());
			}
		}
	);

	// SpawnEnemy - Spawn enemy by type name
	RegisterNativeCommand(
		TEXT("SpawnEnemy"),
		TEXT("Spawn enemy actor(s) near player"),
		EDebugCommandCategory::Spawn,
		TEXT("SpawnEnemy <EnemyType> [Count]"),
		[](const TArray<FString>& Args, UWorld* World) -> FString
		{
			if (Args.Num() < 1)
			{
				return TEXT("Usage: SpawnEnemy <EnemyType> [Count]");
			}

			if (!World)
			{
				return TEXT("No world found");
			}

			int32 Count = 1;
			if (Args.Num() >= 2)
			{
				Count = FCString::Atoi(*Args[1]);
				if (Count <= 0) Count = 1;
				if (Count > 50) Count = 50; // Safety limit
			}

			// Get player location for spawn reference
			APawn* PlayerPawn = DebugCommands::GetPlayerPawn(World);
			FVector BaseLocation = PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;
			FVector ForwardVector = PlayerPawn ? PlayerPawn->GetActorForwardVector() : FVector::ForwardVector;

			// Try to load the enemy blueprint
			FString EnemyType = Args[0];
			FString BlueprintPath;

			// Check common enemy blueprint paths
			TArray<FString> SearchPaths = {
				FString::Printf(TEXT("/Game/Variant_Combat/Blueprints/%s.%s"), *EnemyType, *EnemyType),
				FString::Printf(TEXT("/Game/Variant_Combat/Blueprints/Enemies/%s.%s"), *EnemyType, *EnemyType),
				FString::Printf(TEXT("/Game/Blueprints/%s.%s"), *EnemyType, *EnemyType),
				FString::Printf(TEXT("/Game/Blueprints/Enemies/%s.%s"), *EnemyType, *EnemyType),
			};

			UClass* EnemyClass = nullptr;

			for (const FString& Path : SearchPaths)
			{
				UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *Path);
				if (Blueprint && Blueprint->GeneratedClass)
				{
					EnemyClass = Blueprint->GeneratedClass;
					break;
				}
			}

			// Try direct path if provided
			if (!EnemyClass)
			{
				UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *EnemyType);
				if (Blueprint && Blueprint->GeneratedClass)
				{
					EnemyClass = Blueprint->GeneratedClass;
				}
			}

			if (!EnemyClass)
			{
				return FString::Printf(TEXT("Could not find enemy type: %s"), *EnemyType);
			}

			// Spawn enemies
			int32 SpawnedCount = 0;
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			for (int32 i = 0; i < Count; ++i)
			{
				// Calculate spawn position in a spread pattern
				float Angle = (float)i / (float)Count * 360.0f;
				float Distance = 400.0f + (i / 8) * 200.0f;
				FVector Offset = ForwardVector.RotateAngleAxis(Angle, FVector::UpVector) * Distance;
				FVector SpawnLocation = BaseLocation + ForwardVector * 300.0f + Offset;

				AActor* SpawnedActor = World->SpawnActor<AActor>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				if (SpawnedActor)
				{
					SpawnedCount++;
				}
			}

			return FString::Printf(TEXT("Spawned %d/%d %s"), SpawnedCount, Count, *EnemyType);
		}
	);
}

#endif // WITH_DEBUG_COMMANDS
