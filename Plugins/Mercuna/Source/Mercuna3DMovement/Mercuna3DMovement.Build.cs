#define MERCUNA_BINARY
// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.

using System.IO;
using UnrealBuildTool;

public class Mercuna3DMovement : ModuleRules
{
	public Mercuna3DMovement(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));

        PrivateIncludePaths.AddRange(
			new string[] {
				"Mercuna3DMovement/Private",
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
				"Slate",
				"SlateCore",
			}
			);

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Unreal headers are C++20 warning-free from 5.2
		//if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 2) CppStandard = CppStandardVersion.Cpp20;

#if MERCUNA_BINARY
		bPrecompile = true;
		bUsePrecompiled = true;
#endif		
    }
}
