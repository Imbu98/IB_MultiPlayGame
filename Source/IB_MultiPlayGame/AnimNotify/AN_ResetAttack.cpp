#include "AN_ResetAttack.h"
#include "../Character/IB_MainChar.h"

#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/CombatComponent.h"


void UAN_ResetAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{

	if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(MeshComp->GetOwner()))
	{
		if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(IB_MainChar->GetController()))
		{
			if (UCombatComponent* CombatComponent = IB_RPGPlayerController->GetCombatComponent())
			{
				CombatComponent->ResetAttack();
			}
		}

	}
	//IBChar->ResetAttack();

}



