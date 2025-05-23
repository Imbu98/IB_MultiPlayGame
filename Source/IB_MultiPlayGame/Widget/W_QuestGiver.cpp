#include "W_QuestGiver.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/QuestLogComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "W_QuestLogEntry_Objectives.h"

void UW_QuestGiver::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UW_QuestGiver::NativeConstruct()
{
	Super::NativeConstruct();

	SetWidgetProPerty();

	//Delegate button 
	if (Btn_DeclineButton)
	{
		Btn_DeclineButton->OnClicked.Clear();
		Btn_DeclineButton->OnClicked.AddDynamic(this, &UW_QuestGiver::OnClickDeclineButton);
	}
	if (Btn_AcceptButton)
	{
		Btn_AcceptButton->OnClicked.Clear();
		Btn_AcceptButton->OnClicked.AddDynamic(this, &UW_QuestGiver::OnClickAcceptButton);
	}

	// SetInputMode
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController,Btn_AcceptButton);
		PlayerController->SetShowMouseCursor(true);
	}

}

void UW_QuestGiver::NativeDestruct()
{
	Super::NativeDestruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->SetShowMouseCursor(false);
	}
	
}

void UW_QuestGiver::OnClickDeclineButton()
{
	this->RemoveFromParent();
}

void UW_QuestGiver::OnClickAcceptButton()
{
	if (UQuestLogComponent* QuestLogComponent= GetOwningPlayer()->GetComponentByClass<UQuestLogComponent>())
	{
		QuestLogComponent->AddNewQuest(QuestID);
		this->RemoveFromParent();
	}
}

void UW_QuestGiver::SetWidgetProPerty()
{
	if (TextBlock_QuestName)
	{
		TextBlock_QuestName->SetText(QuestDetails.QuestName);
	}
	if (TextBlock_QuestDescription)
	{
		TextBlock_QuestDescription->SetText(QuestDetails.LogDescription);
	}
	if (TextBlock_StageDescription && QuestDetails.Stages.Num() > 0)
	{
		TextBlock_StageDescription->SetText(QuestDetails.Stages[0].Description);
	}

	for (FObjectiveDetails ObjectiveDetails : QuestDetails.Stages[0].Objectives)
	{
		if (WBP_QuestLogEntry_Objectives = CreateWidget<UW_QuestLogEntry_Objectives>(this, WBP_QuestLogEntry_ObjectivesClass))
		{
			WBP_QuestLogEntry_Objectives->ObjectiveData = ObjectiveDetails;
			
			if (VerticalBox_Objectives)
			{
				VerticalBox_Objectives->AddChildToVerticalBox(WBP_QuestLogEntry_Objectives);
			}

		}
	}
}
