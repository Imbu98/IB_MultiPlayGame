#include "W_PlayerInfo.h"
#include "W_InventorySlot.h"
#include "../Components/InventoryComponent.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "kismet/GameplayStatics.h"
#include "Components\Button.h"


void UW_PlayerInfo::NativeConstruct()
{
	if (EquippedWeaponSlot)
	{
		EquippedWeaponSlot->OnClickedEquippedActionButtonDelegate.Clear();
		EquippedWeaponSlot->OnClickedEquippedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		EquippedWeaponSlot->SlotIndex=999;
	}
	if (EquippedChestSlot)
	{
		EquippedChestSlot->OnClickedEquippedActionButtonDelegate.Clear();
		EquippedChestSlot->OnClickedEquippedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		EquippedChestSlot->SlotIndex=999;
	}
}

void UW_PlayerInfo::OnclickedEquippedButton(const FMasterItemDefinition& ItemInfo)
{
	if (!IsValid(InventoryComponent)) return;
	if (ItemInfo.ItemTag==FGameplayTag()) return;
	
	AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IB_RPGPC)
	{
		// 얘를 먼저 해줘야 ItemInfo가 남아있다
		InventoryComponent->AddItem(ItemInfo,1);
		IB_RPGPC->ServerUnEquipItem(ItemInfo);

		// 이후 초기화
		switch (ItemInfo.ItemParts)
		{
		case EItemParts::Weapon:
			{
				if (EquippedWeaponSlot)
				{
					
					EquippedWeaponSlot->ClearSlot();
					break;
				}
			}
			case EItemParts::Chest:
			{
				if (EquippedChestSlot)
				{
					EquippedChestSlot->ClearSlot();
				}
			}
			default:
			break;
		}
		
	}
}



void UW_PlayerInfo::SetEquippedItemWidget(TMap<EItemParts,FMasterItemDefinition> EquippedItemsMap)
{
	for (const auto& Pair : EquippedItemsMap)
	{
		UpdateEquippedSlot(Pair.Value); // 이미 파츠별로 정리되어 있음
	}
}

void UW_PlayerInfo::UpdateEquippedSlot(const FMasterItemDefinition& EquippedItems)
{
	if (!IsValid(InventoryComponent)) return;
	
	FMasterItemDefinition StaticItemDefinition = InventoryComponent->GetItemDefinitionByTag(EquippedItems.ItemTag);
	UTexture2D* ItemIcon = StaticItemDefinition.Icon;
	
	switch (EquippedItems.ItemParts)
	{
	case EItemParts::Weapon:
		{
			if (EquippedWeaponSlot)
			{
				EquippedWeaponSlot->SetItemImage(ItemIcon);
				EquippedWeaponSlot->Item = EquippedItems;
				EquippedWeaponSlot->UpdateSlot();
				break;
			}
		}
	case EItemParts::Chest:
		{
			if (EquippedChestSlot)
			{
				EquippedChestSlot->SetItemImage(ItemIcon);
				EquippedChestSlot->Item = EquippedItems;
				EquippedChestSlot->UpdateSlot();
				break;
			}
		}
	default:
		break;
	}

}
