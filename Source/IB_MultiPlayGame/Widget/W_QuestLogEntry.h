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
	UPROPERTY()
	FName QuestID;
	UPROPERTY()
	TObjectPtr<UQuestComponent> QuestBase;

	UPROPERTY(EditAnywhere,Category="Custom Values | Actor")
	UDataTable* DT_QuestData;

	FOnQuestSelected QuestSelectedDelegate;


public:
	UFUNCTION()
	void OnClickedQuestLogEntryButton();

	
};
