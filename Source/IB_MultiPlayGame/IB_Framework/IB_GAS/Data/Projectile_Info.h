#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../AbilitySystem/IB_RPGAbilityTypes.h"
#include "Projectile_Info.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UProjectile_Info : public UDataAsset
{
	GENERATED_BODY()

public:
	
UPROPERTY(EditDefaultsOnly)
TMap<FGameplayTag, FProjectileParams> ProjectileMap;

	
};
