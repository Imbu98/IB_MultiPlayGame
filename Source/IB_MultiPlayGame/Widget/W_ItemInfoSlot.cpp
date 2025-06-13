#include "W_ItemInfoSlot.h"
#include "../Inventory/ItemTypes.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UW_ItemInfoSlot::NativeConstruct()
{
	if (IMG_SlotImage)
	{
		IMG_SlotImage->SetBrushFromTexture(Item.Icon);
		
	}
	if (Text_ItemQuantity)
	{
		Text_ItemQuantity->SetText(FText::AsNumber(Item.ItemQuantity));
	}
	SetSlotRarityImg();
}

void UW_ItemInfoSlot::SetSlotRarityImg()
{
	FLinearColor BorderColor;

	switch (Item.ItemRarity)
	{
	case EItemRarity::None:
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
