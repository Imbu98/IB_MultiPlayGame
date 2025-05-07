#include "W_RPGSystemWidget.h"
#include "W_Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Components/InventoryComponent.h"
#include "../WidgetController/InventoryWidgetController.h"




void UW_RPGSystemWidget::SetWidgetController(class UWidgetController* InWidgetController)
{
	if (InWidgetController)
	{
		WidgetController = InWidgetController;
		InventoryWidgetController = Cast<UInventoryWidgetController>(WidgetController);
		AIB_RPGPlayerController* PC = Cast<AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC)
		{
			InventoryComponent = IInventoryInterface::Execute_GetInventoryComponent(PC);
			UW_Inventory* InventoryWidget = Cast<UW_Inventory>(this);
			if (InventoryWidget)
			{
				InventoryWidget->BindInventoryItemDelegate();
			}
	
		}
	}
}

void UW_RPGSystemWidget::OnWidgetControllerSet()
{
	
}


