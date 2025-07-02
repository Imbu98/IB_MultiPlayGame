#include "InventoryWidgetController.h"
#include "../Interfaces/InventoryInterface.h"
#include "../Components/InventoryComponent.h"
#include "../IB_Framework/IB_GameInstance.h"
#include "Kismet/GameplayStatics.h"

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

		OwningInventory->UserInventoryPackageDelegate.AddLambda(
			[this](const FUserInventory& InventoryContents,const EItemTypes InventoryType)
			{
				UpdateInventory(InventoryContents,InventoryType);
	
			});
	}

}

void UInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		BroadcastInventoryContents(OwningInventory->GetCachedUserInventory(),EItemTypes::Item_None);

	}
}

void UInventoryWidgetController::UpdateInventory(const FUserInventory& InventoryContents,const EItemTypes InventoryType)
{
	if (IsValid(OwningInventory))
	{
		if (UIB_GameInstance* IB_GameInstance = Cast<UIB_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			IB_GameInstance->SavePlayerInventory(OwningInventory);
		}
		//OwningInventory->ReConstructInventoryMap(InventoryContents);
		BroadcastInventoryContents(InventoryContents,InventoryType);
	}
}

// on client
void UInventoryWidgetController::BroadcastInventoryContents(const FUserInventory& InventoryContents,const EItemTypes InventoryType)
{
	if (IsValid(OwningInventory))
	{
		//ScrollBoxResetDelegate.Broadcast();

		InventoryItemDelegate.Broadcast(InventoryContents,InventoryType);

		InventoryBroadCastComplete.Broadcast();
	}

}


