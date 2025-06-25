#include "CombatComponent.h"
#include "../ETC/Equippable/Armor/ArmorBase.h"
#include "../ETC/Equippable/Weapon/WeaponBase.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "../Widget/W_RPGSystemWidget.h"
#include "../Widget/W_Inventory.h"
#include "../Widget/W_InventorySlot.h"
#include "../Widget/W_EquippedItemSlot.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeaponBase);
	DOREPLIFETIME(UCombatComponent, EquippedChest);
}

void UCombatComponent::ResetAttack()
{
	AttackCount = 0;
}

void UCombatComponent::SetEquippedItem(AActor* SpawnedItem)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsValid(SpawnedItem)) return;

	if (AArmorBase* ArmorBase = Cast<AArmorBase>(SpawnedItem))
	{
		const FGameplayTag HelmetTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Helmet"));
		const FGameplayTag ChestTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Chest"));
		const FGameplayTag PantsTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Pants"));
		const FGameplayTag GlovesTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Gloves"));
		const FGameplayTag BootsTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Armor.Boots"));

		FMasterItemDefinition ArmorDefinition = ArmorBase->GetItemDefinition();

		if (ArmorDefinition.ItemTag.MatchesTag(HelmetTag))
		{
			EquippedHelmet = ArmorBase;
		}
		else if (ArmorDefinition.ItemTag.MatchesTag(ChestTag))
		{
			EquippedChest = ArmorBase;
		}
		else if (ArmorDefinition.ItemTag.MatchesTag(PantsTag))
		{
			EquippedPants = ArmorBase;
		}
		else if (ArmorDefinition.ItemTag.MatchesTag(GlovesTag))
		{
			EquippedGloves = ArmorBase;
		}
		else if (ArmorDefinition.ItemTag.MatchesTag(BootsTag))
		{
			EquippedBoots = ArmorBase;
		}
	}
	else if (AWeaponBase* WeaponBase = Cast<AWeaponBase>(SpawnedItem))
	{
		EquippedWeaponBase = WeaponBase;

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

	if (Iteminfo.ItemTag.MatchesTag(HelmetTag))
	{
		if (EquippedHelmet)
		{
			EquippedHelmet->Destroy();
			EquippedHelmet = nullptr;
		}
	}
	else if (Iteminfo.ItemTag.MatchesTag(ChestTag))
	{
		if (EquippedChest)
		{
			EquippedChest->Destroy();
			EquippedChest = nullptr;
		}
	}
	else if (Iteminfo.ItemTag.MatchesTag(PantsTag))
	{
		if (EquippedPants)
		{
			EquippedPants->Destroy();
			EquippedPants = nullptr;
		}
	}
	else if (Iteminfo.ItemTag.MatchesTag(GlovesTag))
	{
		if (EquippedGloves)
		{
			EquippedGloves->Destroy();
			EquippedGloves = nullptr;
		}
	}
	else if (Iteminfo.ItemTag.MatchesTag(BootsTag))
	{
		if (EquippedBoots)
		{
			EquippedBoots->Destroy();
			EquippedBoots = nullptr;
		}
	}
	else if (Iteminfo.ItemTag.MatchesTag(WeaponTag))
	{
		if (EquippedWeaponBase)
		{
			EquippedWeaponBase->Destroy();
			EquippedWeaponBase = nullptr;
		}
		
	}

}

void UCombatComponent::OnRep_EquippedWeapon()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, ([this]
	{
		if (!EquippedWeaponBase) return;

		FMasterItemDefinition ItemDef = EquippedWeaponBase->GetItemDefinition();

		// PlayerController → InventoryWidget → EquippedWeaponSlot 찾기
		if (AIB_RPGPlayerController* PC = Cast<AIB_RPGPlayerController>(GetOwner()))
		{
			if (UW_Inventory* InventoryWidget = Cast<UW_Inventory>(PC->InventoryWidget))   // 너가 만든 함수
			{
				if (UW_InventorySlot* WeaponSlot = InventoryWidget->WBP_EquippedItemSlot->EquippedWeaponSlot)
				{
					WeaponSlot->SetItem(ItemDef);
					WeaponSlot->UpdateSlot();
				}
			}
		}
	}), 0.2f, false);
	
}

void UCombatComponent::OnRep_EquippedChest()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, ([this]
	{
		if (!EquippedWeaponBase) return;

		FMasterItemDefinition ItemDef = EquippedWeaponBase->GetItemDefinition();

		// PlayerController → InventoryWidget → EquippedWeaponSlot 찾기
		if (AIB_RPGPlayerController* PC = Cast<AIB_RPGPlayerController>(GetOwner()))
		{
			if (UW_Inventory* InventoryWidget = Cast<UW_Inventory>(PC->InventoryWidget))   // 너가 만든 함수
			{
				if (UW_InventorySlot* ChestSlot = InventoryWidget->WBP_EquippedItemSlot->EquippedChestSlot)
				{
					ChestSlot->SetItem(ItemDef);
					ChestSlot->UpdateSlot();
				}
			}
		}
	}), 0.2f, false);
}