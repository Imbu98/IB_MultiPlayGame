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
	if (!IsValid(QuestComponent)) return;

	if (Textblock_QuestName)
	{
		Textblock_QuestName->SetText(QuestComponent->QuestDetails.QuestName);
	}
	for (FObjectiveDetails ObjectiveDetails : QuestComponent->QuestDetails.Stages[0].Objectives)
	{
			if (WBP_QuestLogEntry_Objectives = CreateWidget<UW_QuestLogEntry_Objectives>(this, WBP_QuestLogEntry_ObjectivesClass))
			{
				WBP_QuestLogEntry_Objectives->ObjectiveData = ObjectiveDetails;
				WBP_QuestLogEntry_Objectives->QuestActor = QuestComponent;

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
	if (!IsValid(Quest)) return;

	QuestComponent = Quest;

	if (!IsValid(QuestComponent)) return;

		if (Textblock_QuestName)
		{
			Textblock_QuestName->SetText(QuestComponent->QuestDetails.QuestName);
		}

		for (FObjectiveDetails ObjectiveDetails : QuestComponent->QuestDetails.Stages[0].Objectives)
		{
			if (WBP_QuestLogEntry_Objectives = CreateWidget<UW_QuestLogEntry_Objectives>(this, WBP_QuestLogEntry_ObjectivesClass))
			{
				WBP_QuestLogEntry_Objectives->ObjectiveData = ObjectiveDetails;
				WBP_QuestLogEntry_Objectives->QuestActor = QuestComponent;

				if (VerticalBox_Objectives)
				{
					VerticalBox_Objectives->AddChildToVerticalBox(WBP_QuestLogEntry_Objectives);
				}
			}
		}

}

void UW_QuestTracker::QuestCompleted(UQuestComponent* Quest)
{
	if (IsValid(QuestComponent) && Quest == QuestComponent)
	{
		this->RemoveFromParent();
	}
}


