#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "IB_RPGAbilitySystemComponent.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UIB_RPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);
	
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	void SetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel);

private:

	FGameplayAbilitySpecHandle ActiveProjectileAbilty;

	UPROPERTY(EditDefaultsOnly,Category= "Custom Values | Projectile Ability ")
	TSubclassOf<UGameplayAbility> DynamicProjectileAbility;

	UFUNCTION(Server,Reliable)
	void ServerSetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel);
	
	
	
};
