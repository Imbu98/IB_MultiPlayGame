#include "CombatComponent.h"

#include "StateComponent.h"
#include "../ETC/Equippable/Armor/ArmorBase.h"
#include "../ETC/Equippable/Weapon/WeaponBase.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../IB_Framework/IB_GAS/IB_RPGAbilitySystemComponent.h"

#include "../Components/CollisionComponent.h"
#include "Net/UnrealNetwork.h"
#include "../Widget/W_RPGSystemWidget.h"
#include "../Widget/W_Inventory.h"
#include "../Widget/W_InventorySlot.h"
#include "../Widget/W_PlayerInfo.h"
#include "IB_MultiPlayGame/Character/IB_MainChar.h"
#include <AbilitySystemBlueprintLibrary.h>

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
	
	// None�� LastItemIndex �� ���� �ʱ�ȭ
	
	EquippedItemsDefinition.SetNum(RealEquipItemSlotQuantity);
	EquippedItemsDefinition.Emplace(FMasterItemDefinition());
	
	for (uint8 i =FirstIndex ; i <LastIndex ; ++i)
	{
		EItemParts Parts = static_cast<EItemParts>(i);
		EquippedItemMap.Add(Parts, FMasterItemDefinition()); // Add�� �̹� ������ ���
		
		
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedItemsDefinition);
	DOREPLIFETIME(UCombatComponent, CanContinueAttack);
	DOREPLIFETIME(UCombatComponent, AttackCount);
}

void UCombatComponent::ResetAttack()
{
	if (GetOwner())
	{
		if (!GetOwner()->HasAuthority())
		{
			ServerResetAttack();
			return;
		}
		if (UStateComponent* StateComponent = GetOwner()->FindComponentByClass<UStateComponent>())
		{
			StateComponent->ResetCurrentState();
			CanContinueAttack = true;
			AttackCount = 0;
		}
	}
}

void UCombatComponent::ServerResetAttack_Implementation()
{
	if (GetOwner())
	{
		if (GetOwner()->HasAuthority())
		{
			ResetAttack();
		}
	}
}

void UCombatComponent::ServerSetCanContinueAttack_Implementation()
{
	if (!GetOwner()) return;

	if (!GetOwner()->HasAuthority()) return;

	CanContinueAttack = true;
}

void UCombatComponent::SetEquippedItem(AActor* SpawnedItem, const uint8& DefinitionMainNumber)
{
	if (!GetOwner()) return;

	if (!GetOwner()->HasAuthority()) return;
	if (!IsValid(SpawnedItem)) return;

	if (AEquippableBase* EquippedItem = Cast<AEquippableBase>(SpawnedItem))
	{
		{
			FMasterItemDefinition EquippedItemDefinition = EquippedItem->GetItemDefinition();

			if (EquippedItemDefinition.ItemParts == EItemParts::Weapon)
			{
				if (AWeaponBase* Weapon = Cast<AWeaponBase>(EquippedItem))
				{
					SetWeapon(Weapon, DefinitionMainNumber);
					IsAttachedWeapon = true;
				}
			}

			//For Apply Once
			if (DefinitionMainNumber == 0)
			{
				// EquippedItemActor������ ItemInfo Map���� ����

				EquippedInstancedItemMap.Add(EquippedItemDefinition.ItemParts, EquippedItem);

				//EquippedItemsDefinition.Add(EquippedItemDefinition);

				// Map�� ����� ���� ������ ������ Array�� �߰�
				int32 EquipSlotIndex = DefinitionIndex(EquippedItemDefinition);

				if (EquippedItemsDefinition.IsValidIndex(EquipSlotIndex))
				{
					EquippedItemsDefinition[EquipSlotIndex] = EquippedItemDefinition;
				}

				// Server���� ���� Array�� Map�� �����, Ŭ���̾�ƮRPC�� Ŭ�󿡼��� Map�� ������� 
				for (const FMasterItemDefinition& ItemDef : EquippedItemsDefinition)
				{
					EquippedItemMap.Add(ItemDef.ItemParts, ItemDef);
				}
			}

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
			EquippedInstancedItemMap[ItemInfo.ItemParts] = nullptr;
			if (EquippedItemsDefinition.IsValidIndex(SlotIndex))
			{
				EquippedItemsDefinition[SlotIndex] = FMasterItemDefinition();
			}

			if (ItemInfo.ItemParts == EItemParts::Weapon)
			{
				IsAttachedWeapon = false;
				if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
				{
					if (UIB_RPGAbilitySystemComponent* IB_RPGAbilitySystemComponent = Cast<UIB_RPGAbilitySystemComponent>(OwnerAsc))
					{
						if (IB_RPGAbilitySystemComponent->Implements<URPGAbilitySystemInterface>())
						{
							UE_LOG(LogTemp, Warning, TEXT("Try to Clear meleeAttack Ability"));
							IRPGAbilitySystemInterface::Execute_ClearMeleeAttackAbility(IB_RPGAbilitySystemComponent);
						}
					}
				}
				if (MainHandWeapon)
				{
					MainHandWeapon->UnEquipItem();
					MainHandWeapon->Destroy();
					MainHandWeapon = nullptr;
				}
				if (SubHandWeapon)
				{
					SubHandWeapon->Destroy();
					MainHandWeapon = nullptr;
				}
			}
			else
			{

				EquippedItem->UnEquipItem();
				EquippedItem->Destroy();

			}
		}
	}
}

