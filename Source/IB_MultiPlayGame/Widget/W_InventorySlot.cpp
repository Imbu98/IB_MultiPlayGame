#include "W_InventorySlot.h"
#include "W_DraggedImageItem.h"
#include "Blueprint/DragDropOperation.h"
#include "DragDropOperation/IB_DragDropOperation.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/InventoryComponent.h"
#include "../Interfaces/InventoryInterface.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"




void UW_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_ItemSlot)
	{
		Btn_ItemSlot->OnClicked.Clear();
		Btn_ItemSlot->OnClicked.AddDynamic(this, &UW_InventorySlot::OnclickedActionButton);
		
	}
	AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IB_RPGPC)
	{
		InventoryComponent = IInventoryInterface::Execute_GetInventoryComponent(IB_RPGPC);
		if (InventoryComponent)
		{
			
		}
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
		if (!IsValid(InventoryComponent)) return;
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

					int32 ItemQuantity=0;
					FMasterItemDefinition StaticItemInfo=FMasterItemDefinition();
					
					FPackagedInventory Inventory = GetInventory(SlotType);

					if (Inventory.ItemQuantities.IsValidIndex(SlotIndex))
					{
						ItemQuantity = Inventory.ItemQuantities[SlotIndex];
					}
					if (Inventory.ItemDefinitions.IsValidIndex(SlotIndex))
					{
						StaticItemInfo = InventoryComponent->GetItemDefinitionByTag(Inventory.ItemDefinitions[SlotIndex].ItemTag);
					}

					if (ItemQuantity == 0) return;  //�������� ������ �ƹ��͵� �ȵǰ�
					
					DragOp->FromSlotIndex = SlotIndex;
					WBP_DraggedImageItem->ItemImage = StaticItemInfo.Icon;
					DragOp->FromSlotType = SlotType;

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

	if (!IsValid(FromSlot)&& !IsValid(ToSlot)) return false;
	if (FromSlot == ToSlot) return false;
	
	if (FromSlot->SlotType==ToSlot->SlotType)
	{
		if (InventoryComponent)
		{
			int32 SourceIndex = FromSlot->SlotIndex; 
			int32 TargetIndex = ToSlot->SlotIndex;

			FUserInventory& CachedInventory = InventoryComponent->GetCachedUserInventory();
			
			FPackagedInventory Inventory = FPackagedInventory();
			
			if (SlotType==ESlotTypes::Slot_Equippable)
			{
				Inventory = CachedInventory.EquippableInventory;
			}
			else if (SlotType==ESlotTypes::Slot_Consumable)
			{
				Inventory = CachedInventory.ConsumableInventory;
			}
			else if (SlotType==ESlotTypes::Slot_ETC)
			{
				Inventory = CachedInventory.ETCInventory;
			}
				InventoryComponent->SwapItemsInPackagedInventory(Inventory, SourceIndex, TargetIndex);
			
				FMasterItemDefinition SourceItem = Inventory.ItemDefinitions[SourceIndex];
				FMasterItemDefinition TargetItem = Inventory.ItemDefinitions[TargetIndex];
			
				this->UpdateSlot(SourceItem);
				FromSlot->UpdateSlot(TargetItem);
				return true;
		}
	}
return false;
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
		if (Quantity > 0)
		{
			Text_ItemQuantity->SetText(FText::FromString(FString::Printf(TEXT("x %d"), Quantity)));
		}
		else
		{
			Text_ItemQuantity->SetText(FText::FromString(FString::Printf(TEXT(""))));
		}
		
	}
}



void UW_InventorySlot::OnclickedActionButton()
{
	FMasterItemDefinition ItemInfo = FMasterItemDefinition();
	FPackagedInventory Inventory = GetInventory(SlotType);
	
	if (Inventory.ItemDefinitions.IsValidIndex(SlotIndex))
	{
		ItemInfo = Inventory.ItemDefinitions[SlotIndex];
	}
	
	OnClickedActionButtonDelegate.Broadcast(ItemInfo,SlotIndex);
}

void UW_InventorySlot::UpdateSlot(const FMasterItemDefinition& ItemInfo)
{		
		if (IMG_SlotImage)
		{
			IMG_SlotImage->SetBrushFromTexture(SlotItemImage);
		}
		SetSlotRarityImg(ItemInfo);
}

void UW_InventorySlot::ClearSlot()
{
	if (IMG_SlotImage)
	{
		IMG_SlotImage->SetBrushFromTexture(nullptr);
	}
	if (Text_ItemQuantity)
	{
		Text_ItemQuantity->SetText(FText::FromString(FString::Printf(TEXT(""))));
	}
	if (Border_Frame)
	{
		Border_Frame->SetBrushColor(FLinearColor::White);
	}
    // tag�ʱ�ȭ �߰�
	//Item.ItemTag = FGameplayTag::RequestGameplayTag(FName("Item.None"));
}

void UW_InventorySlot::SetSlotRarityImg(const FMasterItemDefinition& ItemInfo)
{
	FLinearColor BorderColor=FLinearColor::White;

	switch (ItemInfo.ItemRarity)
	{
	case EItemRarity::Common:
		{
			BorderColor = FLinearColor::Gray;
			break;
		}
	case EItemRarity::Rare:
		{
			BorderColor = FLinearColor::Blue;
			break;
		}
	case EItemRarity::Epic:
		{

			BorderColor = FLinearColor(0.5f, 0.0f, 0.5f);
			break;
		}
	case EItemRarity::Legendary:
		{
			BorderColor = FLinearColor(1.0f, 0.5f, 0.0f);
			break;
		}

	default:
		break;
	}
	if (Border_Frame)
	{
		Border_Frame->SetBrushColor(BorderColor);
	}
}

EItemTypes UW_InventorySlot::FilterCategoryTag(const FGameplayTag& Tag)
{
	FGameplayTag ConsumableTag = FGameplayTag::RequestGameplayTag(FName("Item.Consumable"));
	if (Tag.MatchesTag(ConsumableTag))
	{
		return EItemTypes::Item_Consumable;
	}
	return EItemTypes::Item_Consumable;
}

FPackagedInventory UW_InventorySlot::GetInventory(const ESlotTypes InventoryType)
{
	// 여기서 인벤토리 아이템 정보를 브로드캐스트해준다
	FUserInventory& CachedUserInventory = InventoryComponent->GetCachedUserInventory();
	FPackagedInventory Inventory = FPackagedInventory();

	if (InventoryType==ESlotTypes::Slot_Equippable)
	{
		if (CachedUserInventory.EquippableInventory.ItemDefinitions.IsValidIndex(SlotIndex))
		{
			Inventory= CachedUserInventory.EquippableInventory;
		}
	}
	else if (InventoryType==ESlotTypes::Slot_Consumable)
	{
		if (CachedUserInventory.ConsumableInventory.ItemDefinitions.IsValidIndex(SlotIndex))
		{
			Inventory= CachedUserInventory.ConsumableInventory;
		}
	}
	else if (InventoryType==ESlotTypes::Slot_ETC)
	{
		if (CachedUserInventory.ETCInventory.ItemDefinitions.IsValidIndex(SlotIndex))
		{
			Inventory= CachedUserInventory.ETCInventory;
		}
	}
	else if (InventoryType==ESlotTypes::Slot_PlayerInfo)
	{
		// 빈 ItemInfo를 보내줘도됨
	}
	return Inventory;
}