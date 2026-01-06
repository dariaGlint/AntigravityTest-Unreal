using UnrealBuildTool;

public class FormationSystem : ModuleRules
{
	public FormationSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule",
				"NavigationSystem"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"GameplayTasks"
			}
		);
	}
}
