#include "IB_BlueprintFunctionLibrary.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../IB_GAS/AbilitySystem/RPGGameplayTags.h"
#include "../../Character/IB_MainChar.h"
#include "../IB_GAS/IB_RPGPlayerController.h"


UIB_CharacterClassInfo* UIB_BlueprintFunctionLibrary::GetCharacterClassDefaultInfo(const UObject* WorldContextObject)
{
	if (const AIB_RPGGameMode* IB_RPGGameMode =Cast<AIB_RPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return IB_RPGGameMode->GetCharacterClassDefaultInfo();
	}

	return nullptr;
}

UProjectile_Info* UIB_BlueprintFunctionLibrary::GetProjectileInfo(const UObject* WorldContextObject)
{
	if (const AIB_RPGGameMode* IB_RPGGameMode = Cast<AIB_RPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return IB_RPGGameMode->GetProjectileInfo();
	}

	return nullptr;
}

UWeapon_Info* UIB_BlueprintFunctionLibrary::GetWeaponInfo(const UObject* WorldContextObject)
{
	if (const AIB_RPGGameMode* IB_RPGGameMode = Cast<AIB_RPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return IB_RPGGameMode->GetWeaponInfo();
	}

	return nullptr;
}

void UIB_BlueprintFunctionLibrary::ApplyDamageEffect(const FDamageEffectInfo& DamageEffectInfo)
{

			FGameplayEffectContextHandle ContextHandle = DamageEffectInfo.SourceASC->MakeEffectContext();

			ContextHandle.AddSourceObject(DamageEffectInfo.AvatarActor);

			const FGameplayEffectSpecHandle SpecHandle = DamageEffectInfo.SourceASC->MakeOutgoingSpec(DamageEffectInfo.DamageEffect, DamageEffectInfo.AbilityLevel, ContextHandle);

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,RPGGameplayTags::Combat::Data_Damage,DamageEffectInfo.BaseDamage);

			if (IsValid(DamageEffectInfo.TargetASC))
			{
				DamageEffectInfo.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}

			
}

AIB_MainChar* UIB_BlueprintFunctionLibrary::GetMyPlayerCharacter(const UObject* WorldContextObject)
{
	if (AIB_MainChar* PlayerCharacter = Cast<AIB_MainChar>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0)))
	{
		return PlayerCharacter;
	}
	return nullptr;
	
}

AGameModeBase* UIB_BlueprintFunctionLibrary::GetMyGameMode(const UObject* WorldContextObject)
{
	if (AIB_RPGGameMode* IB_RPGGameMode = Cast<AIB_RPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return IB_RPGGameMode;
	}
	return nullptr;
}

AIB_RPGPlayerController* UIB_BlueprintFunctionLibrary::GetMyPlayerController(const UObject* WorldContextObject)
{
	if (AIB_MainChar* PlayerCharacter = Cast<AIB_MainChar>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0)))
	{
		 if (AIB_RPGPlayerController* PlayerController = Cast<AIB_RPGPlayerController>(PlayerCharacter->GetController()))
		{
			 return PlayerController;
		}
	}
	return nullptr;
}
