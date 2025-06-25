#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Inventory/ItemTypes.h"
#include "W_PlayerInfo.generated.h"

class UW_InventorySlot;

UCLASS()
class IB_MULTIPLAYGAME_API UW_PlayerInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	TObjectPtr<UW_InventorySlot> EquippedWeaponSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UW_InventorySlot> EquippedHelmetSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UW_InventorySlot> EquippedChestSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UW_InventorySlot> EquippedPantsSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UW_InventorySlot> EquippedGlovesSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UW_InventorySlot> EquippedBootsSlot;

public:
	void OnclickedEquippedButton(const FMasterItemDefinition& ItemInfo);
	UFUNCTION()
	void SetEquippedItemWidget(TArray<FMasterItemDefinition> EquippedItemsInfo);
	
	
};
