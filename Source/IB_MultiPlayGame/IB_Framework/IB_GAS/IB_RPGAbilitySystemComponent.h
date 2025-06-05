#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "../../Interfaces/RPGAbilitySystemInterface.h"
#include "IB_RPGAbilitySystemComponent.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UIB_RPGAbilitySystemComponent : public UAbilitySystemComponent , public IRPGAbilitySystemInterface
{
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);
	
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	void SetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel);

	virtual void SetDynamicWeapon_Implementation(const FGameplayTag& WeaponTag, int32 AbilityLevel) override;

private:

	FGameplayAbilitySpecHandle ActiveProjectileAbilty;

	FGameplayAbilitySpecHandle ActiveWeaponAttackAbilty;

	UPROPERTY(EditDefaultsOnly,Category= "Custom Values | Abilities ")
	TSubclassOf<UGameplayAbility> DynamicProjectileAbility;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values | Abilities ")
	TSubclassOf<UGameplayAbility> DynamicWeaponAttackAbility;

	UFUNCTION(Server,Reliable)
	void ServerSetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel);

	UFUNCTION(Server, Reliable)
	void ServerSetDynamicWeapon(const FGameplayTag& WeaponTag, int32 AbilityLevel);
	
	
	
};
