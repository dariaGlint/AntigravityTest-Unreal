// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AntigravityTest : ModuleRules
{
	public AntigravityTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AntigravityTest",
			"AntigravityTest/Variant_Platforming",
			"AntigravityTest/Variant_Platforming/Animation",
			"AntigravityTest/Variant_Combat",
			"AntigravityTest/Variant_Combat/AI",
			"AntigravityTest/Variant_Combat/Animation",
			"AntigravityTest/Variant_Combat/Gameplay",
			"AntigravityTest/Variant_Combat/Interfaces",
			"AntigravityTest/Variant_Combat/UI",
			"AntigravityTest/Variant_SideScrolling",
			"AntigravityTest/Variant_SideScrolling/AI",
			"AntigravityTest/Variant_SideScrolling/Gameplay",
			"AntigravityTest/Variant_SideScrolling/Interfaces",
			"AntigravityTest/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
