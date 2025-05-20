#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "IB_RPGAttributeSet.h"
#include "IB_RPGPlayerState.generated.h"

class UIB_RPGAbilitySystemComponent;

UCLASS()
class IB_MULTIPLAYGAME_API AIB_RPGPlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AIB_RPGPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	UIB_RPGAbilitySystemComponent* GetIB_RPGAbilitySystemComponent() const;
	UFUNCTION(BlueprintPure)
	UIB_RPGAttributeSet* GetIB_RPGAttributeSet() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TObjectPtr<UIB_RPGAbilitySystemComponent> IB_RPGAbilitySystemComp;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TObjectPtr<UIB_RPGAttributeSet> IB_RPGAttributeSet;
	
	
	
};
