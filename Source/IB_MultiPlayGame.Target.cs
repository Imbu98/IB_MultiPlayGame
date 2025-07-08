// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;



public class IB_MultiPlayGameTarget : TargetRules
{
	public IB_MultiPlayGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("IB_MultiPlayGame");
		
		GlobalDefinitions.Add("UE_PROJECT_STEAMPRODUCTNAME=\"spacewar\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"spacewar\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDESC=\"Spacewar\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480"); // 실제 AppID로 교체
    }
}
