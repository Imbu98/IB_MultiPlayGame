#include "W_LocationNotify.h"
#include "Components/TextBlock.h"

void UW_LocationNotify::NativeConstruct()
{
	Super::NativeConstruct();

	if (TextBlock_LocationName)
	{
		TextBlock_LocationName->SetText(LocationName);
	}
	if(WAM_FlyIn)
	{
		PlayAnimationForward(WAM_FlyIn);
		
	}
}

void UW_LocationNotify::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == WAM_FlyIn)
	{
		this->RemoveFromParent();
	}
}
