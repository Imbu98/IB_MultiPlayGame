#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IB_MultiPlayGame/IB_MultiPlayGameCharacter.h"
#include "GameplayTagContainer.h"
#include "../DefineDelegates.h"
#include "IB_BaseChar.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API AIB_BaseChar : public ACharacter
{
	GENERATED_BODY()

public:
	AIB_BaseChar();

	UFUNCTION()
	virtual void OnHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	virtual void OnManaChanged(float CurrentMana, float MaxMana);

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo();

	virtual void BindCallbacksToDependencies();

	virtual void InitClassDefaults();

	UPROPERTY(EditAnywhere, Category = "Custom Values|Character Info")
	FGameplayTag CharacterTag;

	


};
