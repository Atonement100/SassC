// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SassC : ModuleRules
{
	public SassC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Public/SassC.h";
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		}

		MinFilesUsingPrecompiledHeaderOverride = 1;
		bUseUnity = false;
	}
}