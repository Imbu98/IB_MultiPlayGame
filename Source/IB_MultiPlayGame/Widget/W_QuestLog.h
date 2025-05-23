#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_QuestLog.generated.h"

class UScrollBox;
class UWidgetSwitcher;
class UTextBlock;
class UButton;
class UVerticalBox;
class UW_QuestLogEntry;
class UW_QuestLogEntry_Objectives;
class AQuest_Base;
class UQuestComponent;

UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestLog : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_CloseWidgetButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestDescription;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StageDescription;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UScrollBox> SB_MainQuests;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UScrollBox> SB_SideQuests;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Objectives;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WS_WidgetSwitcher;

	UPROPERTY(EditAnywhere,Category="Custom Values | Widgets")
	TSubclassOf<UUserWidget> WBP_QuestLogEntryClass;

	UPROPERTY()
	TObjectPtr<UW_QuestLogEntry> WBP_QuestLogEntry;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widgets")
	TSubclassOf<UUserWidget> WBP_QuestLogEntry_ObjectivesClass;

	UPROPERTY()
	TObjectPtr<UW_QuestLogEntry_Objectives> WBP_QuestLogEntry_Objectives;

	UPROPERTY(EditAnywhere, Category = "Custom Values | DataTable")
	TObjectPtr<UDataTable> DT_QuestDataTable;


	UPROPERTY()
	TObjectPtr<UQuestComponent> CurrentQuestActor;

	UFUNCTION()
	void OnQuestSelected(FName QuestID, UQuestComponent* QuestActor);
	UFUNCTION()
	void DisplayQuest(FName QuestID, UQuestComponent* QuestActor);
	UFUNCTION()
	void OnClickedCloseWidgetBtn();
	UFUNCTION()
	void CreateEntryWidget();
	

	
};
