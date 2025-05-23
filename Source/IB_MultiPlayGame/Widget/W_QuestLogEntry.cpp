#include "W_QuestLogEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../QuestSystem/QuestStructure.h"


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
}

void UW_QuestLogEntry::OnClickedQuestLogEntryButton()
{
	QuestSelectedDelegate.Broadcast(QuestID,QuestBase);
}
