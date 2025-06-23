#include "CombatComponent.h"
#include "../ETC/Equippable/Armor/ArmorBase.h"
#include "../ETC/Equippable/Weapon/WeaponBase.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "../Widget/W_RPGSystemWidget.h"
#include "../Widget/W_Inventory.h"

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
		const FGameplayTag WeaponTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable.Weapon"));
		FMasterItemDefinition WeaponDefinition = WeaponBase->GetItemDefinition();

		EquippedWeaponBase = WeaponBase;

	}

	
	
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(GetOwner()))
	{
		if (UW_Inventory* W_Inventory = Cast<UW_Inventory>(IB_RPGPlayerController->InventoryWidget))
		{
			FMasterItemDefinition ItemInfo =  EquippedWeaponBase->GetItemDefinition();
			W_Inventory->SetEquippedItemWidget(ItemInfo);
		}
	}
}
