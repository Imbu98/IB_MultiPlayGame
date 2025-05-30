#include "W_ItemInfoSlot.h"
#include "../Inventory/ItemTypes.h"

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
}