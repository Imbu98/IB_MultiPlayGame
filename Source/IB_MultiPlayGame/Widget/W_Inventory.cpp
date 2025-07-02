#include "W_Inventory.h"
#include "../Components/InventoryComponent.h"
#include "../Components/CombatComponent.h"
#include "W_ItemRow.h"
#include "W_InventorySlot.h"
#include "W_PlayerInfo.h"
#include "../Inventory/ItemTypes.h"
#include "../DefineDelegates.h"
#include "../WidgetController/InventoryWidgetController.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../ETC/Equippable/Weapon/WeaponBase.h"
#include "../ETC/Equippable/Armor/ArmorBase.h"

#include "kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"



void UW_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		//UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
		PlayerController->SetShowMouseCursor(true);
	}
	if (Btn_EquippableInventory)
	{
		Btn_EquippableInventory->OnClicked.Clear();
		Btn_EquippableInventory->OnClicked.AddDynamic(this,&ThisClass::OnEquippableButtonClicked);
	}
	if (Btn_ConsumableInventory)
	{
		Btn_ConsumableInventory->OnClicked.Clear();
		Btn_ConsumableInventory->OnClicked.AddDynamic(this,&ThisClass::OnComsumbableButtonClicked);
	}
	if (Btn_ETCInventory)
	{
		Btn_ETCInventory->OnClicked.Clear();
		Btn_ETCInventory->OnClicked.AddDynamic(this,&ThisClass::OnETCButtonClicked);
	}
}

void UW_Inventory::NativeDestruct()
{
	Super::NativeDestruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->SetShowMouseCursor(false);
	}

	if (InventoryWidgetController)
	{
		InventoryWidgetController->InventoryItemDelegate.Clear();
		InventoryWidgetController->InventoryBroadCastComplete.Clear();
	}
}

void UW_Inventory::BindInventoryItemDelegate()
{
	if (InventoryWidgetController)
	{
		// Tab�� ���� Invetory������ ����� �������� ItemRecieve��������Ʈ�� ������� �ʴ´� ���߿� ��� ���� �����غ���
		InventoryWidgetController->InventoryItemDelegate.AddDynamic(this, &UW_Inventory::InventoryItemRecieved);
		
		InventoryWidgetController->InventoryBroadCastComplete.AddDynamic(this, &UW_Inventory::InventoryBroadcastComplete);
		
	}
}
void UW_Inventory::InventoryBroadcastComplete()
{
	//ResetActiveWidgets(EquippableSlots);
	//ResetActiveWidgets(ConsumableSlots);
	//ResetActiveWidgets(ETCSlots);
}

void UW_Inventory::BroadcastOnclickEvent(ESlotTypes SlotType)
{
	switch (SlotType)
	{
	case ESlotTypes::Slot_Equippable:
		for (UW_InventorySlot* Widgets : EquippableSlots)
		{
			if (Widgets)
			{
				Widgets->OnClickedActionButtonDelegate.AddUObject(this, &UW_Inventory::OnActionButtonClicked);
			}
		}
		break;
	case ESlotTypes::Slot_Consumable:
		for (UW_InventorySlot* Widgets : ConsumableSlots)
		{
			if (Widgets)
			{
				Widgets->OnClickedActionButtonDelegate.AddUObject(this, &UW_Inventory::OnActionButtonClicked);
			}
		}
		break;
	case ESlotTypes::Slot_ETC:
		for (UW_InventorySlot* Widgets : ETCSlots)
		{
			if (Widgets)
			{
				Widgets->OnClickedActionButtonDelegate.AddUObject(this, &UW_Inventory::OnActionButtonClicked);
			}
		}
		break;
	default:
		break;
	}
}

void UW_Inventory::InventoryItemRecieved(const FUserInventory& PackagedInventory,const EItemTypes InventoryType)
{
	
	HandleInventoryItemRecieved(PackagedInventory,InventoryType);
}


void UW_Inventory::HandleInventoryItemRecieved(const FUserInventory& PackagedInventory,const EItemTypes InventoryType)
{

	MakeItemRowWidget(PackagedInventory,InventoryType);
}

// On client
void UW_Inventory::MakeItemRowWidget(const FUserInventory& PackagedInventory,const EItemTypes InventoryType)
{
	//Item_None이면 먼 처음 위젯을 만들 때 라는 뜻
	if (InventoryType==EItemTypes::Item_None)
	{
		InitializeSlots();
	}
	if (InventoryType==EItemTypes::Item_Equippable)
	{
		UpdateItemWidgets(PackagedInventory.EquippableInventory, ESlotTypes::Slot_Equippable,WB_EquippableInventoryContents);
	}
	if (InventoryType==EItemTypes::Item_Consumable)
	{
		UpdateItemWidgets(PackagedInventory.ConsumableInventory, ESlotTypes::Slot_Consumable,WB_ConsumableInventoryContents);
	}
	if (InventoryType==EItemTypes::Item_ETC)
	{
		UpdateItemWidgets(PackagedInventory.ETCInventory, ESlotTypes::Slot_ETC,WB_ETCInventoryContents);
	}
}

