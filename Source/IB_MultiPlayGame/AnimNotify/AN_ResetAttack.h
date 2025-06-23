#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ResetAttack.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UAN_ResetAttack : public UAnimNotify
{
	GENERATED_BODY()

	UFUNCTION()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
