#define MERCUNA_BINARY
// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.

using System.IO;
using UnrealBuildTool;

public class Mercuna : ModuleRules
{
	public Mercuna(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));

        PrivateIncludePaths.AddRange(
			new string[] {
                "Mercuna/Private",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "AIModule"
            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "Projects",
                "Slate",
				"SlateCore",
                "RenderCore",
                "RHI",
                "GameplayTasks",
                "InputCore",				
                "Navmesh",
                "NavigationSystem",
                "Landscape",
                "MercunaCore",
                "Mercuna3DNavigation",
                "Mercuna2DNavigation",
                "Mercuna3DMovement"
			}
			);
			
		SetupModulePhysicsSupport(Target);
	
        PublicDependencyModuleNames.Add("PhysicsCore");

        PublicDependencyModuleNames.Add("Chaos");
        PrivateDependencyModuleNames.Add("ChaosVehicles");

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        PublicDefinitions.AddRange(
			new string[]
			{
                "MERCUNA_INTEGRATION",
				"MER_UNREAL_ENGINE",
				"MERCUNA_3D_NAVIGATION",
				"MERCUNA_GROUND_NAVIGATION"
			}
			);
			
#if UE_5_1_OR_LATER
        SetupGameplayDebuggerSupport(Target);
#else
		if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
        }
#endif

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
#if MERCUNA_BINARY
		bPrecompile = true;
		bUsePrecompiled = true;
#endif		
    }
}
