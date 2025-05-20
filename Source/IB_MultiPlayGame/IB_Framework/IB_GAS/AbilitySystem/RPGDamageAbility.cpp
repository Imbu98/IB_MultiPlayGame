#include "RPGDamageAbility.h"
#include "AbilitySystemGlobals.h"
#include "../AbilitySystem/IB_RPGAbilityTypes.h"

void URPGDamageAbility::CaptureDamageEffectInfo(AActor* TargetActor, FDamageEffectInfo& OutInfo)
{
	if (AActor* AvavaterActorFromInfo = GetAvatarActorFromActorInfo())
	{
		OutInfo.AbilityLevel = GetAbilityLevel();
		OutInfo.AvatarActor = AvavaterActorFromInfo;
		OutInfo.BaseDamage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
		OutInfo.DamageEffect = DamageEffect;
		OutInfo.SourceASC = GetAbilitySystemComponentFromActorInfo();

		if (IsValid(TargetActor))
		{
			OutInfo.TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
		}
	}
}
