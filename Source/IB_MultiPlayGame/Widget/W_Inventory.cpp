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
		InventoryWidgetController->ScrollBoxResetDelegate.Clear();
	}
}

void UW_Inventory::BindInventoryItemDelegate()
{
	if (InventoryWidgetController)
	{
		// Tab�� ���� Invetory������ ����� �������� ItemRecieve��������Ʈ�� ������� �ʴ´� ���߿� ��� ���� �����غ���
		InventoryWidgetController->InventoryItemDelegate.AddDynamic(this, &UW_Inventory::InventoryItemRecieved);
		
		InventoryWidgetController->InventoryBroadCastComplete.AddDynamic(this, &UW_Inventory::InventoryBroadcastComplete);
		
		InventoryWidgetController->ScrollBoxResetDelegate.AddDynamic(this, &UW_Inventory::OnScrollBoxReset);

	}
}
void UW_Inventory::InventoryBroadcastComplete()
{
	for (UW_InventorySlot* Widgets : ActiveItemWidgets)
	{
		Widgets->OnClickedActionButtonDelegate.AddUObject(this, &UW_Inventory::OnActionButtonClicked);
	}

}

void UW_Inventory::InventoryItemRecieved(const FPackagedInventory& PackagedInventory)
{
	
	HandleInventoryItemRecieved(PackagedInventory);
}


void UW_Inventory::HandleInventoryItemRecieved(const FPackagedInventory& PackagedInventory)
{

	MakeItemRowWidget(PackagedInventory);
}

// On client
void UW_Inventory::MakeItemRowWidget(const FPackagedInventory& PackagedInventory)
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC)) return;

	UCombatComponent* CombatComponent = PC->GetComponentByClass<UCombatComponent>();
	if (!IsValid(CombatComponent)) return;

	if (!IsValid(InventoryWidgetController)) return;

	if (!IsValid(InventoryComponent)) return;

	const int32 NumSlots = InventoryComponent->GetInventorySize();

	for (int32 i = 0; i < NumSlots; i++)
	{
		UW_InventorySlot* SlotWidget = CreateWidget<UW_InventorySlot>(this, WBP_InventorySlotClass);
		if (!SlotWidget) continue;
		

		// �±װ� ��ȿ�� �������� ���
		if (PackagedInventory.ItemTags.IsValidIndex(i) &&
			PackagedInventory.ItemQuantities.IsValidIndex(i) &&
			PackagedInventory.ItemTags[i].IsValid()&&
			PackagedInventory.ItemDefinitions[i].ItemTag.IsValid())
		{
			// ������ ������ ������ ������ ��������
			FMasterItemDefinition ItemData = InventoryComponent->GetItemDefinitionByTag(PackagedInventory.ItemTags[i]); // �� �Լ��� OwningInventory ��� ����

			ItemData.ItemQuantity = PackagedInventory.ItemQuantities[i];

			SlotWidget->SetItemImage(ItemData.Icon);
			SlotWidget->SetQuiantityText(ItemData.ItemQuantity);
			SlotWidget->Item = PackagedInventory.ItemDefinitions[i];
		}
		else
		{
			// �� ���� ó��
			SlotWidget->ClearSlot(); // �� �Լ��� �⺻ ������, �ؽ�Ʈ ��� �� ����
		}
		SlotWidget->SlotIndex = i;

		if (WB_InventoryContents)
		{
			WB_InventoryContents->AddChild(SlotWidget);
		}

		ActiveItemWidgets.Add(SlotWidget);
	}
	//SetEquippedItemWidget(CombatComponent->EquippedItemsDefinition);
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

void UW_Inventory::OnActionButtonClicked(const FMasterItemDefinition& Item,const float& SlotIndex)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->UseItem(Item,SlotIndex,1);
	}
}

