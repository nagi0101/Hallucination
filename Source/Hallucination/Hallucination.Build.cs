// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Hallucination : ModuleRules
{
	public Hallucination(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "AIModule", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
