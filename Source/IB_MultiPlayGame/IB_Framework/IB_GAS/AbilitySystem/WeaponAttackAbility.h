#pragma once

#include "CoreMinimal.h"
#include "RPGGameplayAbility.h"
#include "IB_RPGAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "RPGDamageAbility.h"
#include "WeaponAttackAbility.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UWeaponAttackAbility : public URPGDamageAbility
{
	GENERATED_BODY()
	
public:
	UWeaponAttackAbility();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values | Weapon")
	FGameplayTag WeaponToSpawnTag;

	UPROPERTY()
	float WeaponDamage;

private:
	UPROPERTY()
	TObjectPtr<AActor> AvatarActorFromInfo;

	FWeaponParams CurrentWeaponParams;

	UFUNCTION()
	void SpawnAndAttachWeapon(const FWeaponParams& CurrentWeaponParamsRef);

	UFUNCTION(BlueprintCallable)
	void WeaponAttack();
};
