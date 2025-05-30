#include "W_QuestLog.h"
#include "../Components/QuestLogComponent.h"
#include "../Components/QuestComponent.h"
#include "../QuestSystem/QuestStructure.h"
#include "../QuestSystem/Quest_Base.h"
#include "W_QuestLogEntry.h"
#include "W_QuestLogEntry_Objectives.h"
#include "W_QuestTracker.h"
#include "../DefineDelegates.h"

#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"





void UW_QuestLog::NativePreConstruct()
{
	Super::NativePreConstruct();

	
}

void UW_QuestLog::NativeConstruct()
{
	
	Super::NativeConstruct();

	CreateEntryWidget();

	if (Btn_CloseWidgetButton)
	{
		Btn_CloseWidgetButton->OnClicked.Clear();
		Btn_CloseWidgetButton->OnClicked.AddDynamic(this, &UW_QuestLog::OnClickedCloseWidgetBtn);
	}

	// Set InputMode : UIOnly
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
		PlayerController->SetShowMouseCursor(true);
	}
}

void UW_QuestLog::NativeDestruct()
{
	Super::NativeDestruct();
	// Set InputMode : GameMode
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->SetShowMouseCursor(false);
	}
}

void UW_QuestLog::DisplayQuest(FName QuestID, const FActiveQuestData& ActiveQuest)
{
	UQuestComponent* QuestCompnent = GetOwningPlayer()->GetComponentByClass<UQuestComponent>();
	if (!IsValid(QuestCompnent)) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("DisplayQuest")));
	if (ActiveQuest.QuestID.IsNone()) return;

	CurrentActiveQuestData = ActiveQuest;
	
	WS_WidgetSwitcher->SetActiveWidgetIndex(1);
	if (!IsValid(VerticalBox_Objectives)) return;
	
	VerticalBox_Objectives->ClearChildren();
	

	if (DT_QuestDataTable)
	{
		if (FQuestDetails* QuestDetails = DT_QuestDataTable->FindRow<FQuestDetails>(QuestID, TEXT("")))
		{
			FStageDetails StageDetail = QuestDetails->Stages[0];
			
			if (TextBlock_QuestName)
			{
				TextBlock_QuestName->SetText(QuestDetails->QuestName);
			}
			if (TextBlock_QuestDescription)
			{
				TextBlock_QuestDescription->SetText(QuestDetails->LogDescription);
			}
			if (TextBlock_StageDescription&& !StageDetail.Description.IsEmpty())
			{
				TextBlock_StageDescription->SetText(StageDetail.Description);
			}
			for (FObjectiveDetails ObjectiveDetails : CurrentActiveQuestData.CurrentStageDetails.Objectives)
			{
				if (WBP_QuestLogEntry_Objectives = CreateWidget<UW_QuestLogEntry_Objectives>(this, WBP_QuestLogEntry_ObjectivesClass))
				{
					WBP_QuestLogEntry_Objectives->ActiveQuestData = CurrentActiveQuestData;
					WBP_QuestLogEntry_Objectives->ObjectiveData = ObjectiveDetails;
					WBP_QuestLogEntry_Objectives->QuestActor = QuestCompnent;

					if (VerticalBox_Objectives)
					{
						VerticalBox_Objectives->AddChildToVerticalBox(WBP_QuestLogEntry_Objectives);
					}
				}
			}

		}
	}
}

void UW_QuestLog::OnClickedCloseWidgetBtn()
{
	this->RemoveFromParent();
}

