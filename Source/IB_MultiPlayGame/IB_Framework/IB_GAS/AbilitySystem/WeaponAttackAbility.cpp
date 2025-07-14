#include "WeaponAttackAbility.h"
#include "../Data/Weapon_Info.h"
#include "../Data/ArmorInfo.h"
#include "../../FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../../../ETC/Equippable/Weapon/WeaponBase.h"
#include "../../../Character/IB_MainChar.h"
#include "../IB_RPGAbilitySystemComponent.h"
#include "../IB_RPGPlayerController.h"
#include "../../../Components/CombatComponent.h"

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
	
	if (!WeaponTag.IsValid() || !IsValid(AvatarActorFromInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("NoWeaponTag or AvatarActroFromInfo"));
		return;
	}
	if (UWeapon_Info* WeaponInfo = UIB_BlueprintFunctionLibrary::GetWeaponInfo(AvatarActorFromInfo))
	{
		
		CurrentWeaponParams = *WeaponInfo->WeaponMap.Find(WeaponTag);
	}
	UE_LOG(LogTemp, Warning, TEXT("WeaponAttackability is activated"));

}

void UWeaponAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		FTimerHandle TimerHandle;
		const FGameplayAbilitySpecHandle HandleCopy = Handle;
		const FGameplayAbilityActorInfo* ActorInfoCopy = ActorInfo;
		const FGameplayAbilityActivationInfo ActivationInfoCopy = ActivationInfo;

		WeaponAttack();

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

void UWeaponAttackAbility::WeaponAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Try To weapon Attack"));

	if (!IsValid(AvatarActorFromInfo)) return;

	AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(AvatarActorFromInfo);
	if (!IsValid(IB_MainChar)) return;
	
	if (AIB_RPGPlayerController* IB_PlayerController= Cast<AIB_RPGPlayerController>(AvatarActorFromInfo->GetOwner()))
	{
		if (UCombatComponent* CombatComponent = IB_PlayerController->GetCombatComponent())
		{
			int32 AttackIndex = CombatComponent->AttackCount;

			if (CurrentWeaponParams.WeaponAttackMontageArray.Num() > 0)
			{
				if (CombatComponent->CanContinueAttack)
				{
					CombatComponent->CanContinueAttack = false;

					IB_MainChar->MulticastPlayMontage(CurrentWeaponParams.WeaponAttackMontageArray[AttackIndex]);
					CombatComponent->AttackCount++;

					if (AttackIndex >= CurrentWeaponParams.WeaponAttackMontageArray.Num() - 1)
					{
						CombatComponent->ResetAttack();
					}
				}
			}
		}
	}
}