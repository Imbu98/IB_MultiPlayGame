#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_RPGSystemWidget.generated.h"

class UInventoryWidgetController;
class UInventoryComponent;

UCLASS()
class IB_MULTIPLAYGAME_API UW_RPGSystemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void SetWidgetController(class UWidgetController* InWidgetController);
	
	UFUNCTION()
	void OnWidgetControllerSet();

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetController> WidgetController;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	

	

	
	
	
};
