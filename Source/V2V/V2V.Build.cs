// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class V2V : ModuleRules
{
	public V2V(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
		string ThirdPartyLibDir = Path.Combine(PluginDir, "ThirdParty", "Win64");
		PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyLibDir, "miniupnpc.lib"));
		PublicDelayLoadDLLs.Add("miniupnpc.dll");
        RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/miniupnpc.dll", "$(PluginDir)/ThirdParty/Win64/miniupnpc.dll", StagedFileType.NonUFS);
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/miniupnpc.dll", "$(PluginDir)/ThirdParty/Win64/miniupnpc.dll", StagedFileType.NonUFS);
        RuntimeDependencies.Add(Path.Combine(ThirdPartyLibDir, "miniupnpc.dll"));
        string ThirdPartyIncludeDir = Path.Combine(PluginDir, "ThirdParty", "Includes");
		PublicIncludePaths.Add(ThirdPartyIncludeDir);		
		
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "Networking", "Projects", "HTTP", "Json", "JsonUtilities", "ApplicationCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
