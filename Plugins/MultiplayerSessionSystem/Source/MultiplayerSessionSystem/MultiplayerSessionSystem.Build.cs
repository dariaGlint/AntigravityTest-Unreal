using UnrealBuildTool;

public class MultiplayerSessionSystem : ModuleRules
{
	public MultiplayerSessionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"OnlineSubsystem",
				"OnlineSubsystemUtils"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Networking",
				"Sockets"
			}
		);
	}
}
