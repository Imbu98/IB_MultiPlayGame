#include "W_PlayerInfo.h"
#include "W_InventorySlot.h"
#include "../Components/InventoryComponent.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "kismet/GameplayStatics.h"
#include "Components\Button.h"
#include "IB_MultiPlayGame/Components/CombatComponent.h"


void UW_PlayerInfo::NativeConstruct()
{
	
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

void UW_PlayerInfo::InitPlayerInfoSlots()
{
	if (!IsValid(CombatComponent)) return;
	
	if (EquippedWeaponSlot)
	{
		EquippedWeaponSlot->OnClickedActionButtonDelegate.Clear();
		EquippedWeaponSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 WeaponIndex =static_cast<uint8>(EItemParts::Weapon);
		EquippedWeaponSlot->SlotIndex=WeaponIndex;
		EquippedWeaponSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		EquippedWeaponSlot->ClearSlot();
		
		
	}
	if (EquippedHelmetSlot)
	{
		EquippedHelmetSlot->OnClickedActionButtonDelegate.Clear();
		EquippedHelmetSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 HelmetIndex =static_cast<uint8>(EItemParts::Helmet);
		EquippedHelmetSlot->SlotIndex=HelmetIndex;
		EquippedHelmetSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		EquippedHelmetSlot->ClearSlot();
		
	}
	if (EquippedChestSlot)
	{
		EquippedChestSlot->OnClickedActionButtonDelegate.Clear();
		EquippedChestSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 ChestIndex =static_cast<uint8>(EItemParts::Chest);
		EquippedChestSlot->SlotIndex=ChestIndex;
		EquippedChestSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		EquippedChestSlot->ClearSlot();
	}
	if (EquippedPantsSlot)
	{
		EquippedPantsSlot->OnClickedActionButtonDelegate.Clear();
		EquippedPantsSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 PantsIndex =static_cast<uint8>(EItemParts::Pants);
		EquippedPantsSlot->SlotIndex=PantsIndex;
		EquippedPantsSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		EquippedPantsSlot->ClearSlot();
	}
	if (EquippedGlovesSlot)
	{
		EquippedGlovesSlot->OnClickedActionButtonDelegate.Clear();
		EquippedGlovesSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 GlovesIndex =static_cast<uint8>(EItemParts::Gloves);
		EquippedGlovesSlot->SlotIndex=GlovesIndex;
		EquippedGlovesSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		EquippedGlovesSlot->ClearSlot();
	}
	if (EquippedBootsSlot)
	{
		EquippedBootsSlot->OnClickedActionButtonDelegate.Clear();
		EquippedBootsSlot->OnClickedActionButtonDelegate.AddUObject(this,&ThisClass::OnclickedEquippedButton);
		uint8 BootsIndex =static_cast<uint8>(EItemParts::Boots);
		EquippedBootsSlot->SlotIndex=BootsIndex;
		EquippedBootsSlot->SlotType = ESlotTypes::Slot_PlayerInfo;
		EquippedBootsSlot->ClearSlot();
	}
}

