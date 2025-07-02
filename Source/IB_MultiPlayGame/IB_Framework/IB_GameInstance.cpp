#include "IB_GameInstance.h"


void UIB_GameInstance::SavePlayerInventory(UInventoryComponent* Inventory)
{
	if (Inventory)
	{
		bResetInventory = Inventory->IsFirstStart;
	}
}