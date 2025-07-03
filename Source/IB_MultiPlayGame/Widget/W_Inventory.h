#pragma once

#include "CoreMinimal.h"
#include "W_RPGSystemWidget.h"
#include "../Inventory/ItemTypes.h"
#include "W_Inventory.generated.h"

class UWidgetSwitcher;
struct FPackagedInventory;
class UScrollBox;
class UWrapBox;
class UTextBlock;
class UButton;
struct FMasterItemDefinition;
class UW_ItemRow;
class UW_InventorySlot;
class UW_PlayerInfo;



UCLASS()
class IB_MULTIPLAYGAME_API UW_Inventory : public UW_RPGSystemWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
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
	
	UPROPERTY(EditAnywhere, Category = "CustomValues|Widgets")
	TObjectPtr<UW_PlayerInfo> WBP_EquippedItemSlotClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Btn_EquippableInventory;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Btn_ConsumableInventory;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Btn_ETCInventory;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher>WidgetSwitcher_InventorySwitcher;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWrapBox> WB_EquippableInventoryContents;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWrapBox> WB_ConsumableInventoryContents;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWrapBox> WB_ETCInventoryContents;
	
	UPROPERTY()
	UW_InventorySlot* WBP_InventorySlot;
	UPROPERTY(EditAnywhere, Category = "CustomValues|Widgets")
	TSubclassOf<UW_InventorySlot> WBP_InventorySlotClass;
	UPROPERTY()
	TArray<UW_InventorySlot*> EquippableSlots;

	UPROPERTY()
	TArray<UW_InventorySlot*> ConsumableSlots;

	UPROPERTY()
	TArray<UW_InventorySlot*> ETCSlots;

	UPROPERTY()
	int32 InventorySlotIndex=0;

	UPROPERTY(EditAnywhere, Category = "CustomValues|DataTable")
	TObjectPtr<UDataTable> DT_ItemDefinition;
	


public:
	UFUNCTION()
	void BindInventoryItemDelegate();

	UFUNCTION()
	void InventoryItemRecieved(const FPackagedInventory& InventoryContents);
	UFUNCTION()
	void BroadcastOnclickEvent(ESlotTypes SlotType);
	UFUNCTION()
	void ResetActiveWidgets(const ESlotTypes InventoryType);
	UFUNCTION()
	void OnActionButtonClicked(const FMasterItemDefinition& Item,const float& SlotIndex);

	UFUNCTION()
	void OnEquippableButtonClicked();
	UFUNCTION()
	void OnComsumbableButtonClicked();
	UFUNCTION()
	void OnETCButtonClicked();

	UFUNCTION()
	void HandleInventoryItemRecieved(const FPackagedInventory& InventoryContents);
	
	UFUNCTION()
	void MakeItemRowWidget(const FPackagedInventory& InventoryContents);UFUNCTION()
	void InitializeSlots();
	UFUNCTION()
	void UpdateItemWidgets(const FPackagedInventory& SubInventory, ESlotTypes SlotType,UWrapBox* InventoryContentsWrapBox);
	UFUNCTION()
	void CreateInventorySlot(UWrapBox* InventoryContentsWrapBox,ESlotTypes SlotType);
	

	


};
