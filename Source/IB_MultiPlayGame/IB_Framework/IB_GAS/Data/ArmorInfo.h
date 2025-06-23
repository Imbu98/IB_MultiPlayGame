#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../AbilitySystem/IB_RPGAbilityTypes.h"
#include "ArmorInfo.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UArmorInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FArmorParams> ArmorMaps;

};
