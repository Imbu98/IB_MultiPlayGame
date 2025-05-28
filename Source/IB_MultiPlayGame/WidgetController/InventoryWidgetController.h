#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "../Inventory/ItemTypes.h"
#include "InventoryWidgetController.generated.h"

class UInventoryComponent;
struct FPackagedInventory;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemSignature, const FPackagedInventory&, PackagedInventory);
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

	void UpdateInventory(const FPackagedInventory& InventoryContents);

private:

	

	void BroadcastInventoryContents(const FPackagedInventory& InventoryContents);

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;

	
};
