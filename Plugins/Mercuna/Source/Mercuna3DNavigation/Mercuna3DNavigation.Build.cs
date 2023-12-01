#define MERCUNA_BINARY
// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.

using UnrealBuildTool;

public class Mercuna3DNavigation : ModuleRules
{
	public Mercuna3DNavigation(ReadOnlyTargetRules Target) : base(Target)
    {
        
		PrivateIncludePaths.AddRange(
			new string[] {
                "Mercuna3DNavigation/Private",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"MercunaCore",
			}
			);
		
        PublicDefinitions.AddRange(
            new string[]
            {
                "MERCUNA",    // So that Octree builds in Mercuna mode.
                "MER_UNREAL_ENGINE"
            }
            );

        PCHUsage = PCHUsageMode.NoSharedPCHs;
		PrivatePCHHeaderFile = "Private/Mercuna3DNavigationPrivatePCH.h";
        ShadowVariableWarningLevel = WarningLevel.Error;

		// Unreal headers are C++20 warning-free from 5.2
		//if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 2) CppStandard = CppStandardVersion.Cpp20;

#if MERCUNA_BINARY
		bPrecompile = true;
		bUsePrecompiled = true;
#endif		
    }
}
