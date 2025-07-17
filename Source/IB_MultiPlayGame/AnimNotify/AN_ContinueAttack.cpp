#include "AN_ContinueAttack.h"

#include "IB_MultiPlayGame/Character/IB_MainChar.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "IB_MultiPlayGame/Components/CombatComponent.h"
#include "IB_MultiPlayGame/Components/StateComponent.h"
#include <AbilitySystemBlueprintLibrary.h>

void UAN_ContinueAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(MeshComp->GetOwner()))
	{
		if (AIB_RPGPlayerController* IB_RPGPlayerContrller = Cast<AIB_RPGPlayerController>(IB_MainChar->GetController()))
		{
			if (UCombatComponent* CombatComponent = IB_RPGPlayerContrller->GetCombatComponent())
			{
				CombatComponent->ServerSetCanContinueAttack();
			}
		}
	}
}


