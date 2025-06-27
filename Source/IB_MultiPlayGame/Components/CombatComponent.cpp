#include "CombatComponent.h"
#include "../ETC/Equippable/Armor/ArmorBase.h"
#include "../ETC/Equippable/Weapon/WeaponBase.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "../Widget/W_RPGSystemWidget.h"
#include "../Widget/W_Inventory.h"
#include "../Widget/W_InventorySlot.h"
#include "../Widget/W_PlayerInfo.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	uint8 FirstIndex =static_cast<uint8>(EItemParts::None) + 1;
	uint8 LastIndex = static_cast<uint8>(EItemParts::ForLastItemIndex);
	int32 RealEquipItemSlotQuantity =  LastIndex-2;
	
	// None과 LastItemIndex 를 빼고 초기화
	
	EquippedItemsDefinition.SetNum(RealEquipItemSlotQuantity);
	EquippedItemsDefinition.Emplace(FMasterItemDefinition());
	
	for (uint8 i =FirstIndex ; i <LastIndex ; ++i)
	{
		EItemParts Parts = static_cast<EItemParts>(i);
		EquippedItemMap.Add(Parts, FMasterItemDefinition()); // Add는 이미 있으면 덮어씀
		
		
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedItemsDefinition);
}

void UCombatComponent::ResetAttack()
{
	AttackCount = 0;
}

void UCombatComponent::SetEquippedItem(AActor* SpawnedItem)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsValid(SpawnedItem)) return;

	if (AEquippableBase* EquippedItem = Cast<AEquippableBase>(SpawnedItem))
	{
		FMasterItemDefinition EquippedItemDefinition=EquippedItem->GetItemDefinition();

		// EquippedItemActor정보와 ItemInfo Map으로 저장
		
		EquippedInstancedItemMap.Add(EquippedItemDefinition.ItemParts,EquippedItem);
		

		
		//EquippedItemsDefinition.Add(EquippedItemDefinition);
		
		// Map을 만들기 위한 아이템 정보들 Array에 추가
		int32 EquipSlotIndex = DefinitionIndex(EquippedItemDefinition);
		
		if (EquippedItemsDefinition.IsValidIndex(EquipSlotIndex))
		{
			EquippedItemsDefinition[EquipSlotIndex] = EquippedItemDefinition;
		}
		
		// Server에서 먼저 Array로 Map을 만들고, 클라이언트RPC로 클라에서도 Map을 만들어줌 
		for (const FMasterItemDefinition& ItemDef : EquippedItemsDefinition)
		{
			EquippedItemMap.Add(ItemDef.ItemParts,ItemDef);
		}
		//ClientSetEquippedItemMap(EquippedItemsDefinition);
	}
}
void UCombatComponent::ClientSetEquippedItemMap_Implementation(const TArray<FMasterItemDefinition>& EquippedItemDefinitions)
{
	
}

void UCombatComponent::UnEquipItem(const FMasterItemDefinition& ItemInfo,const float& SlotIndex)
{
	if (!GetOwner()->HasAuthority()) return;
	
	if (AEquippableBase** EquippedItemPtr = EquippedInstancedItemMap.Find(ItemInfo.ItemParts))
	{
		AEquippableBase* EquippedItem = *EquippedItemPtr;
		if (IsValid(EquippedItem))
		{
			EquippedItem->Destroy();
			EquippedInstancedItemMap[ItemInfo.ItemParts] = nullptr;
		}
	}
	if (EquippedItemsDefinition.IsValidIndex(SlotIndex))
	{
		EquippedItemsDefinition[SlotIndex] = FMasterItemDefinition();
	}
}

void UCombatComponent::OnRep_EquippedItemsDefinition()
{
	uint8 FirstIndex =static_cast<uint8>(EItemParts::None) + 1;
	uint8 LastIndex = static_cast<uint8>(EItemParts::ForLastItemIndex);
	for (uint8 i =FirstIndex ; i <LastIndex ; ++i)
	{
		EItemParts Parts = static_cast<EItemParts>(i);
		EquippedItemMap.Add(Parts, FMasterItemDefinition()); // 맵에서 Add는 키가 이미 있으면 덮어씀
	}
	
	for (const FMasterItemDefinition& ItemDef : EquippedItemsDefinition)
	{
		if (ItemDef.ItemParts!=EItemParts::None)
		{
			EquippedItemMap[ItemDef.ItemParts] = ItemDef;
		}
	}
	
	if (AIB_RPGPlayerController* IB_RPGPlayerContoller = Cast<AIB_RPGPlayerController>(GetOwner()))
	{
		if ((IB_RPGPlayerContoller->WBP_PlayerInfoWidget))
		{

			IB_RPGPlayerContoller->WBP_PlayerInfoWidget->SetEquippedItemWidget(EquippedItemMap);
		}
	}
}

TMap<EItemParts, FMasterItemDefinition> UCombatComponent::GetEquippedItemMap()
{
	if (!EquippedItemMap.IsEmpty())
	{
		return EquippedItemMap;
	}
	return TMap<EItemParts, FMasterItemDefinition>();
}

int32 UCombatComponent::DefinitionIndex(const FMasterItemDefinition& EquipItemDefinition)
{
	if (EquipItemDefinition.ItemParts == EItemParts::Weapon)
	{
		return static_cast<uint8>(EItemParts::Weapon);
	}
	if (EquipItemDefinition.ItemParts == EItemParts::Helmet)
	{
		return static_cast<uint8>(EItemParts::Helmet);
	}
	if (EquipItemDefinition.ItemParts == EItemParts::Chest)
	{
		return static_cast<uint8>(EItemParts::Chest);
	}
	if (EquipItemDefinition.ItemParts == EItemParts::Gloves)
	{
		return static_cast<uint8>(EItemParts::Gloves);
	}
	if (EquipItemDefinition.ItemParts == EItemParts::Pants)
	{
		return static_cast<uint8>(EItemParts::Pants);
	}
	if (EquipItemDefinition.ItemParts == EItemParts::Boots)
	{
		return static_cast<uint8>(EItemParts::Boots);
	}
	return static_cast<uint8>(EItemParts::None);
}


