#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GA_TargetActor.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API AGA_TargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AGA_TargetActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	void SetShowDebug(bool InShowDebug) { bDebug = InShowDebug; };


	

	
};
