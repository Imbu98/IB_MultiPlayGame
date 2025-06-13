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
		if (UIB_GameInstance* IB_GameInstance = Cast<UIB_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			IB_GameInstance->SavePlayerInventory(OwningInventory);
		}
		//OwningInventory->ReConstructInventoryMap(InventoryContents);
		BroadcastInventoryContents(InventoryContents);
	}
}

// on client
void UInventoryWidgetController::BroadcastInventoryContents(const FPackagedInventory& InventoryContents)
{
	if (IsValid(OwningInventory))
	{
		ScrollBoxResetDelegate.Broadcast();

		InventoryItemDelegate.Broadcast(InventoryContents);

		InventoryBroadCastComplete.Broadcast();
	}

}


