// Copyright Antigravity. All Rights Reserved.

using UnrealBuildTool;

public class DebugConsoleCommands : ModuleRules
{
	public DebugConsoleCommands(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"AIModule",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);

		// Only include in non-shipping builds
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("WITH_DEBUG_COMMANDS=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_DEBUG_COMMANDS=0");
		}
	}
}
