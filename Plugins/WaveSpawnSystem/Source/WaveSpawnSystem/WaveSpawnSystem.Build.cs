// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WaveSpawnSystem : ModuleRules
{
	public WaveSpawnSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule"
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
