#include "W_Inventory.h"
#include "../Components/InventoryComponent.h"
#include "W_ItemRow.h"
#include "W_InventorySlot.h"
#include "../Inventory/ItemTypes.h"
#include "../DefineDelegates.h"
#include "../WidgetController/InventoryWidgetController.h"

#include "kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"



void UW_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
		PlayerController->SetShowMouseCursor(true);
	}
	
}

void UW_Inventory::NativeDestruct()
{
	Super::NativeDestruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->SetShowMouseCursor(false);
	}

	if (InventoryWidgetController)
	{
		InventoryWidgetController->InventoryItemDelegate.Clear();
		InventoryWidgetController->InventoryBroadCastComplete.Clear();
		InventoryWidgetController->ScrollBoxResetDelegate.Clear();
	}
}

void UW_Inventory::BindInventoryItemDelegate()
{
	if (InventoryWidgetController)
	{
		// Tab을 눌러 Invetory위젯을 만들기 전까지는 ItemRecieve델리게이트가 선언되지 않는다 나중에 어떻게 할지 생각해보자
		InventoryWidgetController->InventoryItemDelegate.AddDynamic(this, &UW_Inventory::InventoryItemRecieved);
		
		InventoryWidgetController->InventoryBroadCastComplete.AddDynamic(this, &UW_Inventory::InventoryBroadcastComplete);
		
		InventoryWidgetController->ScrollBoxResetDelegate.AddDynamic(this, &UW_Inventory::OnScrollBoxReset);

	}
}
void UW_Inventory::InventoryBroadcastComplete()
{
	for (UW_InventorySlot* Widgets : ActiveItemWidgets)
	{
		Widgets->OnClickedActionButtonDelegate.AddUObject(this, &UW_Inventory::OnActionButtonClicked);
	}

}

void UW_Inventory::InventoryItemRecieved(const FPackagedInventory& PackagedInventory)
{
	
	HandleInventoryItemRecieved(PackagedInventory);
}


void UW_Inventory::HandleInventoryItemRecieved(const FPackagedInventory& PackagedInventory)
{

	MakeItemRowWidget(PackagedInventory);
}

// On client
void UW_Inventory::MakeItemRowWidget(const FPackagedInventory& PackagedInventory)
{
	if (!IsValid(InventoryWidgetController)) return;

	if (!IsValid(InventoryComponent)) return;

	const int32 NumSlots = InventoryComponent->GetInventorySize();

	for (int32 i = 0; i < NumSlots; i++)
	{
		UW_InventorySlot* SlotWidget = CreateWidget<UW_InventorySlot>(this, WBP_InventorySlotClass);
		if (!SlotWidget) continue;
		

		// 태그가 유효한 아이템인 경우
		if (PackagedInventory.ItemTags.IsValidIndex(i) &&
			PackagedInventory.ItemQuantities.IsValidIndex(i) &&
			PackagedInventory.ItemTags[i].IsValid()&&
			PackagedInventory.ItemDefinitions[i].ItemTag.IsValid())
		{
			// 아이템 수량과 아이콘 정보만 가져오기
			FMasterItemDefinition ItemData = InventoryComponent->GetItemDefinitionByTag(PackagedInventory.ItemTags[i]); // 이 함수는 OwningInventory 등에서 구현

			ItemData.ItemQuantity = PackagedInventory.ItemQuantities[i];

			SlotWidget->SetItemImage(ItemData.Icon);
			SlotWidget->SetQuiantityText(ItemData.ItemQuantity);
			SlotWidget->Item = PackagedInventory.ItemDefinitions[i];
		}
		else
		{
			// 빈 슬롯 처리
			SlotWidget->ClearSlot(); // 이 함수는 기본 아이콘, 텍스트 비움 등 설정
		}
		SlotWidget->SlotIndex = i;

		if (WB_InventoryContents)
		{
			WB_InventoryContents->AddChild(SlotWidget);
		}

		ActiveItemWidgets.Add(SlotWidget);

	}
}

void UW_Inventory::OnScrollBoxReset()
{
	if (WB_InventoryContents)
	{
		WB_InventoryContents->ClearChildren();
		for (UW_InventorySlot* Widgets : ActiveItemWidgets)
		{
			Widgets->OnClickedActionButtonDelegate.Clear();
		}
		ActiveItemWidgets.Empty();
	}
}

void UW_Inventory::OnActionButtonClicked(const FMasterItemDefinition& Item)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->UseItem(Item.ItemTag,1, Item);
	}
}

