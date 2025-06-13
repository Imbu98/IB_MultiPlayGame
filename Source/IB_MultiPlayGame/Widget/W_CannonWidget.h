#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CannonWidget.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UW_CannonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (BindWidget))
	TObjectPtr<class UProgressBar> PB_Power;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (BindWidget))
	TObjectPtr<class UTextBlock>  T_EquipWeaponCaution;
	

public:
	UFUNCTION()
	void UpdatePowerProgressBar(const float InCannonPowner, const float InMaxCannonPowner);
	
};
