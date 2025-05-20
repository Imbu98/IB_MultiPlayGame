#pragma once

#include "CoreMinimal.h"
#include "RPGGameplayAbility.h"
#include "IB_RPGAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "RPGDamageAbility.h"
#include "ProjectileAbility.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UProjectileAbility : public URPGDamageAbility
{
	GENERATED_BODY()
	
public:
	UProjectileAbility();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly,Category="Custom Values | Projectile")
	FGameplayTag ProjectileToSpawnTag;

private:
	UPROPERTY()
	TObjectPtr<AActor> AvatarActorFromInfo;

	FProjectileParams CurrentProjectileParams;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();
	
};
