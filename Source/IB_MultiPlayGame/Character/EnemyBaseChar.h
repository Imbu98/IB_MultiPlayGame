#pragma once

#include "CoreMinimal.h"
#include "IB_BaseChar.h"
#include "AbilitySystemInterface.h"
#include "../Interfaces/InteractInterface.h"
#include "../DefineDelegates.h"
#include "EnemyBaseChar.generated.h"

class UIB_RPGAbilitySystemComponent;
class UIB_RPGAttributeSet;
class UWidgetComponent;
class UW_EnemyOverHeadBars;
class AIB_RPGPlayerController;

UCLASS()
class IB_MULTIPLAYGAME_API AEnemyBaseChar : public AIB_BaseChar, public IAbilitySystemInterface , public IInteractInterface
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

	virtual void SetDamageInstigator_Implementation(AIB_RPGPlayerController* IB_RPGPlayerController) override;

protected:
	UFUNCTION()
	void HandleEnemyDeath(AIB_RPGPlayerController* IB_RPGPlayerController);
	

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



public:
	UPROPERTY(EditAnywhere, meta = (AllowAccess = true))
	FString ObjectiveID;
	UPROPERTY()
	AIB_RPGPlayerController* DamageInstigator=nullptr;
	UPROPERTY()
	bool bIsDead = false;

};
