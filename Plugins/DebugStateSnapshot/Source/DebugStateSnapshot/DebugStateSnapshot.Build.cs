using UnrealBuildTool;

public class DebugStateSnapshot : ModuleRules
{
	public DebugStateSnapshot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UMG",
				"Slate",
				"SlateCore",
				"InputCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Json",
				"JsonUtilities"
			}
		);

		// Only enable in Development and Editor builds
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("WITH_DEBUG_STATE_SNAPSHOT=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_DEBUG_STATE_SNAPSHOT=0");
		}
	}
}
