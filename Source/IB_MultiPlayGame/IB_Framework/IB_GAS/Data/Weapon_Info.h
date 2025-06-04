#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../AbilitySystem/IB_RPGAbilityTypes.h"
#include "Weapon_Info.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UWeapon_Info : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FWeaponParams> WeaponMap;

};
