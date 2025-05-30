#include "W_QuestLogEntry.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../QuestSystem/QuestStructure.h"
#include "../Components/QuestLogComponent.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UW_QuestLogEntry::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!IsValid(DT_QuestData)) return;
	
	if (FQuestDetails* QuestDetails = DT_QuestData->FindRow<FQuestDetails>(QuestID, (TEXT(""))))
	{
		if (Textblock_QuestName)
		{
			Textblock_QuestName->SetText(QuestDetails->QuestName);
		}
	}
}

void UW_QuestLogEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_QuestLogEntry)
	{
		Btn_QuestLogEntry->OnClicked.Clear();
		Btn_QuestLogEntry->OnClicked.AddDynamic(this, &UW_QuestLogEntry::OnClickedQuestLogEntryButton);
	}
	if (Btn_QuestTrack)
	{
		Btn_QuestTrack->OnClicked.Clear();
		Btn_QuestTrack->OnClicked.AddDynamic(this, &UW_QuestLogEntry::OnClickedQuestTrackButton);
	}
	
}

void UW_QuestLogEntry::OnClickedQuestLogEntryButton()
{
	QuestSelectedDelegate.Broadcast(QuestID, ActiveQuestData);
}

void UW_QuestLogEntry::OnClickedQuestTrackButton()
{
	QuestTrackDelegate.Broadcast(ActiveQuestData);
}

void UW_QuestLogEntry::BindingQuestCompletedDelegate()
{
	if (IB_RPGPlayerController)
	{
		if (UQuestLogComponent* QuestLogComponent = IB_RPGPlayerController->FindComponentByClass<UQuestLogComponent>())
		{
			QuestLogComponent->OnQuestCompletedDeleteTrack.Clear();
			QuestLogComponent->OnQuestCompletedDeleteTrack.AddUObject(this, &UW_QuestLogEntry::QuestCompleted);
		}
	}
}

void UW_QuestLogEntry::QuestCompleted(const FActiveQuestData& ActiveQuest)
{
	if (!ActiveQuest.QuestID.IsNone() &&ActiveQuestData.QuestID == ActiveQuest.QuestID)
	{
		this->RemoveFromParent();
	}
}
