#include "CannonSpawnManager.h"
#include "CannonPawn.h"
#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

ACannonSpawnManager::ACannonSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(66.f);
}


void ACannonSpawnManager::BeginPlay()
{
	Super::BeginPlay();
}






