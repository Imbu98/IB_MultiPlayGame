#include "W_QuestRewards.h"
#include "W_QuestLogEntry_Objectives.h"
#include "W_ItemInfoSlot.h"
#include "../Components/QuestLogComponent.h"
#include "../Components/InventoryComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


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

	SetInventoryRewards();

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
		//  여기부 ㅓ 서버로 바꾸자
		QuestLogComponent->ServerTurnInQuest(QuestID);
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

void UW_QuestRewards::SetInventoryRewards()
{
	if (!IsValid(InventoryComponent)) return;

	if (WrapBox_ItemRewardsBox && QuestDetails.Stages.Num() > 0)
	{
		int32 ItemRewardsNum = QuestDetails.Stages[0].ItemRewards.Num();

		for (FItemRewards ItemRewards : QuestDetails.Stages[0].ItemRewards)
		{
			if (WBP_ItemInfoSlotClass)
			{
				if (WBP_ItemInfoSlot = CreateWidget<UW_ItemInfoSlot>(this, WBP_ItemInfoSlotClass))
				{

					FGameplayTag ItemTag = ItemRewards.ItemTag;
					int32 ItemQuantity = ItemRewards.ItemQuantity;

					FMasterItemDefinition ItemData = InventoryComponent->GetItemDefinitionByTag(ItemTag);
					ItemData.ItemQuantity = ItemQuantity;

					WBP_ItemInfoSlot->Item = ItemData;

					WrapBox_ItemRewardsBox->AddChild(WBP_ItemInfoSlot);
				}
			}
		}
	}
}
