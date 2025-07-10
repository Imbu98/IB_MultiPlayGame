// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IB_MultiPlayGame : ModuleRules
{
	public IB_MultiPlayGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","GameplayAbilities","GameplayTasks","GameplayTags","UMG", "AIModule", "Niagara", "Networking", "Sockets","OnlineSubsystemUtils", "OnlineSubsystem" });

		PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils" });

        PublicDefinitions.Add("ALLOW_UDP_MESSAGING_SHIPPING=1");
        
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        
		
    }
}
