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
		if (!EquippedInstancedItemMap.Contains(EquippedItemDefinition.ItemParts))
		{
			EquippedInstancedItemMap.Add(EquippedItemDefinition.ItemParts,EquippedItem);
		}

		// Map을 만들기 위한 아이템 정보들 Array에 추가
		EquippedItemsDefinition.Add(EquippedItemDefinition);

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

void UCombatComponent::UnEquipItem(const FMasterItemDefinition& ItemInfo)
{
	if (!GetOwner()->HasAuthority()) return;
	
	if (AEquippableBase** EquippedItemPtr = EquippedInstancedItemMap.Find(ItemInfo.ItemParts))
	{
		AEquippableBase* EquippedItem = *EquippedItemPtr;
		if (IsValid(EquippedItem))
		{
			EquippedItem->Destroy();
			EquippedInstancedItemMap.Remove(ItemInfo.ItemParts);
		}
	}
	
	int32 FoundIndex = EquippedItemsDefinition.IndexOfByPredicate(
	[&ItemInfo](const FMasterItemDefinition& Item) {
		return Item.ItemTag == ItemInfo.ItemTag;
	});

	if (FoundIndex != INDEX_NONE)
	{
		EquippedItemsDefinition.RemoveAt(FoundIndex);
	}
}

void UCombatComponent::OnRep_EquippedItemsDefinition()
{
	uint8 FirstIndex =static_cast<uint8>(EItemParts::None) + 1;
	uint8 LastIndex = static_cast<uint8>(EItemParts::ForLastItemIndex);
	for (uint8 i =FirstIndex ; i <LastIndex ; ++i)
	{
		EItemParts Parts = static_cast<EItemParts>(i);
		EquippedItemMap.Add(Parts, FMasterItemDefinition()); // Add는 이미 있으면 덮어씀
	}
	
	for (const FMasterItemDefinition& ItemDef : EquippedItemsDefinition)
	{
		EquippedItemMap[ItemDef.ItemParts] = ItemDef;
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
