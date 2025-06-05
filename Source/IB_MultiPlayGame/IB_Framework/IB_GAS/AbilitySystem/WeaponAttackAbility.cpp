#include "WeaponAttackAbility.h"
#include "../Data/Weapon_Info.h"
#include "../../FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../../../ETC/Equippable/Weapon/WeaponBase.h"

#include "Net/UnrealNetwork.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UWeaponAttackAbility::UWeaponAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UWeaponAttackAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UWeaponAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// giveability로FGameplayAbilitySpecContainer 가 replication 되면 자동 실행
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();

	if (!WeaponToSpawnTag.IsValid() || !IsValid(AvatarActorFromInfo))
	{
		return;
	}

	if (UWeapon_Info* WeaponInfo = UIB_BlueprintFunctionLibrary::GetWeaponInfo(AvatarActorFromInfo))
	{
		CurrentWeaponParams = *WeaponInfo->WeaponMap.Find(WeaponToSpawnTag);
	}
	
	// 클라에서 currentweaponparams 정보들로 스폰
	if (AvatarActorFromInfo->HasAuthority())
	{
		SpawnAndAttachWeapon(CurrentWeaponParams);
	}
}

void UWeaponAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		WeaponAttack();

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

void UWeaponAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UWeaponAttackAbility::SpawnAndAttachWeapon(const FWeaponParams& CurrentWeaponParamsRef)
{
	if (!IsValid(AvatarActorFromInfo)) return;

	if (AvatarActorFromInfo->HasAuthority())
	{
		FTransform SpawnTransform;

		if (AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActorDeferred<AWeaponBase>(CurrentWeaponParamsRef.WeaponClass, SpawnTransform))
		{
			SpawnedWeapon->SetReplicates(true);
			SpawnedWeapon->SetWeaponParams(CurrentWeaponParamsRef);
			//Ability에서 damage effectInfo를 설정
			FDamageEffectInfo DamageEffectInfo;
			CaptureDamageEffectInfo(nullptr, DamageEffectInfo);

			SpawnedWeapon->DamageEffectInfo = DamageEffectInfo;
			SpawnedWeapon->SetOwner(AvatarActorFromInfo);
			SpawnedWeapon->FinishSpawning(SpawnTransform);

			FName SocketName = CurrentWeaponParamsRef.AttackSocketName;
			USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(AvatarActorFromInfo->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (Mesh && Mesh->DoesSocketExist(CurrentWeaponParamsRef.AttackSocketName))
			{
				SpawnedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeaponParamsRef.AttackSocketName);
			}

			UE_LOG(LogTemp, Warning, TEXT("WeaponBase spawned on: %s"), GetWorld()->IsNetMode(NM_Client) ? TEXT("Client") : TEXT("Server"));
		}
	}
}

void UWeaponAttackAbility::WeaponAttack()
{
	// combatcomponent에서 처리하자
	if (!CurrentWeaponParams.WeaponAttackMontageArray.IsEmpty())
	{
		UAbilityTask_PlayMontageAndWait* MontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,               
				NAME_None,          
				CurrentWeaponParams.WeaponAttackMontageArray[0],
				1.0f,                
				FName("Start"),      
				false               
			);
	}
}