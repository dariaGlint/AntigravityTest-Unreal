// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParameterAdjustmentSubsystem.h"
#include "CommandConsoleManager.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

void UParameterAdjustmentSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize default values
	SpeedMultiplier = 1.0f;
	GravityScale = 1.0f;
	JumpHeightMultiplier = 1.0f;
	DamageMultiplier = 1.0f;
	ExpMultiplier = 1.0f;
	RenderDistance = 10000.0f;
	CurrentFOV = 90.0f;

	RegisterParameterCommands();
}

void UParameterAdjustmentSubsystem::Deinitialize()
{
	UnregisterParameterCommands();

	Super::Deinitialize();
}

void UParameterAdjustmentSubsystem::RegisterParameterCommands()
{
	UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
	if (!Manager)
	{
		return;
	}

	Manager->RegisterCommand(
		TEXT("setspeed"),
		TEXT("Set player movement speed multiplier"),
		TEXT("setspeed <multiplier>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetSpeed)
	);

	Manager->RegisterCommand(
		TEXT("setgravity"),
		TEXT("Set gravity scale"),
		TEXT("setgravity <scale>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetGravity)
	);

	Manager->RegisterCommand(
		TEXT("setjump"),
		TEXT("Set jump height multiplier"),
		TEXT("setjump <multiplier>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetJump)
	);

	Manager->RegisterCommand(
		TEXT("setdamage"),
		TEXT("Set damage multiplier"),
		TEXT("setdamage <multiplier>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetDamage)
	);

	Manager->RegisterCommand(
		TEXT("setexp"),
		TEXT("Set experience gain multiplier"),
		TEXT("setexp <multiplier>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetExp)
	);

	Manager->RegisterCommand(
		TEXT("setrenderdist"),
		TEXT("Set render distance"),
		TEXT("setrenderdist <distance>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetRenderDistance)
	);

	Manager->RegisterCommand(
		TEXT("setfov"),
		TEXT("Set field of view"),
		TEXT("setfov <degrees>"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleSetFOV)
	);

	Manager->RegisterCommand(
		TEXT("resetparams"),
		TEXT("Reset all parameters to default"),
		TEXT("resetparams"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleResetParams)
	);

	Manager->RegisterCommand(
		TEXT("listparams"),
		TEXT("List all current parameter values"),
		TEXT("listparams"),
		true,
		FConsoleCommandDelegate::CreateUObject(this, &UParameterAdjustmentSubsystem::HandleListParams)
	);
}

void UParameterAdjustmentSubsystem::UnregisterParameterCommands()
{
	UCommandConsoleManager* Manager = UCommandConsoleManager::Get(this);
	if (!Manager)
	{
		return;
	}

	Manager->UnregisterCommand(TEXT("setspeed"));
	Manager->UnregisterCommand(TEXT("setgravity"));
	Manager->UnregisterCommand(TEXT("setjump"));
	Manager->UnregisterCommand(TEXT("setdamage"));
	Manager->UnregisterCommand(TEXT("setexp"));
	Manager->UnregisterCommand(TEXT("setrenderdist"));
	Manager->UnregisterCommand(TEXT("setfov"));
	Manager->UnregisterCommand(TEXT("resetparams"));
	Manager->UnregisterCommand(TEXT("listparams"));
}

void UParameterAdjustmentSubsystem::SetSpeedMultiplier(float Multiplier)
{
	SpeedMultiplier = Multiplier;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
	{
		float DefaultSpeed = 600.0f; // Default UE character speed
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * SpeedMultiplier;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("Speed multiplier set to: %.2f"), Multiplier));
	}
}

void UParameterAdjustmentSubsystem::SetGravityScale(float Scale)
{
	GravityScale = Scale;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
	{
		PlayerCharacter->GetCharacterMovement()->GravityScale = GravityScale;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("Gravity scale set to: %.2f"), Scale));
	}
}

void UParameterAdjustmentSubsystem::SetJumpHeightMultiplier(float Multiplier)
{
	JumpHeightMultiplier = Multiplier;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
	{
		float DefaultJumpVelocity = 420.0f; // Default UE jump velocity
		PlayerCharacter->GetCharacterMovement()->JumpZVelocity = DefaultJumpVelocity * JumpHeightMultiplier;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("Jump height multiplier set to: %.2f"), Multiplier));
	}
}