UAnimMontage* UCombatComponent::GetWeaponAnimMontage()
{
	if (EquippedInstancedItemMap.Find(EItemParts::Weapon))
	{
		if (AEquippableBase* EquippableItem  =*EquippedInstancedItemMap.Find(EItemParts::Weapon))
		{
			if (AWeaponBase* Weapon = Cast<AWeaponBase>(EquippableItem))
			{
				if (Weapon->WeaponAttackMontageArray.Num() > 0)
				{
					int32 WeaponMontageNum =Weapon->WeaponAttackMontageArray.Num()-1;
					if (AttackCount>WeaponMontageNum)
					{
						ResetAttack();
					}
					return Weapon->WeaponAttackMontageArray[AttackCount];
				}
			}
		}
	}
	return nullptr;
}

void UCombatComponent::OnRep_EquippedItemsDefinition()
{
	uint8 FirstIndex =static_cast<uint8>(EItemParts::None) + 1;
	uint8 LastIndex = static_cast<uint8>(EItemParts::ForLastItemIndex);
	for (uint8 i =FirstIndex ; i <LastIndex ; ++i)
	{
		EItemParts Parts = static_cast<EItemParts>(i);
		EquippedItemMap.Add(Parts, FMasterItemDefinition()); // �ʿ��� Add�� Ű�� �̹� ������ ���
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

void UCombatComponent::SetWeapon(AWeaponBase* InWeapon, const uint8& DefinitionMainNumber)
{
	if (!IsValid(InWeapon)) return;

	if (DefinitionMainNumber == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to Set MainHandWeapon"));
		MainHandWeapon = InWeapon;
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			if (UIB_RPGAbilitySystemComponent* IB_RPGAbilitySystemComponent = Cast<UIB_RPGAbilitySystemComponent>(OwnerAsc))
			{
				if (IB_RPGAbilitySystemComponent->Implements<URPGAbilitySystemInterface>())
				{
					UE_LOG(LogTemp, Warning, TEXT("Try to Set meleeAttack Ability"));
					FMasterItemDefinition WeaponDefinition = InWeapon->GetItemDefinition();
					IRPGAbilitySystemInterface::Execute_SetMeleeAttackAbility(IB_RPGAbilitySystemComponent, WeaponDefinition.ItemTag);
				}
			}
		}
	}
	if(DefinitionMainNumber == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to Set SubHandWeapon"));
		SubHandWeapon = InWeapon;
	}
	
}

void UCombatComponent::ServerEnableCollisionTrace_Implementation(const uint8& DefinitionMainNumber)
{
	UCollisionComponent* CollisionComponent = nullptr;
	if (DefinitionMainNumber == 0)
	{
		if (MainHandWeapon)
		{
			CollisionComponent = MainHandWeapon->GetWeaponCollisionComponent();
		}
	}
	else if (DefinitionMainNumber == 1)
	{
		if (SubHandWeapon)
		{
			CollisionComponent = SubHandWeapon->GetWeaponCollisionComponent();
		}
	}
	CollisionComponent->EnableCollision();
}


void UCombatComponent::ServerDisableCollisionTrace_Implementation()
{
		if (MainHandWeapon)
		{
			if (UCollisionComponent* CollisionComponent = MainHandWeapon->GetWeaponCollisionComponent())
			{
				CollisionComponent->DisableCollision();
			}
		}

		if (SubHandWeapon)
		{
			if (UCollisionComponent* CollisionComponent = SubHandWeapon->GetWeaponCollisionComponent())
			{
				CollisionComponent->DisableCollision();
			}
		}

}

