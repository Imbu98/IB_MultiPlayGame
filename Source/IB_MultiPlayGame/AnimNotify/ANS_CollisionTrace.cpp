#include "ANS_CollisionTrace.h"

#include "../Character/IB_MainChar.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/CombatComponent.h"
#include "../ETC/Equippable/Weapon/WeaponBase.h"
#include "../Components/CollisionComponent.h"

void UANS_CollisionTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(MeshComp->GetOwner()))
	{
		if (AIB_RPGPlayerController* IB_RPGPlayerContrller = Cast<AIB_RPGPlayerController>(IB_MainChar->GetController()))
		{
			if (UCombatComponent* CombatComponent = IB_RPGPlayerContrller->GetCombatComponent())
			{
				CombatComponent->ServerEnableCollisionTrace(DetermineHandIndex);
			}
		}
	}
}

void UANS_CollisionTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(MeshComp->GetOwner()))
	{
		if (AIB_RPGPlayerController* IB_RPGPlayerContrller = Cast<AIB_RPGPlayerController>(IB_MainChar->GetController()))
		{
			if (UCombatComponent* CombatComponent = IB_RPGPlayerContrller->GetCombatComponent())
			{
				CombatComponent->ServerDisableCollisionTrace();
			}
		}
	}
}

UCollisionComponent* UANS_CollisionTrace::GetValidWeaponCollisionRef(USkeletalMeshComponent* MeshComponent)
{
	if (MeshComponent->GetOwner() == nullptr) return nullptr;

	if (UCombatComponent* CombatComponent = MeshComponent->GetOwner()->FindComponentByClass<UCombatComponent>())
	{
		UCollisionComponent* CollisionComponent = nullptr;
		if (CombatComponent != nullptr)
		{
			if (DetermineHandIndex == 0)
			{
				CollisionComponent = CombatComponent->MainHandWeapon->GetWeaponCollisionComponent();
				
			}
			else if (DetermineHandIndex == 1)
			{
				CollisionComponent = CombatComponent->SubHandWeapon->GetWeaponCollisionComponent();
			}
		}
		return CollisionComponent;
	}
	return nullptr;
}
