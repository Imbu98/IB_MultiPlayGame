#include "IB_GameInstance.h"


void UIB_GameInstance::SavePlayerInventory(UInventoryComponent* Inventory)
{
	if (Inventory)
	{
		SavedInventory = Inventory->GetCachedInventory();
		bResetInventory = Inventory->IsFirstStart;
	}
}