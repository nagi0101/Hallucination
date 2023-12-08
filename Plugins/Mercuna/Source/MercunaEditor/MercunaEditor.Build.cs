#define MERCUNA_BINARY
// Copyright (C) 2016-2022 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.

using UnrealBuildTool;

public class MercunaEditor : ModuleRules
{
	public MercunaEditor(ReadOnlyTargetRules Target) : base(Target)
    {
	
		PrivateIncludePaths.AddRange(
			new string[] {
				"MercunaEditor/Private",
			}
			);
					
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		if (Target.Version.MajorVersion > 4)
		{
			PrivateDependencyModuleNames.Add("EditorFramework");
			PrivateDependencyModuleNames.Add("ToolMenus");
		}

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "EditorStyle",
                "AIModule",
				"SourceControl",
				"DeveloperToolSettings",
                "MercunaCore",
                "Mercuna"
			}
			);

        PublicDefinitions.AddRange(
			new string[]
			{
				"MER_UNREAL_ENGINE",
				"MERCUNA_3D_NAVIGATION",
				"MERCUNA_GROUND_NAVIGATION"
			}
			);

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 3)
        {
            CppStandard = CppStandardVersion.Cpp20;
        }
		
#if MERCUNA_BINARY
		bPrecompile = true;
		bUsePrecompiled = true;
#endif		
    }
}
