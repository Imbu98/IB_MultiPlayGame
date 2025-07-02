#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "../Inventory/ItemTypes.h"
#include "InventoryWidgetController.generated.h"

class UInventoryComponent;
struct FUserInventory;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemSignature, const FUserInventory&, PackagedInventory,const EItemTypes ,InventoryType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryBroadCastComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScrollBoxResetSignature);

UCLASS()
class IB_MULTIPLAYGAME_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FInventoryItemSignature InventoryItemDelegate;

	UPROPERTY(BlueprintAssignable)
	FInventoryBroadCastComplete InventoryBroadCastComplete;

	UPROPERTY(BlueprintAssignable)
	FScrollBoxResetSignature ScrollBoxResetDelegate;

	void SetOwningActor(AActor* InOwner);

	void BindCallBacksToDependencies();
	void BroadcastInitialValues();

	void UpdateInventory(const FUserInventory& InventoryContents,const EItemTypes InventoryType);

private:

	

	void BroadcastInventoryContents(const FUserInventory& InventoryContents,const EItemTypes InventoryType);

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;

	
};
