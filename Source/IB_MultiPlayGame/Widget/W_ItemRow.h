//#pragma once
//
//#include "CoreMinimal.h"
//#include "../DefineDelegates.h"
//#include "W_RPGSystemWidget.h"
//#include "W_ItemRow.generated.h"
//
//class UTextBlock;
//struct FGameplayTag;
//class UButton;
//struct FMasterItemDefinition;
//
//UENUM()
//enum EItemTypes : int8
//{
//	None = 0,
//	Consumable = 1,
//};
//
//UCLASS()
//class IB_MULTIPLAYGAME_API UW_ItemRow : public UW_RPGSystemWidget
//{
//	GENERATED_BODY()
//
//public:
//	virtual void NativeConstruct() override;
//
//	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
//	TObjectPtr<UTextBlock> TXT_ItemNameText;
//	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
//	TObjectPtr<UTextBlock> TXT_QuantityText;
//	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
//	TObjectPtr<UTextBlock> TXT_ActionText;
//	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
//	TObjectPtr<UButton> BTN_ActionButton;
//
//
//	FOnClickedActionButton OnClickedActionButtonDelegate;
//
//	
//	FMasterItemDefinition Item;
//
//	UFUNCTION()
//	void SetActionText(const FGameplayTag& CategoryTag);
//	UFUNCTION()
//	void SetItemNameText(FText ItemName);
//	UFUNCTION()
//	void SetQuiantityText(int32 Quantity);
//
//	UFUNCTION()
//	void OnclickedActionButton();
//
//	EItemTypes FilterCategoryTag(const FGameplayTag& Tag);
//	
//};
