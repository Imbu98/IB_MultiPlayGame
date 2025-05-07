#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Overlay.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UW_Overlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UProgressBar> PB_HealthBar;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UProgressBar> PB_ManaBar;
	
	
public:
	UFUNCTION()
	void SetHealthBar(float CurrentHealth,float MaxHealth);
	UFUNCTION()
	void SetManaBar(float CurrentMana,float MaxMana);
	
	
	
	
};
