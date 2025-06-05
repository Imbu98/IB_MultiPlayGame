#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "RPGAbilitySystemInterface.generated.h"

UINTERFACE(MinimalAPI)
class URPGAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

class IB_MULTIPLAYGAME_API IRPGAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	USceneComponent* GetDynamicSpawnPoint();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel=1);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetDynamicWeapon(const FGameplayTag& WeaponTag, int32 AbilityLevel = 1);
	
	
};
