// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IB_MultiPlayGame : ModuleRules
{
	public IB_MultiPlayGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","GameplayAbilities","GameplayTasks","GameplayTags","UMG", "AIModule", "Niagara", "OnlineSubsystem", "Networking", "Sockets","OnlineSubsystemUtils" });

        PublicDefinitions.Add("ALLOW_UDP_MESSAGING_SHIPPING=1");
        
        PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
	        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        }
		
    }
}
