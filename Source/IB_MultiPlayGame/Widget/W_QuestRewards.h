#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../QuestSystem/QuestStructure.h"
#include "W_QuestRewards.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;
class UW_QuestLogEntry_Objectives;

UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestRewards : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestDescription;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StageDescription;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_CancleButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_ColletRewardButton;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widgets")
	TSubclassOf<UUserWidget> WBP_QuestLogEntry_ObjectivesClass;

	UPROPERTY()
	TObjectPtr<UW_QuestLogEntry_Objectives> WBP_QuestLogEntry_Objectives;

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



};
