#include "W_EquippedItemSlot.h"
#include "W_InventorySlot.h"
#include "../Components/InventoryComponent.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "kismet/GameplayStatics.h"
#include "Components\Button.h"


void UW_EquippedItemSlot::NativeConstruct()
{
	if (EquippedWeaponSlot)
	{
		EquippedWeaponSlot->Btn_ItemSlot->OnClicked.Clear();
		EquippedWeaponSlot->OnClickedActionButtonDelegate.AddUObject(this, &ThisClass::OnclickedEquippedButton);
	}
	if (EquippedChestSlot)
	{
		EquippedChestSlot->Btn_ItemSlot->OnClicked.Clear();
		EquippedChestSlot->OnClickedActionButtonDelegate.AddUObject(this, &ThisClass::OnclickedEquippedButton);
	}
}

void UW_EquippedItemSlot::OnclickedEquippedButton(const FMasterItemDefinition& ItemInfo)
{
	AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IB_RPGPC)
	{
		UInventoryComponent* InventoryComponent = IInventoryInterface::Execute_GetInventoryComponent(IB_RPGPC);
		if (EquippedWeaponSlot && EquippedWeaponSlot->Btn_ItemSlot->IsPressed())
		{
			InventoryComponent->AddItem(ItemInfo.ItemTag,1,ItemInfo);
			EquippedWeaponSlot->ClearSlot();
		}
		if (EquippedChestSlot && EquippedChestSlot->Btn_ItemSlot->IsPressed())
		{
			InventoryComponent->AddItem(ItemInfo.ItemTag, 1, ItemInfo);
			EquippedChestSlot->ClearSlot();
		}
		IB_RPGPC->ServerUnEquipItem(ItemInfo);
	}
}
