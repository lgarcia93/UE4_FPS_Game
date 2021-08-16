// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TacticalShooter : ModuleRules
{
	public TacticalShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemSteam" });
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bUseUnity = true;
	}
}
