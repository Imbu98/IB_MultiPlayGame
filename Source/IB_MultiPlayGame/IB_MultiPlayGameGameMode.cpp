// Copyright Epic Games, Inc. All Rights Reserved.

#include "IB_MultiPlayGameGameMode.h"
#include "IB_MultiPlayGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIB_MultiPlayGameGameMode::AIB_MultiPlayGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
