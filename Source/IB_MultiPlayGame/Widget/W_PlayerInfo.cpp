#include "W_PlayerInfo.h"
#include "W_InventorySlot.h"
#include "../Components/InventoryComponent.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "kismet/GameplayStatics.h"
#include "Components\Button.h"
#include "IB_MultiPlayGame/Components/CombatComponent.h"


void UW_PlayerInfo::NativeConstruct()
{
	if (EquippedWeaponSlot)
	{
		EquippedWeaponSlot->OnClickedEquippedActionButtonDelegate.Clear();
		EquippedWeaponSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 WeaponIndex =static_cast<uint8>(EItemParts::Weapon);
		EquippedWeaponSlot->SlotIndex=WeaponIndex;
		EquippedWeaponSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		
	}
	if (EquippedChestSlot)
	{
		EquippedChestSlot->OnClickedEquippedActionButtonDelegate.Clear();
		EquippedChestSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 ChestIndex =static_cast<uint8>(EItemParts::Chest);
		EquippedChestSlot->SlotIndex=ChestIndex;
		EquippedChestSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
	}
}

void UW_PlayerInfo::OnclickedEquippedButton(const FMasterItemDefinition& ItemInfo,const float& SlotIndex)
{
	if (!IsValid(InventoryComponent)) return;
	if (!IsValid(CombatComponent)) return;
	
	AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IB_RPGPC)
	{
		if (CombatComponent->EquippedItemsDefinition.IsValidIndex(SlotIndex))
		{
			FMasterItemDefinition EquippedItemInfo = CombatComponent->EquippedItemsDefinition[SlotIndex];
			// 얘를 먼저 해줘야 ItemInfo가 남아있다
			InventoryComponent->AddItem(EquippedItemInfo,1);
			IB_RPGPC->ServerUnEquipItem(EquippedItemInfo,SlotIndex);
			// 이후 초기화
			switch (EquippedItemInfo.ItemParts)
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
				EquippedWeaponSlot->UpdateSlot(EquippedItems);
				break;
			}
		}
	case EItemParts::Chest:
		{
			if (EquippedChestSlot)
			{
				EquippedChestSlot->SetItemImage(ItemIcon);
				EquippedChestSlot->UpdateSlot(EquippedItems);
				break;
			}
		}
	default:
		break;
	}

}
