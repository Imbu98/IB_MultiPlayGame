#include "AN_Hitcheck.h"
#include "AbilitySystemBlueprintLibrary.h"

UAN_Hitcheck::UAN_Hitcheck()
{

}

FString UAN_Hitcheck::GetNotifyName_Implementation() const
{
	return TEXT("GasAttackHitCheck");
}

void UAN_Hitcheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);
		}
	}


}
