#include "W_InventorySlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "W_DraggedImageItem.h"
#include "Blueprint/DragDropOperation.h"
#include "DragDropOperation/IB_DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/InventoryComponent.h"
#include "../Interfaces/InventoryInterface.h"



void UW_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateSlot();

	if (Btn_ItemSlot)
	{
		Btn_ItemSlot->OnClicked.Clear();
		Btn_ItemSlot->OnClicked.AddDynamic(this, &UW_InventorySlot::OnclickedActionButton);
	}
}

FReply UW_InventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnPreviewMouseButtonDown(MyGeometry, MouseEvent);

}

void UW_InventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (WBP_DraggedImageItemClass)
	{
		WBP_DraggedImageItem = CreateWidget<UW_DraggedImageItem>(this, WBP_DraggedImageItemClass);
		if (WBP_DraggedImageItem)
		{
			AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			
				UIB_DragDropOperation* DragOp = Cast<UIB_DragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UIB_DragDropOperation::StaticClass()));
				if (DragOp)
				{
					DragOp->DefaultDragVisual = WBP_DraggedImageItem;
					DragOp->Pivot = EDragPivot::CenterCenter;
					DragOp->ItemSlot = this;

					DragOp->DraggedItem = Item;
					DragOp->FromSlotIndex = SlotIndex;
					WBP_DraggedImageItem->ItemImage = Item.Icon;
					

					OutOperation = DragOp;
				}
			
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Draged")));
}

bool UW_InventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UIB_DragDropOperation* DragOp = Cast<UIB_DragDropOperation>(InOperation);
	if (!DragOp || !DragOp->ItemSlot) return false;

	UW_InventorySlot* FromSlot = DragOp->ItemSlot;
	UW_InventorySlot* ToSlot = this;

	if (FromSlot == ToSlot) return false;

	AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IB_RPGPC)
	{
		UInventoryComponent* InventoryComponent = IInventoryInterface::Execute_GetInventoryComponent(IB_RPGPC);
		if (InventoryComponent)
		{
			int32 SourceIndex = FromSlot->SlotIndex;
			int32 TargetIndex = ToSlot->SlotIndex;

			FMasterItemDefinition TempItem = ToSlot->Item;
			ToSlot->SetItem(FromSlot->Item);
			FromSlot->SetItem(TempItem);

			int32 TempIndex = ToSlot->SlotIndex;
			ToSlot->SlotIndex = FromSlot->SlotIndex;
			FromSlot->SlotIndex = TempIndex;

			this->UpdateSlot();
			FromSlot->UpdateSlot();

			FPackagedInventory& CachedInventory = InventoryComponent->GetCachedInventory(); 
			InventoryComponent->SwapItemsInPackagedInventory(CachedInventory, SourceIndex, TargetIndex);

			
			
		}
	}

	return true;

}

void UW_InventorySlot::SetItemImage(UTexture2D* ItemImage)
{
	if (IMG_SlotImage)
	{
		IMG_SlotImage->SetBrushFromTexture(ItemImage);
		SlotItemImage = ItemImage;
	}
}

void UW_InventorySlot::SetQuiantityText(int32 Quantity)
{
	if (Text_ItemQuantity)
	{
		Text_ItemQuantity->SetText(FText::FromString(FString::Printf(TEXT("x %d"), Quantity)));
	}
}

void UW_InventorySlot::OnclickedActionButton()
{
	OnClickedActionButtonDelegate.Broadcast(Item);
}

void UW_InventorySlot::UpdateSlot()
{
	if (IMG_SlotImage)
	{
		IMG_SlotImage->SetBrushFromTexture(Item.Icon);
	}
	if (Text_ItemQuantity)
	{
		Text_ItemQuantity->SetText(FText::FromString(FString::Printf(TEXT("x %d"), Item.ItemQuantity)));
	}
}

void UW_InventorySlot::SetItem(const FMasterItemDefinition& NewItem)
{
	Item = NewItem;
}

EItemTypes UW_InventorySlot::FilterCategoryTag(const FGameplayTag& Tag)
{
	FGameplayTag ConsumableTag = FGameplayTag::RequestGameplayTag(FName("Item.Consumable"));
	if (Tag.MatchesTag(ConsumableTag))
	{
		return EItemTypes::Consumable;
	}
	return EItemTypes::None;
}
