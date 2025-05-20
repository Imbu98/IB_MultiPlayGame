#pragma once

#include "CoreMinimal.h"
#include "RPGGameplayAbility.h"
#include "RPGDamageAbility.generated.h"

struct FDamageEffectInfo;

UCLASS()
class IB_MULTIPLAYGAME_API URPGDamageAbility : public URPGGameplayAbility
{
	GENERATED_BODY()
	
	
public:

	void CaptureDamageEffectInfo(AActor* TargetActor, FDamageEffectInfo& OutInfo);

private:
	UPROPERTY(EditDefaultsOnly,Category= "Custom Values | Damage Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values | Damage Effect")
	FScalableFloat BaseDamage;

};
