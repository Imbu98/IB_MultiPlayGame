#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_Hitcheck.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UAN_Hitcheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAN_Hitcheck();

protected:
	virtual FString GetNotifyName_Implementation() const override;
	UFUNCTION()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere,Meta=(Category=Event))
	FGameplayTag TriggerGameplayTag;


	
};
