#include "W_DraggedImageItem.h"
#include "Components/Image.h"

void UW_DraggedImageItem::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Img_DraggedImage)
	{
		Img_DraggedImage->SetBrushFromTexture(ItemImage);
	}
}