void UW_QuestLog::CreateEntryWidget()
{
	if (!IsValid(DT_QuestDataTable)) return;

	if (IB_RPGPlayerController==nullptr) return;


	if (UQuestLogComponent* QuestCompnent = GetOwningPlayer()->GetComponentByClass<UQuestLogComponent>())
	{
		// 퀘스트로그컴포넌트에 퀘스트들로 entrywidget을 만든다
		for(FActiveQuestData ActiveQuestData: QuestCompnent->CurrentQuests)
		{
			WBP_QuestLogEntry = CreateWidget<UW_QuestLogEntry>(GetOwningPlayer(), WBP_QuestLogEntryClass);
			{
				if (WBP_QuestLogEntry)
				{
					WBP_QuestLogEntry->QuestSelectedDelegate.Clear();
					WBP_QuestLogEntry->QuestSelectedDelegate.AddUObject(this, &UW_QuestLog::OnQuestSelected);
					WBP_QuestLogEntry->QuestTrackDelegate.Clear();
					WBP_QuestLogEntry->QuestTrackDelegate.AddUObject(this, &UW_QuestLog::OnTracked);
					
					WBP_QuestLogEntry->QuestID = ActiveQuestData.QuestID;
					WBP_QuestLogEntry->ActiveQuestData = ActiveQuestData;
					WBP_QuestLogEntry->IB_RPGPlayerController = IB_RPGPlayerController;
					WBP_QuestLogEntry->BindingQuestCompletedDelegate();

					if (FQuestDetails* QuestDetails = DT_QuestDataTable->FindRow<FQuestDetails>(ActiveQuestData.QuestID, TEXT("")))
					{
						if (QuestDetails->IsMainQuest) // MainQuest
						{
							if (SB_MainQuests)
							{
								SB_MainQuests->AddChild(WBP_QuestLogEntry);

							}
						}
						else // SideQuest
						{
							if (SB_SideQuests)
							{
								SB_SideQuests->AddChild(WBP_QuestLogEntry);
							}
						}
						
					}

				}
			}
		}
	}
}

void UW_QuestLog::OnTracked(const FActiveQuestData& ActiveQuest)
{
	if (ActiveQuest.QuestID.IsNone()) return;

	if(UQuestComponent* QuestComponent = GetOwningPlayer()->GetComponentByClass<UQuestComponent>())
	{
		if (UQuestLogComponent* QuestLogCompnent = GetOwningPlayer()->GetComponentByClass<UQuestLogComponent>())
		{
			// 퀘스트로그컴포넌트에 퀘스트들로 entrywidget을 만든다
			for (FActiveQuestData ActiveQuestData : QuestLogCompnent->CurrentQuests)
			{
				if (WBP_QuestTrackerClass)
				{
					if (WBP_QuestTracker)
					{
						if (WBP_QuestTracker->IsInViewport())
						{
							//WBP_QuestTracker->Update(Quest);
							WBP_QuestTracker->RemoveFromParent();
						}
						else if (!WBP_QuestTracker->IsInViewport())
						{
							if (WBP_QuestTracker = CreateWidget<UW_QuestTracker>(this, WBP_QuestTrackerClass))
							{
								WBP_QuestTracker->QuestComponent = QuestComponent;
								WBP_QuestTracker->ActiveQuestData = ActiveQuestData;
								if (IB_RPGPlayerController)
								{
									WBP_QuestTracker->IB_RPGPlayerController = IB_RPGPlayerController;
								}
								WBP_QuestTracker->AddToViewport(-1);
							}
						}
					}
					else
					{
						if (WBP_QuestTracker = CreateWidget<UW_QuestTracker>(this, WBP_QuestTrackerClass))
						{
							WBP_QuestTracker->QuestComponent = QuestComponent;
							WBP_QuestTracker->ActiveQuestData = ActiveQuestData;
							if (IB_RPGPlayerController)
							{
								WBP_QuestTracker->IB_RPGPlayerController = IB_RPGPlayerController;
							}
							WBP_QuestTracker->AddToViewport(-1);
						}
					}
				}
			}

		}
	}
}

void UW_QuestLog::OnQuestSelected(FName QuestID, const FActiveQuestData& ActiveQuest)
{
	DisplayQuest(QuestID, ActiveQuest);
}