#pragma once

#include "CoreMinimal.h"
#include "IB_BaseChar.h"
#include "AbilitySystemInterface.h"
#include "EnemyBaseChar.generated.h"

class UIB_RPGAbilitySystemComponent;
class UIB_RPGAttributeSet;
class UWidgetComponent;
class UW_EnemyOverHeadBars;

UCLASS()
class IB_MULTIPLAYGAME_API AEnemyBaseChar : public AIB_BaseChar, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AEnemyBaseChar();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitClassDefaults() override;
	virtual void BindCallbacksToDependencies() override;

	virtual void OnHealthChanged(float CurrentHealth, float MaxHealth) override;
	

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Widgets")
	TSubclassOf<UW_EnemyOverHeadBars> WBP_OverHeadBarsclass;

	UPROPERTY()
	TObjectPtr<UW_EnemyOverHeadBars> OverHeadBars;

	UPROPERTY(EditAnywhere, meta = (AllowAccess = true))
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(BlueprintReadOnly,meta=(AllowAccess=true))
	TObjectPtr<UIB_RPGAbilitySystemComponent> IB_RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowAccess = true))
	TObjectPtr< UIB_RPGAttributeSet> IB_RPGAttributeSet;

	void InitEnemyOverHeadBars();

};