void UW_Inventory::InitializeSlots()
{
	if (WB_EquippableInventoryContents)
	{
		CreateInventorySlot(WB_EquippableInventoryContents,ESlotTypes::Slot_Equippable);
	}
	if (WB_ConsumableInventoryContents)
	{
		CreateInventorySlot(WB_ConsumableInventoryContents,ESlotTypes::Slot_Consumable);
	}
	if (WB_ETCInventoryContents)
	{
		CreateInventorySlot(WB_ETCInventoryContents,ESlotTypes::Slot_ETC);
	}
}

void UW_Inventory::UpdateItemWidgets(const FPackagedInventory& SubInventory, ESlotTypes SlotType,UWrapBox* InventoryContentsWrapBox)
{
	const int32 NumSlots = InventoryComponent->GetInventorySize();
	if (!IsValid(InventoryContentsWrapBox)) return;

	TArray<UW_InventorySlot*>* TargetSlots = nullptr;

	switch (SlotType)
	{
	case ESlotTypes::Slot_Equippable:
		TargetSlots = &EquippableSlots;
		break;
	case ESlotTypes::Slot_Consumable:
		TargetSlots = &ConsumableSlots;
		break;
	case ESlotTypes::Slot_ETC:
		TargetSlots = &ETCSlots;
		break;
	default:
		return;
	}

	if (TargetSlots)
	{
		ResetActiveWidgets(*TargetSlots);
	}
	
	InventoryContentsWrapBox->RemoveFromParent();
	
	for (int32 i = 0; i < NumSlots; i++)
	{
		UW_InventorySlot* SlotWidget = CreateWidget<UW_InventorySlot>(this, WBP_InventorySlotClass);
		if (!SlotWidget) continue;

		if (!SubInventory.ItemTags.IsValidIndex(i) ||
			!SubInventory.ItemQuantities.IsValidIndex(i) ||
			!SubInventory.ItemDefinitions.IsValidIndex(i))
		{
			SlotWidget->ClearSlot();
			continue;
		}
		if (InventoryContentsWrapBox)
		{
			InventoryContentsWrapBox->AddChild(SlotWidget);
		}

		FMasterItemDefinition StaticItemData = InventoryComponent->GetItemDefinitionByTag(SubInventory.ItemTags[i]);
		StaticItemData.ItemQuantity = SubInventory.ItemQuantities[i];
		FMasterItemDefinition ItemData = SubInventory.ItemDefinitions[i];

		SlotWidget->SetItemImage(StaticItemData.Icon);
		SlotWidget->SetQuiantityText(ItemData.ItemQuantity);
		SlotWidget->UpdateSlot(ItemData);
		SlotWidget->SlotType = SlotType;
		TargetSlots->Add(SlotWidget);

		switch (SlotType)
		{
		case ESlotTypes::Slot_Equippable:
			EquippableSlots.Add(SlotWidget);
			break;
		case ESlotTypes::Slot_Consumable:
			ConsumableSlots.Add(SlotWidget);
			break;
		case ESlotTypes::Slot_ETC:
			ETCSlots.Add(SlotWidget);
			break;
		default:
			break;
		}
	}
	BroadcastOnclickEvent(SlotType);
}

void UW_Inventory::CreateInventorySlot(UWrapBox* InventoryContentsWrapBox, ESlotTypes SlotType)
{
	if (!IsValid(InventoryComponent)) return;

	const int32 NumSlots = InventoryComponent->GetInventorySize();
	
	for (int32 i = 0; i < NumSlots; i++)
	{
		UW_InventorySlot* SlotWidget = CreateWidget<UW_InventorySlot>(this, WBP_InventorySlotClass);
		if (!SlotWidget) continue;

		SlotWidget->SlotIndex = i;
		SlotWidget->ClearSlot(); // 초기엔 빈 슬롯
		SlotWidget->SlotType = SlotType;

		if (InventoryContentsWrapBox)
		{
			InventoryContentsWrapBox->AddChild(SlotWidget);
		}
		switch (SlotType)
		{
		case ESlotTypes::Slot_Equippable:
			EquippableSlots.Add(SlotWidget);
			break;
		case ESlotTypes::Slot_Consumable:
			ConsumableSlots.Add(SlotWidget);
			break;
		case ESlotTypes::Slot_ETC:
			ETCSlots.Add(SlotWidget);
			break;
		default:
			break;
		}
		
	}
}

void UW_Inventory::ResetActiveWidgets(TArray<UW_InventorySlot*>& ActiveWidget)
{
	for (UW_InventorySlot* Widgets : ActiveWidget)
	{
		if (Widgets)
		{
			Widgets->OnClickedActionButtonDelegate.Clear();
		}
	}
	ActiveWidget.Empty();
}

void UW_Inventory::OnActionButtonClicked(const FMasterItemDefinition& Item,const float& SlotIndex)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->UseItem(Item,SlotIndex,1);
	}
}

void UW_Inventory::OnEquippableButtonClicked()
{
	
	if (WidgetSwitcher_InventorySwitcher)
	{
		WidgetSwitcher_InventorySwitcher->SetActiveWidgetIndex(0);
	}
}

void UW_Inventory::OnComsumbableButtonClicked()
{
	
	if (WidgetSwitcher_InventorySwitcher)
	{
		WidgetSwitcher_InventorySwitcher->SetActiveWidgetIndex(1);
	}
}

void UW_Inventory::OnETCButtonClicked()
{
	
	if (WidgetSwitcher_InventorySwitcher)
	{
		WidgetSwitcher_InventorySwitcher->SetActiveWidgetIndex(2);
	}
}

