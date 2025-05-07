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
		BroadcastInventoryContents();

	}
}

void UInventoryWidgetController::UpdateInventory(const FPackagedInventory& InventoryContents)
{
	if (IsValid(OwningInventory))
	{
		OwningInventory->ReConstructInventoryMap(InventoryContents);

		BroadcastInventoryContents();
	}
}

void UInventoryWidgetController::BroadcastInventoryContents()
{
	if (IsValid(OwningInventory))
	{
		TMap<FGameplayTag, int32> LocalInventoryMap = OwningInventory->GetInventoryTagMap();

		ScrollBoxResetDelegate.Broadcast();

		for (const auto& Pair : LocalInventoryMap)
		{
			FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(Pair.Key);
			Item.ItemQuantity = Pair.Value;
			InventoryItemDelegate.Broadcast(Item);
		}
		InventoryBroadCastComplete.Broadcast();
	}
}



