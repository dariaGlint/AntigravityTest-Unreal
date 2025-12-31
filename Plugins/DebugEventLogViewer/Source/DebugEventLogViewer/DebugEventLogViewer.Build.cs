// Copyright Antigravity. All Rights Reserved.

using UnrealBuildTool;

public class DebugEventLogViewer : ModuleRules
{
	public DebugEventLogViewer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UMG",
				"Slate",
				"SlateCore",
				"Json",
				"JsonUtilities",
				"ApplicationCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"DebugConsoleCommands",
			}
		);

		// Only include in non-shipping builds
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("WITH_EVENT_LOG=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_EVENT_LOG=0");
		}
	}
}
