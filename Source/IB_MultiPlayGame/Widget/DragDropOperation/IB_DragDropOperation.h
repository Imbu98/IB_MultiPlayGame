#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "../../Inventory/ItemTypes.h"
#include "IB_DragDropOperation.generated.h"

class UW_InventorySlot;

UCLASS()
class IB_MULTIPLAYGAME_API UIB_DragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FMasterItemDefinition DraggedItem;
	UPROPERTY()
	TObjectPtr<UW_InventorySlot> ItemSlot;
	UPROPERTY()
	int32 FromSlotIndex;
	
};
