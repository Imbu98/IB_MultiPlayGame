#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../QuestSystem/QuestStructure.h"
#include "W_QuestGiver.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;
class UW_QuestLogEntry_Objectives;
class UWrapBox;
class UW_ItemInfoSlot;
class UInventoryComponent;

UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestGiver : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestDescription;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StageDescription;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_DeclineButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_AcceptButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UWrapBox> WrapBox_ItemRewardsBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Objectives;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widgets")
	TSubclassOf<UUserWidget> WBP_QuestLogEntry_ObjectivesClass;

	UPROPERTY()
	TObjectPtr<UW_QuestLogEntry_Objectives> WBP_QuestLogEntry_Objectives;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widgets")
	TSubclassOf<UUserWidget> WBP_ItemInfoSlotClass;

	UPROPERTY()
	TObjectPtr<UW_ItemInfoSlot> WBP_ItemInfoSlot;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere)
	FQuestDetails QuestDetails;
	UPROPERTY(EditAnywhere)
	FName QuestID;

public:
	UFUNCTION()
	void OnClickDeclineButton();
	UFUNCTION()
	void OnClickAcceptButton();
	UFUNCTION()
	void SetWidgetProPerty();
	UFUNCTION()
	void SetInventoryRewards();

};
