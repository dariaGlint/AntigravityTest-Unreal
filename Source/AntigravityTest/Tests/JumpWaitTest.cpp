#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "AntigravityTest/AntigravityTestCharacter.h"
#include "AntigravityTest/GA_JumpWait.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FJumpWaitTest, "AntigravityTest.JumpWait", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FJumpWaitTest::RunTest(const FString& Parameters)
{
	// Create a temporary world
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!World)
	{
		AddError("Failed to create World");
		return false;
	}

	// Spawn Character
	AAntigravityTestCharacter* Character = World->SpawnActor<AAntigravityTestCharacter>();
	if (!Character)
	{
		AddError("Failed to spawn Character");
		World->DestroyWorld(false);
		return false;
	}

	// Initialize ASC
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC)
	{
		AddError("Character has no ASC");
		World->DestroyWorld(false);
		return false;
	}
	ASC->InitAbilityActorInfo(Character, Character);

	// Grant Ability
	FGameplayAbilitySpec Spec(UGA_JumpWait::StaticClass(), 1, -1, Character);
	FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);

	// Activate Ability
	bool bActivated = ASC->TryActivateAbility(Handle);
	if (!bActivated)
	{
		AddError("Failed to activate GA_JumpWait");
		World->DestroyWorld(false);
		return false;
	}

	// Check initial state: Should NOT be jumping
	// We check bPressedJump using reflection because it's protected
	FBoolProperty* PressedJumpProp = FindFProperty<FBoolProperty>(ACharacter::StaticClass(), TEXT("bPressedJump"));
	bool bPressedJump = false;
	if (PressedJumpProp)
	{
		bPressedJump = PressedJumpProp->GetPropertyValue_InContainer(Character);
	}
	TestFalse("Character should not be jumping immediately", bPressedJump);

	// Tick World for 0.5s
	// We need to tick the TimerManager because AbilityTask_WaitDelay uses it
	World->GetTimerManager().Tick(0.5f);

	// Check state: Should still NOT be jumping
	if (PressedJumpProp)
	{
		bPressedJump = PressedJumpProp->GetPropertyValue_InContainer(Character);
	}
	TestFalse("Character should not be jumping after 0.5s", bPressedJump);

	// Tick World for another 0.6s (Total 1.1s)
	World->GetTimerManager().Tick(0.6f);

	// Check final state: Should be jumping
	if (PressedJumpProp)
	{
		bPressedJump = PressedJumpProp->GetPropertyValue_InContainer(Character);
	}
	TestTrue("Character should be jumping after 1.1s", bPressedJump);

	// Clean up
	World->DestroyWorld(false);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
