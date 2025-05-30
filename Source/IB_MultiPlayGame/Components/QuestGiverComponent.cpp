#include "QuestGiverComponent.h"
#include "../Character/IB_MainChar.h"
#include "../Character/IB_NPCBase.h"
#include "../IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/QuestLogComponent.h"
#include "../Components/QuestComponent.h"
#include "../QuestSystem/QuestStructure.h"
#include "../Widget/W_QuestGiver.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



UQuestGiverComponent::UQuestGiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UQuestGiverComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UQuestGiverComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


// On server
FString UQuestGiverComponent::InteractWith_Implementation(APlayerController* CharacterController)
{
	if (!GetOwner()->HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("NPC Has No Authority"));
		return"";
	}

	if (!QuestRowHandle.DataTable)
	{
		return "";
	}
	if (QuestRowHandle.RowName.IsNone())
	{
		return "";
	}


	FQuestDetails* QuestDetails = QuestRowHandle.DataTable->FindRow<FQuestDetails>(QuestRowHandle.RowName, TEXT(""));
	if (!QuestDetails)
	{
		return "";
	}

	

	AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(CharacterController);
	if (IB_RPGPlayerController)
	{
		
		if (IB_RPGPlayerController->QuestLogComponent)
		{
			
			if (!IB_RPGPlayerController->QuestLogComponent->QuaryActiveQuest(QuestRowHandle.RowName))
			{
				IB_RPGPlayerController->ClientDisplayQuest(*QuestDetails, QuestRowHandle.RowName);
				return UKismetSystemLibrary::GetDisplayName(GetOwner());
			}
			else
			{
				FActiveQuestData ActiveQuestData = IB_RPGPlayerController->QuestLogComponent->GetQuestActor(QuestRowHandle.RowName).GetValue();
				if (!ActiveQuestData.QuestID.IsNone())
				{
					if (ActiveQuestData.IsCompleted)
					{
						IB_RPGPlayerController->ClientDisplayRewards(*QuestDetails, QuestRowHandle.RowName);
						return UKismetSystemLibrary::GetDisplayName(GetOwner());
					}
				}
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Already on Quest"));
				return UKismetSystemLibrary::GetDisplayName(GetOwner());
			}

		}
		return "";
	}
	return "";
}