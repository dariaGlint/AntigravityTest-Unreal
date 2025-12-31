#include "CombatEventListener.h"
#include "EventLogSubsystem.h"

UCombatEventListener::UCombatEventListener()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatEventListener::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		EventLogSubsystem = GameInstance->GetSubsystem<UEventLogSubsystem>();
	}
}

void UCombatEventListener::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EventLogSubsystem = nullptr;
	Super::EndPlay(EndPlayReason);
}

void UCombatEventListener::LogDamageEvent(float DamageAmount, AActor* DamageCauser, const FVector& DamageLocation)
{
#if WITH_DEBUG_EVENT_LOG
	if (!EventLogSubsystem)
	{
		return;
	}

	const FString Message = FString::Printf(TEXT("Took %.1f damage from %s at %s"),
		DamageAmount,
		DamageCauser ? *DamageCauser->GetName() : TEXT("Unknown"),
		*DamageLocation.ToString());

	const FString ContextData = FString::Printf(TEXT("{\"damage\":%.1f,\"location\":\"%s\"}"),
		DamageAmount,
		*DamageLocation.ToString());

	EventLogSubsystem->LogEvent(
		EDebugEventCategory::Combat,
		EDebugEventSeverity::Info,
		Message,
		GetOwner(),
		ContextData
	);
#endif
}

void UCombatEventListener::LogDeathEvent(AActor* Killer)
{
#if WITH_DEBUG_EVENT_LOG
	if (!EventLogSubsystem)
	{
		return;
	}

	const FString Message = FString::Printf(TEXT("Died (killed by %s)"),
		Killer ? *Killer->GetName() : TEXT("Unknown"));

	const FString ContextData = FString::Printf(TEXT("{\"killer\":\"%s\"}"),
		Killer ? *Killer->GetName() : TEXT("Unknown"));

	EventLogSubsystem->LogEvent(
		EDebugEventCategory::Combat,
		EDebugEventSeverity::Warning,
		Message,
		GetOwner(),
		ContextData
	);
#endif
}

void UCombatEventListener::LogAttackEvent(const FString& AttackType, AActor* Target)
{
#if WITH_DEBUG_EVENT_LOG
	if (!EventLogSubsystem)
	{
		return;
	}

	const FString Message = FString::Printf(TEXT("Performed %s attack on %s"),
		*AttackType,
		Target ? *Target->GetName() : TEXT("Unknown"));

	const FString ContextData = FString::Printf(TEXT("{\"attackType\":\"%s\",\"target\":\"%s\"}"),
		*AttackType,
		Target ? *Target->GetName() : TEXT("Unknown"));

	EventLogSubsystem->LogEvent(
		EDebugEventCategory::Combat,
		EDebugEventSeverity::Info,
		Message,
		GetOwner(),
		ContextData
	);
#endif
}

void UCombatEventListener::LogHealEvent(float HealAmount, AActor* Healer)
{
#if WITH_DEBUG_EVENT_LOG
	if (!EventLogSubsystem)
	{
		return;
	}

	const FString Message = FString::Printf(TEXT("Healed for %.1f by %s"),
		HealAmount,
		Healer ? *Healer->GetName() : TEXT("Self"));

	const FString ContextData = FString::Printf(TEXT("{\"heal\":%.1f}"), HealAmount);

	EventLogSubsystem->LogEvent(
		EDebugEventCategory::Combat,
		EDebugEventSeverity::Info,
		Message,
		GetOwner(),
		ContextData
	);
#endif
}
