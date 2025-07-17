#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GA_AttackAT.generated.h"

class AGA_TargetActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle& , TargetDataHandle);

UCLASS()
class IB_MULTIPLAYGAME_API UGA_AttackAT : public UAbilityTask
{
	GENERATED_BODY()

public:
	UGA_AttackAT();
	
	UFUNCTION()
	static UGA_AttackAT* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AGA_TargetActor> TargetActorClass);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void SpawnAndInitializeTargetActor();
	void FinalizeTargetActor();

protected:

	void OnTargetDataReadyToCallBack(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	UPROPERTY()
	FTraceResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<AGA_TargetActor> TargetActorClass;

	UPROPERTY()
	TObjectPtr<AGA_TargetActor> SpawnedTargetActor;

	
};
