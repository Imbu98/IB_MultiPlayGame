#include "IB_RPGAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "../IB_GAS/AbilitySystem/RPGGameplayAbility.h"
#include "../IB_GAS/AbilitySystem/ProjectileAbility.h"
#include "../IB_GAS/AbilitySystem/WeaponAttackAbility.h"


void UIB_RPGAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1.f);
		if (const URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(RPGAbility->InputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UIB_RPGAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : PassivesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1.f);

		
		GiveAbility(AbilitySpec);
		
	}
}

void UIB_RPGAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
	checkf(AttributeEffect,TEXT("No valid default attributes for this character %s"),*GetAvatarActor()->GetName());
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect,1.f,ContextHandle);
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UIB_RPGAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
					PredictionKey);
			}
		}
	}

	//FScopedAbilityListLock ActiveScopeLock(*this); 
}

void UIB_RPGAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,
					PredictionKey);
			}
		}
	}
}

void UIB_RPGAbilitySystemComponent::SetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	if (!ProjectileTag.IsValid())
	{
		return;
	}

	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSetDynamicProjectile(ProjectileTag,AbilityLevel);
		return;
	}
	if (ActiveProjectileAbilty.IsValid())
	{
		ClearAbility(ActiveProjectileAbilty);
	}

	if (IsValid(DynamicProjectileAbility))
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(DynamicProjectileAbility, AbilityLevel);
		if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(Spec.Ability))
		{
			ProjectileAbility->ProjectileToSpawnTag = ProjectileTag;
			Spec.GetDynamicSpecSourceTags().AddTag(ProjectileAbility->InputTag);

			// 
			ActiveProjectileAbilty = GiveAbility(Spec);
		}
	}

}

void UIB_RPGAbilitySystemComponent::ServerSetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	SetDynamicProjectile(ProjectileTag, AbilityLevel);

}

void UIB_RPGAbilitySystemComponent::SetMeleeAttackAbility_Implementation(const FGameplayTag& WeaponTag)
{
	if (!WeaponTag.IsValid())
	{
		return;
	}

	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSetMeleeAttackAbility(WeaponTag);
		return;
	}
	if (ActiveWeaponMeleeAttackAbilty.IsValid())
	{
		ClearAbility(ActiveWeaponMeleeAttackAbilty);
	}

	if (IsValid(WeaponMeleeAttackAbility))
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(WeaponMeleeAttackAbility);
		if (UWeaponAttackAbility* WeaponAttackAbility = Cast<UWeaponAttackAbility>(Spec.Ability))
		{
			// equipment component �����, item���� �� WeaponAttackAbility->CurrentWeaponParams�� �� item�� ����
			
			WeaponAttackAbility->WeaponTag = WeaponTag;

			Spec.GetDynamicSpecSourceTags().AddTag(WeaponAttackAbility->InputTag);

			ActiveWeaponMeleeAttackAbilty = GiveAbility(Spec);

		}
	}
}

void UIB_RPGAbilitySystemComponent::ServerSetMeleeAttackAbility_Implementation(const FGameplayTag& WeaponTag)
{
	SetMeleeAttackAbility(WeaponTag);
}



