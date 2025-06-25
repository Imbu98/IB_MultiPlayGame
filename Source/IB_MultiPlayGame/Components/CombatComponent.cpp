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

	// 총 개수는 None과 아이템 수를 세기위한 LastItemIndex 2개를 빼서 계산
	int32 AcutualEquippedItemNum = static_cast<int32>(EItemParts::ForLastItemIndex) - 2;

	for (int32 i = 0; i < AcutualEquippedItemNum; ++i)
	{
		EquippedItems.Emplace(nullptr);
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
		EquippedItems.Add(EquippedItem);
		EquippedItemsDefinition.Add(EquippedItem->GetItemDefinition());
		GetOwner()->ForceNetUpdate();
		/*const FGameplayTag HelmetTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Helmet"));
		const FGameplayTag ChestTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Chest"));
		const FGameplayTag PantsTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Pants"));
		const FGameplayTag GlovesTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Gloves"));
		const FGameplayTag BootsTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Boots"));*/

		/*FMasterItemDefinition ArmorDefinition = ArmorBase->GetItemDefinition();

		switch (ArmorDefinition.ItemParts)
		{
		case EItemParts::None:
			break;

		case EItemParts::Helmet:
		{
			EquippedHelmet = ArmorBase;
			break;
		}
		case EItemParts::Chest:
		{
			EquippedChest = ArmorBase;
			break;
		}
		case EItemParts::Pants:
		{
			EquippedPants = ArmorBase;
			break;
		}
		case EItemParts::Gloves:
		{
			EquippedGloves = ArmorBase;
			break;
		}
		case EItemParts::Boots:
		{
			EquippedBoots = ArmorBase;
			break;
		}
		default:
			break;
		}

	}
	else if (AWeaponBase* WeaponBase = Cast<AWeaponBase>(SpawnedItem))
	{
		EquippedWeaponBase = WeaponBase;

	}	*/
	}
}

void UCombatComponent::UnEquipItem(const FMasterItemDefinition& Iteminfo)
{
	if (!GetOwner()->HasAuthority()) return;

	const FGameplayTag WeaponTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Weapon"));
	const FGameplayTag HelmetTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Helmet"));
	const FGameplayTag ChestTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Chest"));
	const FGameplayTag PantsTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Pants"));
	const FGameplayTag GlovesTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Gloves"));
	const FGameplayTag BootsTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Boots"));

	for (AEquippableBase* EquippedItem : EquippedItems)
	{
		if (EquippedItem)
		{
			FMasterItemDefinition EquippedItmeDefinition = EquippedItem->GetItemDefinition();
			if (EquippedItmeDefinition.ItemTag != FGameplayTag())
			{
				if (EquippedItmeDefinition.ItemParts == Iteminfo.ItemParts)
				{
					EquippedItem->Destroy();
					EquippedItem=nullptr;
				}
			}
		}
	}
}

// 고치기 UW_PlayerInfoWidget에서 처리하자
void UCombatComponent::OnRep_EquippbaleBase()
{
	if (AIB_RPGPlayerController* IB_RPGPlayerContoller = Cast<AIB_RPGPlayerController>(GetOwner()))
	{
		if ((IB_RPGPlayerContoller->WBP_PlayerInfoWidget))
		{

			IB_RPGPlayerContoller->WBP_PlayerInfoWidget->SetEquippedItemWidget(EquippedItemsDefinition);
		}
	}
}