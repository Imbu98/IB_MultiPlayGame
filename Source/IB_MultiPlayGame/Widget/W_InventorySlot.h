#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DefineDelegates.h"
#include "../Inventory/ItemTypes.h"
#include "IB_MultiPlayGame/Components/InventoryComponent.h"
#include "W_InventorySlot.generated.h"

class UButton;
class UImage;
class UTextBlock;
struct FGameplayTag;
struct FMasterItemDefinition;
class UW_DraggedImageItem;
class UBorder;
class UInventoryComponent;


UCLASS()

class IB_MULTIPLAYGAME_API UW_InventorySlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
public:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	TObjectPtr<UButton> Btn_ItemSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> IMG_SlotImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBorder> Border_Frame;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemQuantity;
	UPROPERTY()
	int32 SlotIndex;
	UPROPERTY(EditAnywhere,Category="Image")
	TObjectPtr<UTexture2D> SlotItemImage;
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	ESlotTypes SlotType;

	UPROPERTY(EditAnywhere,Category="CustomValues|Widget")
	TSubclassOf<UW_DraggedImageItem> WBP_DraggedImageItemClass;
	UPROPERTY()
	UW_DraggedImageItem* WBP_DraggedImageItem;


	FOnClickedActionButton OnClickedActionButtonDelegate;
	
	FOnClickedEquippedActionButton OnClickedEquippedActionButtonDelegate;

	UFUNCTION()
	void SetItemImage(UTexture2D* ItemImage);
	UFUNCTION()
	void SetQuiantityText(int32 Quantity);
	UFUNCTION()
	void SetSlotRarityImg(const FMasterItemDefinition& ItemInfo);

	UFUNCTION()
	void OnclickedActionButton();

	UFUNCTION()
	void UpdateSlot(const FMasterItemDefinition& ItemInfo);
	UFUNCTION()
	void ClearSlot();
	UFUNCTION()
	FPackagedInventory GetInventory(const ESlotTypes InventoryType);

	EItemTypes FilterCategoryTag(const FGameplayTag& Tag);
	
	
};
