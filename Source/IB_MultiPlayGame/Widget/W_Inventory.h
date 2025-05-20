#pragma once

#include "CoreMinimal.h"
#include "W_RPGSystemWidget.h"
#include "W_Inventory.generated.h"

class UScrollBox;
class UWrapBox;
class UTextBlock;
struct FMasterItemDefinition;
class UW_ItemRow;
class UW_InventorySlot;

UCLASS()
class IB_MULTIPLAYGAME_API UW_Inventory : public UW_RPGSystemWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeDestruct() override;

	


	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	//TObjectPtr<UScrollBox> SB_InventoryContents;
	/*UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_ItemDescriptionText;
	UPROPERTY()
	UW_ItemRow* WBP_ItemRow;
	UPROPERTY(EditAnywhere, Category = "CustomValues|Widgets")
	TSubclassOf<UW_ItemRow> WBP_ItemRowClass;
	UPROPERTY()
	TArray<UW_ItemRow*> ActiveItemWidgets;*/

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWrapBox> WB_InventoryContents;
	UPROPERTY()
	UW_InventorySlot* WBP_InventorySlot;
	UPROPERTY(EditAnywhere, Category = "CustomValues|Widgets")
	TSubclassOf<UW_InventorySlot> WBP_InventorySlotClass;
	UPROPERTY()
	TArray<UW_InventorySlot*> ActiveItemWidgets;

	UPROPERTY()
	int32 InventorySlotIndex=0;


public:

	UFUNCTION()
	void BindInventoryItemDelegate();

	UFUNCTION()
	void InventoryItemRecieved(const FMasterItemDefinition& Item);
	UFUNCTION()
	void InventoryBroadcastComplete();
	UFUNCTION()
	void OnScrollBoxReset();
	UFUNCTION()
	void OnActionButtonClicked(const FMasterItemDefinition& Item);

	UFUNCTION()
	void HandleInventoryItemRecieved(const FMasterItemDefinition& Item);

	UFUNCTION()
	void MakeItemRowWidget(const FMasterItemDefinition& Item);


};
