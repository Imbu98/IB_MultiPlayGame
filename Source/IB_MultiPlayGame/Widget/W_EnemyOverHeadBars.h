#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_EnemyOverHeadBars.generated.h"

class UProgressBar;

UCLASS()
class IB_MULTIPLAYGAME_API UW_EnemyOverHeadBars : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_HealthBar;

	UFUNCTION()
	void SetProgressBarPercent(float CurrentHealth,float MaxHealth);
	
};
