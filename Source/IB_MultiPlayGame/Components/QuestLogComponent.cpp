#include "QuestLogComponent.h"
#include "QuestComponent.h"
#include "../QuestSystem/Quest_Base.h"
#include "Net/UnrealNetwork.h"
#include "QuestGiverComponent.h"
#include "../Character/IB_MainChar.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"



UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UQuestLogComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UQuestLogComponent, CurrentActiveQuests);
	DOREPLIFETIME(UQuestLogComponent, IsAlreadyOnQuest);
}


//On Client
void UQuestLogComponent::AddNewQuest(FName QuestId)
{
	if (!GetOwner()->HasAuthority())
	{
		CurrentActiveQuests.AddUnique(QuestId);

		if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
		{
			QuestComponent->QuestID = QuestId;
			QuestComponent->GetQuestDetails();
			CurrentQuests.Add(QuestComponent);
		}
		ServerAddNewQuest(QuestId);
		return;
	}
}

void UQuestLogComponent::ServerAddNewQuest_Implementation(FName QuestId)
{
	CurrentActiveQuests.AddUnique(QuestId);

	if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
	{
		QuestComponent->QuestID = QuestId;
		QuestComponent->GetQuestDetails();
		CurrentQuests.Add(QuestComponent);
	}
}

void UQuestLogComponent::CompleteQuest(const FName& QuestId)
{
	if (QuestId.IsNone()) return;
	// for client
	CompletedQuests.AddUnique(QuestId);
	CurrentActiveQuests.Remove(QuestId);

	// for server
	if (!GetOwner()->HasAuthority())
	{
		ServerCompleteQuest(QuestId);
	}
	
}

void UQuestLogComponent::ServerCompleteQuest_Implementation(const FName& QuestId)
{
	if (QuestId.IsNone()) return;

	CompletedQuests.AddUnique(QuestId);
	CurrentActiveQuests.Remove(QuestId);
}


bool UQuestLogComponent::QuaryActiveQuest(const FName& QuestId)
{
	if (!QuestId.IsNone())
	{
		return CurrentActiveQuests.Contains(QuestId);

	}
	return false;
}

void UQuestLogComponent::TrackQuest(FName QuestId)
{

}

UQuestComponent* UQuestLogComponent::GetQuestActor(FName QuestId)
{
	if (CurrentQuests.IsEmpty()) return nullptr;

	for (UQuestComponent* QuestComponent : CurrentQuests)
	{
		if (QuestComponent->QuestID == QuestId)
		{
			return QuestComponent;
		}
	}
	return nullptr;
}

void UQuestLogComponent::TurnInQuest(const FName& QuestId)
{
	if (QuestId.IsNone()) return;

	
		if (UQuestComponent* QuestComponent = GetQuestActor(QuestId))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("XP : %d"), QuestComponent->CurrentStageDetails.XPReward));
			CompleteQuest(QuestId);
		}
}


