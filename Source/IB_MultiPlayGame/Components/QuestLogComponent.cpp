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

	IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(GetOwner());
	
}

void UQuestLogComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UQuestLogComponent, CurrentActiveQuests);
	DOREPLIFETIME(UQuestLogComponent, IsAlreadyOnQuest);
	DOREPLIFETIME(UQuestLogComponent, CurrentQuests);
}


//On Client
void UQuestLogComponent::AddNewQuest(FName QuestId)
{
	if (!GetOwner()->HasAuthority())
	{
		CurrentActiveQuests.AddUnique(QuestId);

		if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
		{
			for (int i = 0;i < QuestComponent->MaxQuestAcceptable;i++)
			{
				if (QuestComponent->CurrentQuests[i].QuestID.IsNone())
				{
					QuestComponent->CurrentQuests[i].QuestID = QuestId;
					QuestComponent->GetQuestDetails();
					CurrentQuests.Add(QuestComponent->CurrentQuests[i]);
					break;
				}
			}
		}
		ServerAddNewQuest(QuestId);
		return;
	}
}

// On Server
void UQuestLogComponent::ServerAddNewQuest_Implementation(FName QuestId)
{
	CurrentActiveQuests.AddUnique(QuestId);

	if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
	{
		for (int i = 0;i < QuestComponent->MaxQuestAcceptable;i++)
		{
			if (QuestComponent->CurrentQuests[i].QuestID.IsNone())
			{
				QuestComponent->CurrentQuests[i].QuestID = QuestId;
				QuestComponent->GetQuestDetails();
				CurrentQuests.Add(QuestComponent->CurrentQuests[i]);
				return;
			}
		}
	}
}

// on server
void UQuestLogComponent::CompleteQuest(const FName& QuestId)
{
	
	if (QuestId.IsNone()&& !IsValid(IB_RPGPlayerController)) return;

	CompletedQuests.AddUnique(QuestId);
	CurrentActiveQuests.Remove(QuestId);

	RemoveCurrentQuest(QuestId);

	if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
	{
		if (GetOwner()->HasAuthority())
		{
			ClientCompleteQuest(QuestId, QuestComponent->CurrentQuests);
			return;
		}
	}
	

}

void UQuestLogComponent::ClientCompleteQuest_Implementation(const FName& QuestId,const TArray<FActiveQuestData>& ActiveQuests)
{
	if (QuestId.IsNone() && !IsValid(IB_RPGPlayerController)) return;

	CompletedQuests.AddUnique(QuestId);
	CurrentActiveQuests.Remove(QuestId);

	OnQuestCompletedDeleteTrack.Broadcast(GetQuestActor(QuestId).GetValue());
	OnQuestCompletedDeleteLogEntry.Broadcast(GetQuestActor(QuestId).GetValue());

	if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
	{
		QuestComponent->CurrentQuests = ActiveQuests;
	}
	
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

TOptional<FActiveQuestData> UQuestLogComponent::GetQuestActor(FName QuestId)
{
	if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
	{
		if (QuestComponent->CurrentQuests.IsEmpty()) return TOptional<FActiveQuestData>();

		for (FActiveQuestData& ActiveQuestData : QuestComponent->CurrentQuests)
		{
			if (ActiveQuestData.QuestID == QuestId)
			{
				return ActiveQuestData;
			}
		}
		return TOptional<FActiveQuestData>();
	}
	return TOptional<FActiveQuestData>();
}

void UQuestLogComponent::TurnInQuest(const FName& QuestId)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerTurnInQuest(QuestId);
	}
}

void UQuestLogComponent::ServerTurnInQuest_Implementation(const FName& QuestId)
{
	if (QuestId.IsNone()) return;

	if (UInventoryComponent* InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>())
	{
		FActiveQuestData ActiveQuestData = GetQuestActor(QuestId).GetValue();

		for (FItemRewards ItemRewards : ActiveQuestData.CurrentStageDetails.ItemRewards)
		{
			FGameplayTag ItemTag = ItemRewards.ItemTag;
			int32 Quantity = ItemRewards.ItemQuantity;

			InventoryComponent->AddItem(ItemTag, Quantity);
		}
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("XP : %d"), ActiveQuestData.CurrentStageDetails.XPReward));
		CompleteQuest(QuestId);
	}
}

void UQuestLogComponent::RemoveCurrentQuest(const FName& QuestId)
{
	TArray<FActiveQuestData> ToRemove;

	if (UQuestComponent* QuestComponent = GetOwner()->FindComponentByClass<UQuestComponent>())
	{

		// 1. 조건에 맞는 QuestComponent를 임시 배열에 저장
		for (FActiveQuestData& ActiveQuestData : QuestComponent->CurrentQuests)
		{
			if (!ActiveQuestData.QuestID.IsNone() && ActiveQuestData.QuestID == QuestId)
			{
				ToRemove.Add(ActiveQuestData);
			}
		}

		// 2. 임시 배열에 있는 요소들을 CurrentQuests에서 제거
		for (FActiveQuestData& ActiveQuestData : ToRemove)
		{
			QuestComponent->CurrentQuests.Remove(ActiveQuestData);
		}
	}
}


