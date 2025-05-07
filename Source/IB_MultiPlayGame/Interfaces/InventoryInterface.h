#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

class UInventoryComponent;

UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class IB_MULTIPLAYGAME_API IInventoryInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UInventoryComponent* GetInventoryComponent();

	//virtual UInventoryComponent* GetInventoryComponent() { return nullptr; };
	
	
};
