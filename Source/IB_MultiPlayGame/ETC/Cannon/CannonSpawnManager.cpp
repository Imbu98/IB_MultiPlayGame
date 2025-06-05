#include "CannonSpawnManager.h"
#include "Cannon.h"
#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

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

void ACannonSpawnManager::ServerSpawnOwnedCannon_Implementation(AIB_RPGPlayerController* IB_PlayerController)
{
	if (CannonPawn&& IB_PlayerController)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = IB_PlayerController;
		ActorSpawnParameters.Instigator = IB_PlayerController->GetPawn();
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetRotation(GetActorRotation().Quaternion());

		if (ACannon* OwningSpawnedCannon = GetWorld()->SpawnActor<ACannon>(CannonPawn, SpawnTransform, ActorSpawnParameters))
		{
			if (IB_PlayerController)
			{
				OwningSpawnedCannon->SetOwner(IB_PlayerController);
				OwningSpawnedCannon->SetReplicates(true);
				OwningSpawnedCannon->bOnlyRelevantToOwner = true;
				OwningSpawnedCannon->NetUpdateFrequency = 100.f;
				OwningSpawnedCannon->MinNetUpdateFrequency = 50.f;
			}
		}
	}
}