void UParameterAdjustmentSubsystem::SetDamageMultiplier(float Multiplier)
{
	DamageMultiplier = Multiplier;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("Damage multiplier set to: %.2f"), Multiplier));
	}

	// TODO: Integrate with game's damage system
}

void UParameterAdjustmentSubsystem::SetExpMultiplier(float Multiplier)
{
	ExpMultiplier = Multiplier;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("Experience multiplier set to: %.2f"), Multiplier));
	}

	// TODO: Integrate with game's experience system
}

void UParameterAdjustmentSubsystem::SetRenderDistance(float Distance)
{
	RenderDistance = Distance;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("Render distance set to: %.0f"), Distance));
	}

	// TODO: Integrate with rendering system
}

void UParameterAdjustmentSubsystem::SetFOV(float FOV)
{
	CurrentFOV = FMath::Clamp(FOV, 60.0f, 120.0f);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->SetFOV(CurrentFOV);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("FOV set to: %.0f"), CurrentFOV));
	}
}

void UParameterAdjustmentSubsystem::ResetAllParameters()
{
	SetSpeedMultiplier(1.0f);
	SetGravityScale(1.0f);
	SetJumpHeightMultiplier(1.0f);
	SetDamageMultiplier(1.0f);
	SetExpMultiplier(1.0f);
	SetRenderDistance(10000.0f);
	SetFOV(90.0f);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
			TEXT("All parameters reset to default"));
	}
}

void UParameterAdjustmentSubsystem::ListParameters()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("=== Current Parameters ==="));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("Speed Multiplier: %.2f"), SpeedMultiplier));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("Gravity Scale: %.2f"), GravityScale));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("Jump Height Multiplier: %.2f"), JumpHeightMultiplier));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("Damage Multiplier: %.2f"), DamageMultiplier));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("Experience Multiplier: %.2f"), ExpMultiplier));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("Render Distance: %.0f"), RenderDistance));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
			FString::Printf(TEXT("FOV: %.0f"), CurrentFOV));
	}
}

// Command handlers
void UParameterAdjustmentSubsystem::HandleSetSpeed(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setspeed <multiplier>"));
		}
		return;
	}

	float Multiplier = FCString::Atof(*Args[0]);
	SetSpeedMultiplier(Multiplier);
}

void UParameterAdjustmentSubsystem::HandleSetGravity(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setgravity <scale>"));
		}
		return;
	}

	float Scale = FCString::Atof(*Args[0]);
	SetGravityScale(Scale);
}

void UParameterAdjustmentSubsystem::HandleSetJump(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setjump <multiplier>"));
		}
		return;
	}

	float Multiplier = FCString::Atof(*Args[0]);
	SetJumpHeightMultiplier(Multiplier);
}

void UParameterAdjustmentSubsystem::HandleSetDamage(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setdamage <multiplier>"));
		}
		return;
	}

	float Multiplier = FCString::Atof(*Args[0]);
	SetDamageMultiplier(Multiplier);
}

void UParameterAdjustmentSubsystem::HandleSetExp(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setexp <multiplier>"));
		}
		return;
	}

	float Multiplier = FCString::Atof(*Args[0]);
	SetExpMultiplier(Multiplier);
}

void UParameterAdjustmentSubsystem::HandleSetRenderDistance(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setrenderdist <distance>"));
		}
		return;
	}

	float Distance = FCString::Atof(*Args[0]);
	SetRenderDistance(Distance);
}

void UParameterAdjustmentSubsystem::HandleSetFOV(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Usage: setfov <degrees>"));
		}
		return;
	}

	float FOV = FCString::Atof(*Args[0]);
	SetFOV(FOV);
}

void UParameterAdjustmentSubsystem::HandleResetParams(const TArray<FString>& Args)
{
	ResetAllParameters();
}

void UParameterAdjustmentSubsystem::HandleListParams(const TArray<FString>& Args)
{
	ListParameters();
}
