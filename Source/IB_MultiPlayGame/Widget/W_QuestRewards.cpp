#include "W_QuestRewards.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/QuestLogComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "W_QuestLogEntry_Objectives.h"

void UW_QuestRewards::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UW_QuestRewards::NativeConstruct()
{
	Super::NativeConstruct();

	SetWidgetProPerty();

	//Delegate button 
	if (Btn_CancleButton)
	{
		Btn_CancleButton->OnClicked.Clear();
		Btn_CancleButton->OnClicked.AddDynamic(this, &UW_QuestRewards::OnClickDeclineButton);
	}
	if (Btn_ColletRewardButton)
	{
		Btn_ColletRewardButton->OnClicked.Clear();
		Btn_ColletRewardButton->OnClicked.AddDynamic(this, &UW_QuestRewards::OnClickAcceptButton);
	}

	// SetInputMode
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, Btn_ColletRewardButton);
		PlayerController->SetShowMouseCursor(true);
	}

}

void UW_QuestRewards::NativeDestruct()
{
	Super::NativeDestruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->SetShowMouseCursor(false);
	}

}

void UW_QuestRewards::OnClickDeclineButton()
{
	this->RemoveFromParent();
}

void UW_QuestRewards::OnClickAcceptButton()
{
	if (UQuestLogComponent* QuestLogComponent = GetOwningPlayer()->GetComponentByClass<UQuestLogComponent>())
	{
		QuestLogComponent->TurnInQuest(QuestID);
		this->RemoveFromParent();
	}
}

void UW_QuestRewards::SetWidgetProPerty()
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

}
