#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Inventory/ItemTypes.h"
#include "W_ItemInfoSlot.generated.h"

class UButton;
class UImage;
class UTextBlock;
struct FGameplayTag;
struct FMasterItemDefinition;
class UW_DraggedImageItem;
class UBorder;

UCLASS()
class IB_MULTIPLAYGAME_API UW_ItemInfoSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> IMG_SlotImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBorder> Border_Frame;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemQuantity;

	UPROPERTY(EditAnywhere, Category = "Image")
	TObjectPtr<UTexture2D> SlotItemImage;
	UPROPERTY(EditAnywhere, Category = "Image")
	TObjectPtr<UTexture2D> SlotCommonImage;
	UPROPERTY(EditAnywhere, Category = "Image")
	TObjectPtr<UTexture2D> SlotRareImage;
	UPROPERTY(EditAnywhere, Category = "Image")
	TObjectPtr<UTexture2D> SlotEpicImage;
	UPROPERTY(EditAnywhere, Category = "Image")
	TObjectPtr<UTexture2D> SlotLegendaryImage;

	FMasterItemDefinition Item;

public:
	UFUNCTION()
	void SetSlotRarityImg();

};
