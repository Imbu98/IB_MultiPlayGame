#include "QuestLogComponent.h"
#include "QuestComponent.h"
#include "../QuestSystem/Quest_Base.h"



UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UQuestLogComponent::AddNewQuest(FName QuestId)
{
	CurrentActiveQuests.AddUnique(QuestId);

	if (UQuestComponent* QuestComponent= GetOwner()->FindComponentByClass<UQuestComponent>())
	{
		QuestComponent->QuestID = QuestId;
		QuestComponent->GetQuestDetails();
		CurrentQuests.Add(QuestComponent);
	}
	
}


void UQuestLogComponent::CompleteQuest(FName QuestId)
{
}

bool UQuestLogComponent::QuaryActiveQuest(FName QuestId)
{
	return CurrentActiveQuests.Contains(QuestId);
}

void UQuestLogComponent::TrackQuest(FName QuestId)
{

}




