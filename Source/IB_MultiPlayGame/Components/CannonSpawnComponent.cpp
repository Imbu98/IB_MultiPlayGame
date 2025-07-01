#include "CannonSpawnComponent.h"

#include "IB_MultiPlayGame/ETC/Cannon/CannonActor.h"
#include "IB_MultiPlayGame/ETC/Cannon/CannonPawn.h"
#include "IB_MultiPlayGame/ETC/Cannon/CannonSpawnManager.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Kismet/GameplayStatics.h"


UCannonSpawnComponent::UCannonSpawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCannonSpawnComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCannonSpawnComponent::SpawnOwnedCannonActor(AIB_RPGPlayerController* IB_PlayerController)
{
	if (!IsValid(GetOwner())) return;
	
	if (!GetOwner()->HasAuthority())
	{
		ServerSpawnOwnedCannonActor(IB_PlayerController);
		return;
	}

	if (BP_CannonActor && IB_PlayerController) //BP_CannonActor추가
	{
		if(ACannonSpawnManager* CannonSpawnManager = Cast<ACannonSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACannonSpawnManager::StaticClass())))
		{
			FActorSpawnParameters CannonSpawnParameters;
			CannonSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			CannonSpawnParameters.Owner = IB_PlayerController;
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(CannonSpawnManager->GetActorLocation());
			SpawnTransform.SetRotation(CannonSpawnManager->GetActorRotation().Quaternion());
			if (ACannonActor* OwningSpawnedCannon = GetWorld()->SpawnActor<ACannonActor>(BP_CannonActor, SpawnTransform, CannonSpawnParameters))
			{
				OwningSpawnedCannon->SetNetUpdateFrequency(100.f);
				OwningSpawnedCannon->SetMinNetUpdateFrequency(50.f);
				OwningSpawnedCannon->SetOwner(IB_PlayerController);
				OwningSpawnedCannon->SetReplicates(true);
				OwningSpawnedCannon->bNetUseOwnerRelevancy = true;
				
			}
		}
	}
}

void UCannonSpawnComponent::ServerSpawnOwnedCannonActor_Implementation(AIB_RPGPlayerController* IB_PlayerController)
{
	if (!IsValid(IB_PlayerController)) return;
	
	if (IB_PlayerController->HasAuthority())
	{
		SpawnOwnedCannonActor(IB_PlayerController);
	}
}

void UCannonSpawnComponent::SpawnOwnedCannonPawn(AIB_RPGPlayerController* IB_PlayerController)
{
	if (!GetOwner()->HasAuthority()) return;
	
	if (BP_CannonPawn && IB_PlayerController) //BP_CannonActor추가
	{
		if(ACannonSpawnManager* CannonSpawnManager = Cast<ACannonSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACannonSpawnManager::StaticClass())))
		{
			FActorSpawnParameters CannonSpawnParameters;
			CannonSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			CannonSpawnParameters.Owner = IB_PlayerController;
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(CannonSpawnManager->GetActorLocation());
			SpawnTransform.SetRotation(CannonSpawnManager->GetActorRotation().Quaternion());
			if (ACannonPawn* OwningSpawnedCannonPawn = GetWorld()->SpawnActor<ACannonPawn>(BP_CannonPawn, SpawnTransform, CannonSpawnParameters))
			{
				OwningSpawnedCannonPawn->SetNetUpdateFrequency(100.f);
				OwningSpawnedCannonPawn->SetMinNetUpdateFrequency(50.f);
				OwningSpawnedCannonPawn->SetOwner(IB_PlayerController);
				OwningSpawnedCannonPawn->SetReplicates(true);
				//OwningSpawnedCannonPawn->ClientSetCannonInfo(IB_PlayerController,IB_PlayerController->GetPawn());
				OwningSpawnedCannonPawn->PossessedBy(IB_PlayerController);
				IB_PlayerController->OwningCannon =OwningSpawnedCannonPawn;
				IB_PlayerController->ServerSwitchController();
				OwningSpawnedCannonPawn->bNetUseOwnerRelevancy = true;
			}
		}
	}
}



