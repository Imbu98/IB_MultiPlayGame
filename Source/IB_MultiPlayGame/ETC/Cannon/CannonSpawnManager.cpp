#include "CannonSpawnManager.h"
#include "Cannon.h"
#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

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



void ACannonSpawnManager::SpawnOwnedCannon(AIB_RPGPlayerController* IB_PlayerController)
{
	if (!HasAuthority()) return;

	if (CannonPawn && IB_PlayerController)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParameters.Owner = IB_PlayerController;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetRotation(GetActorRotation().Quaternion());

		if (ACannon* OwningSpawnedCannon = GetWorld()->SpawnActor<ACannon>(CannonPawn, SpawnTransform, ActorSpawnParameters))
		{
			if (IB_PlayerController)
			{
				IB_PlayerController->OwningCannon = OwningSpawnedCannon;
				OwningSpawnedCannon->SetNetUpdateFrequency(100.f);
				OwningSpawnedCannon->SetMinNetUpdateFrequency(50.f);
				OwningSpawnedCannon->SetOwner(IB_PlayerController);
				OwningSpawnedCannon->SetReplicates(true);
				OwningSpawnedCannon->AutoPossessPlayer = EAutoReceiveInput::Disabled;
				OwningSpawnedCannon->AutoPossessAI= EAutoPossessAI::Disabled;
				OwningSpawnedCannon->bOnlyRelevantToOwner = true;

				if (!OwningSpawnedCannon->GetOwner())
				{
					UE_LOG(LogTemp, Error, TEXT("Cannon has NO owner on server!"));
				}
			}
		}
	}
}


