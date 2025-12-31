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

		// Only compile in non-shipping builds
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("WITH_DEBUG_EVENT_LOG=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_DEBUG_EVENT_LOG=0");
		}
	}
}
