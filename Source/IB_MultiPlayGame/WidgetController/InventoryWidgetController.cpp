#include "InventoryWidgetController.h"
#include "../Interfaces/InventoryInterface.h"
#include "../Components/InventoryComponent.h"

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallBacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

	if (IsValid(OwningInventory))
	{
		//OwningInventory->InventoryPackageDelegate.AddUObject(this, &ThisClass::UpdateInventory);

		OwningInventory->InventoryPackageDelegate.AddLambda(
			[this](const FPackagedInventory& InventoryContents)
			{
				UpdateInventory(InventoryContents);
			});
	}

}

void UInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		BroadcastInventoryContents(OwningInventory->GetCachedInventory());

	}
}

void UInventoryWidgetController::UpdateInventory(const FPackagedInventory& InventoryContents)
{
	if (IsValid(OwningInventory))
	{
		
		OwningInventory->ReConstructInventoryMap(InventoryContents);
		BroadcastInventoryContents(InventoryContents);
	}
}

void UInventoryWidgetController::BroadcastInventoryContents(const FPackagedInventory& InventoryContents)
{
	if (IsValid(OwningInventory))
	{
		InventoryItemDelegate.Broadcast(InventoryContents);

		InventoryBroadCastComplete.Broadcast();
	}

}


