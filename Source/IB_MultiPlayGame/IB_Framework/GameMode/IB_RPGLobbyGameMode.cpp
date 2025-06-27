#include "IB_RPGLobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "IB_MultiPlayGame/Character/IB_MainChar.h"
#include "IB_MultiPlayGame/Components/CannonSpawnComponent.h"
#include "IB_MultiPlayGame/ETC/Cannon/CannonSpawnManager.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Kismet/GameplayStatics.h"


void AIB_RPGLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AIB_RPGLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		if (AIB_MainChar* MainChar= Cast<AIB_MainChar>(NewPlayer->GetPawn()))
		{
			MainChar->bOnlyRelevantToOwner = true;
		}
	}

	if (AIB_RPGPlayerController* PlayerController = Cast<AIB_RPGPlayerController>(NewPlayer))
	{
		
			if (UCannonSpawnComponent* CannonSpawnComponent =  PlayerController->GetCannonSpawnComponent())
			{
				CannonSpawnComponent->SpawnOwnedCannonActor(PlayerController);
			}
		
	}

}
