#define MERCUNA_BINARY
// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.

using UnrealBuildTool;

public class MercunaCore : ModuleRules
{
	public MercunaCore(ReadOnlyTargetRules Target) : base(Target)
    {
		
        PrivateIncludePaths.AddRange(
			new string[] {
				"MercunaCore/Private"
			}
			);

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
			);

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
        {
            PublicDefinitions.Add("MER_DEBUG_BUILD");
        }

        PublicDefinitions.Add("MER_UNREAL_ENGINE");

        PCHUsage = PCHUsageMode.NoSharedPCHs;
        PrivatePCHHeaderFile = "Private/MercunaCorePrivatePCH.h";
        ShadowVariableWarningLevel = WarningLevel.Error;
#if MERCUNA_BINARY
		bPrecompile = true;
		bUsePrecompiled = true;
#endif
    }
}
