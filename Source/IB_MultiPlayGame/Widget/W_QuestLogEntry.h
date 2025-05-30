#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DefineDelegates.h"
#include "W_QuestLogEntry.generated.h"

class UTextBlock;
class UCheckBox;
class UButton;
class AQuest_Base;
class UQuestComponent;

UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestLogEntry : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_QuestLogEntry;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UTextBlock> Textblock_QuestName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UCheckBox> CheckBox_IsComplete;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> Btn_QuestTrack;

	UPROPERTY()
	TObjectPtr<AIB_RPGPlayerController> IB_RPGPlayerController;


	UPROPERTY()
	FName QuestID;
	UPROPERTY()
	FActiveQuestData ActiveQuestData;

	UPROPERTY(EditAnywhere,Category="Custom Values | Actor")
	UDataTable* DT_QuestData;

	FOnQuestSelected QuestSelectedDelegate;
	FOnQuestTrack QuestTrackDelegate;


public:
	UFUNCTION()
	void OnClickedQuestLogEntryButton();
	UFUNCTION()
	void OnClickedQuestTrackButton();
	UFUNCTION()
	void BindingQuestCompletedDelegate();
	UFUNCTION()
	void QuestCompleted(const FActiveQuestData& ActiveQuest);

	
};
