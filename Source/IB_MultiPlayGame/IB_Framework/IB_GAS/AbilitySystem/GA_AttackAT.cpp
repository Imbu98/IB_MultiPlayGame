#include "GA_AttackAT.h"

#include "GA_TargetActor.h"
#include "AbilitySystemComponent.h"

UGA_AttackAT::UGA_AttackAT()
{
}

UGA_AttackAT* UGA_AttackAT::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AGA_TargetActor> TargetActorClass)
{
	UGA_AttackAT* NewTask = NewAbilityTask<UGA_AttackAT>(OwningAbility);
	if (NewTask)
	{
		NewTask->TargetActorClass = TargetActorClass;
		return NewTask;
	}

	return nullptr;
}

void UGA_AttackAT::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void UGA_AttackAT::OnDestroy(bool AbilityEnded)
{
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

void UGA_AttackAT::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<AGA_TargetActor>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->SetShowDebug(true);
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &ThisClass::OnTargetDataReadyToCallBack);
	}
}

void UGA_AttackAT::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		const FTransform SpawnTrasnform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTrasnform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);

		SpawnedTargetActor->StartTargeting(Ability);
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void UGA_AttackAT::OnTargetDataReadyToCallBack(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}
	EndTask();
}
