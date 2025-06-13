#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../Components/InventoryComponent.h"
#include "IB_GameInstance.generated.h"

class UInventoryComponent;

UCLASS()
class IB_MULTIPLAYGAME_API UIB_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FPackagedInventory SavedInventory;
	UPROPERTY()
	bool bResetInventory;

public:
	UFUNCTION()
	void SavePlayerInventory(UInventoryComponent* Inventory);
	
};
