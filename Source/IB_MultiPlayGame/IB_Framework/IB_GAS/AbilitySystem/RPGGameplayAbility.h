#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGGameplayAbility.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category="CustomValues|Input")
	FGameplayTag InputTag;
	
};
