#include "CannonSpawnManager.h"
#include "Cannon.h"
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



void ACannonSpawnManager::SpawnOwnedCannon(AIB_RPGPlayerController* IB_PlayerController)
{
	if (!HasAuthority()) return;

	if (BP_CannonPawn && IB_PlayerController&&AIClass)
	{
		FActorSpawnParameters CannonSpawnParameters;
		CannonSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CannonSpawnParameters.Owner = IB_PlayerController;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetRotation(GetActorRotation().Quaternion());

		if (ACannon* OwningSpawnedCannon = GetWorld()->SpawnActor<ACannon>(BP_CannonPawn, SpawnTransform, CannonSpawnParameters))
		{

			OwningSpawnedCannon->SetNetUpdateFrequency(100.f);
			OwningSpawnedCannon->SetMinNetUpdateFrequency(50.f);
			OwningSpawnedCannon->AutoPossessPlayer = EAutoReceiveInput::Disabled;
			OwningSpawnedCannon->AutoPossessAI = EAutoPossessAI::Disabled;
			OwningSpawnedCannon->SetOwner(IB_PlayerController);
			OwningSpawnedCannon->SetReplicates(true);
			OwningSpawnedCannon->bNetUseOwnerRelevancy = true;

			FActorSpawnParameters AIControllerSpawnParameters;
			AIControllerSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AIClass, AIControllerSpawnParameters))
			{
				UE_LOG(LogTemp, Warning, TEXT("AIController spawned: %s"), *AIController->GetName());
				AIController->SetNetUpdateFrequency(100.f);
				AIController->SetMinNetUpdateFrequency(50.f);
				AIController->SetReplicates(true);
				AIController->Possess(OwningSpawnedCannon);

				if (!OwningSpawnedCannon->GetOwner())
				{
					UE_LOG(LogTemp, Error, TEXT("Cannon has NO owner on server!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AIController spawn FAILED!"));
				return;
			}
		}
	}
}


