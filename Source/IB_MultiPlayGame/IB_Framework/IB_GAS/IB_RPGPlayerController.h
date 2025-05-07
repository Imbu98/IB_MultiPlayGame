#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IB_MultiPlayGame/Components/InventoryComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "../../Interfaces/InventoryInterface.h"
#include "IB_RPGPlayerController.generated.h"

class UInventoryWidgetController;
class UW_RPGSystemWidget;
class URPGInputConfig;
class UIB_RPGAbilitySystemComponent;


UCLASS()
class IB_MULTIPLAYGAME_API AIB_RPGPlayerController : public APlayerController, public IAbilitySystemInterface , public IInventoryInterface
{
	GENERATED_BODY()

public:
	AIB_RPGPlayerController();

	virtual void SetupInputComponent() override;

	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UInventoryWidgetController* GetInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void BeginPlay() override;

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Input")
	TObjectPtr<UIB_RPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(EditDefaultsOnly,Category= "CustomValues|Input")
	TObjectPtr<URPGInputConfig> RPGInputConfig;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess=true),Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UW_RPGSystemWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Widgets")
	TSubclassOf<UW_RPGSystemWidget> InventoryWidgetClass;

	UIB_RPGAbilitySystemComponent* GetRPGAbilitySystemComponent();
	
	
	
};
