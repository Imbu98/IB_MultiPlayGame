#include "W_Inventory.h"
#include "../Components/InventoryComponent.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "W_ItemRow.h"
#include "W_InventorySlot.h"
#include "../Inventory/ItemTypes.h"
#include "../DefineDelegates.h"
#include "../WidgetController/InventoryWidgetController.h"


void UW_Inventory::NativeDestruct()
{
	if (InventoryWidgetController)
	{
		InventoryWidgetController->InventoryItemDelegate.Clear();
		InventoryWidgetController->InventoryBroadCastComplete.Clear();
		InventoryWidgetController->ScrollBoxResetDelegate.Clear();
	}
}




void UW_Inventory::BindInventoryItemDelegate()
{
	if (InventoryWidgetController)
	{
		// Tab을 눌러 Invetory위젯을 만들기 전까지는 ItemRecieve델리게이트가 선언되지 않는다 나중에 어떻게 할지 생각해보자
		InventoryWidgetController->InventoryItemDelegate.AddDynamic(this, &UW_Inventory::InventoryItemRecieved);
		
		InventoryWidgetController->InventoryBroadCastComplete.AddDynamic(this, &UW_Inventory::InventoryBroadcastComplete);
		
		InventoryWidgetController->ScrollBoxResetDelegate.AddDynamic(this, &UW_Inventory::OnScrollBoxReset);

	}
}

void UW_Inventory::InventoryItemRecieved(const FMasterItemDefinition& Item)
{
	HandleInventoryItemRecieved(Item);
}

void UW_Inventory::InventoryBroadcastComplete()
{
	for (UW_InventorySlot* Widgets : ActiveItemWidgets)
	{
		Widgets->OnClickedActionButtonDelegate.AddUObject(this, &UW_Inventory::OnActionButtonClicked);
	}
	
}

void UW_Inventory::OnScrollBoxReset()
{
	if (WB_InventoryContents)
	{
		WB_InventoryContents->ClearChildren();
		for (UW_InventorySlot* Widgets : ActiveItemWidgets)
		{
			Widgets->OnClickedActionButtonDelegate.Clear();
		}
		ActiveItemWidgets.Empty();
	}
}

void UW_Inventory::OnActionButtonClicked(const FMasterItemDefinition& Item)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->UseItem(Item.ItemTag,1);
	}
}

void UW_Inventory::HandleInventoryItemRecieved(const FMasterItemDefinition& Item)
{
	MakeItemRowWidget(Item);
}

void UW_Inventory::MakeItemRowWidget(const FMasterItemDefinition& Item)
{

	WBP_InventorySlot = CreateWidget<UW_InventorySlot>(this, WBP_InventorySlotClass);
	if (WBP_InventorySlot)
	{
		WBP_InventorySlot->SetItemImage(Item.Icon);
		WBP_InventorySlot->SetQuiantityText(Item.ItemQuantity);
		WBP_InventorySlot->Item = Item;
		WBP_InventorySlot->SlotIndex = ActiveItemWidgets.Num();
		
		
		if (WB_InventoryContents)
		{
			WB_InventoryContents->AddChild(WBP_InventorySlot);
			ActiveItemWidgets.Add(WBP_InventorySlot);
		}
	}
}
