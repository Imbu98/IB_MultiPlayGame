#include "W_QuestTracker.h"
#include "../QuestSystem/QuestStructure.h"
#include "../Components/QuestComponent.h"
#include "../Widget/W_QuestLogEntry_Objectives.h"
#include "../Widget/W_QuestLog.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/QuestLogComponent.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"


void UW_QuestTracker::NativeConstruct()
{
	if (ActiveQuestData.QuestID.IsNone()) return;

	if(!IsValid(QuestComponent)) return;

	QuestComponent->OnObjectiveHeardDelegate.Clear();
	QuestComponent->OnObjectiveHeardDelegate.AddUObject(this, &UW_QuestTracker::OnObjectiveHeard);

	if (Textblock_QuestName)
	{
		Textblock_QuestName->SetText(ActiveQuestData.QuestDetails.QuestName);
	}
	for (FObjectiveDetails ObjectiveDetails : ActiveQuestData.QuestDetails.Stages[0].Objectives)
	{
			if (WBP_QuestLogEntry_Objectives = CreateWidget<UW_QuestLogEntry_Objectives>(this, WBP_QuestLogEntry_ObjectivesClass))
			{
				WBP_QuestLogEntry_Objectives->ObjectiveData = ObjectiveDetails;
				WBP_QuestLogEntry_Objectives->QuestActor = QuestComponent;
				WBP_QuestLogEntry_Objectives->ActiveQuestData = ActiveQuestData;

				if (VerticalBox_Objectives)
				{
					VerticalBox_Objectives->AddChildToVerticalBox(WBP_QuestLogEntry_Objectives);
				}
			}
	 }
	if (IB_RPGPlayerController)
	{
		if (UQuestLogComponent* QuestLogComponent = IB_RPGPlayerController->FindComponentByClass<UQuestLogComponent>())
		{
			QuestLogComponent->OnQuestCompletedDeleteTrack.Clear();
			QuestLogComponent->OnQuestCompletedDeleteTrack.AddUObject(this, &UW_QuestTracker::QuestCompleted);
		}
	}
}

void UW_QuestTracker::Update(UQuestComponent* Quest)
{
	if (!IsValid(VerticalBox_Objectives)) return;
	if (!IsValid(Quest)) return;

	QuestComponent = Quest;

	if (!IsValid(QuestComponent)) return;

	VerticalBox_Objectives->ClearChildren();

	QuestComponent->OnObjectiveHeardDelegate.Clear();
	QuestComponent->OnObjectiveHeardDelegate.AddUObject(this, &UW_QuestTracker::OnObjectiveHeard);

		if (Textblock_QuestName)
		{
			Textblock_QuestName->SetText(ActiveQuestData.QuestDetails.QuestName);
		}

		for (FObjectiveDetails ObjectiveDetails : ActiveQuestData.QuestDetails.Stages[0].Objectives)
		{
			if (WBP_QuestLogEntry_Objectives = CreateWidget<UW_QuestLogEntry_Objectives>(this, WBP_QuestLogEntry_ObjectivesClass))
			{
				WBP_QuestLogEntry_Objectives->ObjectiveData = ObjectiveDetails;
				WBP_QuestLogEntry_Objectives->QuestActor = QuestComponent;
				WBP_QuestLogEntry_Objectives->ActiveQuestData = ActiveQuestData;

				
					VerticalBox_Objectives->AddChildToVerticalBox(WBP_QuestLogEntry_Objectives);
				
			}
		}
}

void UW_QuestTracker::QuestCompleted(const FActiveQuestData& ActiveQuest)
{
	if (!ActiveQuest.QuestID.IsNone() && ActiveQuestData.QuestID == ActiveQuest.QuestID)
	{
		this->RemoveFromParent();
	}
}

void UW_QuestTracker::OnObjectiveHeard()
{
	Update(QuestComponent);
}


