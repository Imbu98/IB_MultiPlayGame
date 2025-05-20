#include "ProjectileAbility.h"
#include "../Data/Projectile_Info.h"
#include "../../FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../../../ETC/Projectiles/ProjectileBase.h"
#include "../../../Interfaces/RPGAbilitySystemInterface.h"

UProjectileAbility::UProjectileAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UProjectileAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();

	if (!ProjectileToSpawnTag.IsValid()||!IsValid(AvatarActorFromInfo))
	{
		return;
	}

	if (UProjectile_Info* ProjectileInfo = UIB_BlueprintFunctionLibrary::GetProjectileInfo(AvatarActorFromInfo))
	{
		CurrentProjectileParams = *ProjectileInfo->ProjectileMap.Find(ProjectileToSpawnTag);
	}

}

void UProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo,TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		SpawnProjectile();

		FTimerHandle TimerHandle;
		const FGameplayAbilitySpecHandle HandleCopy = Handle;
		const FGameplayAbilityActorInfo* ActorInfoCopy = ActorInfo;
		const FGameplayAbilityActivationInfo ActivationInfoCopy = ActivationInfo;


		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, HandleCopy, ActorInfoCopy, ActivationInfoCopy]()
			{
				EndAbility(HandleCopy, ActorInfoCopy, ActivationInfoCopy, true, false);
			}, 0.1f, false);
	}
	
}

void UProjectileAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UProjectileAbility::SpawnProjectile()
{
	if (!IsValid(CurrentProjectileParams.ProjectileClass))
	{
		return;
	}

	if (USceneComponent* SpawnPointComp = IRPGAbilitySystemInterface::Execute_GetDynamicSpawnPoint(AvatarActorFromInfo))
	{
		const FVector SpawnPoint = SpawnPointComp->GetComponentLocation();
		const FVector TargetLocation = AvatarActorFromInfo->GetActorForwardVector() * 10000.f;
		const FRotator TargetRotation = (TargetLocation - SpawnPoint).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnPoint);
		SpawnTransform.SetRotation(TargetRotation.Quaternion());

		if (AProjectileBase* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(CurrentProjectileParams.ProjectileClass, SpawnTransform))
		{
			SpawnedProjectile->SetProjectileParams(CurrentProjectileParams);

			FDamageEffectInfo DamageEffectInfo;
			CaptureDamageEffectInfo(nullptr, DamageEffectInfo);

			SpawnedProjectile->DamageEffectInfo = DamageEffectInfo;
			SpawnedProjectile->SetOwner(AvatarActorFromInfo);

			SpawnedProjectile->FinishSpawning(SpawnTransform);
		}
	}


}
