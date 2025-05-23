#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../QuestSystem/QuestStructure.h"
#include "W_QuestLogEntry_Objectives.generated.h"

class UTextBlock;
class UCheckBox;
class AQuest_Base;
class UQuestComponent;


UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestLogEntry_Objectives : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Description;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UCheckBox> CheckBox_IsCompleted;

	UPROPERTY()
	TObjectPtr<UQuestComponent> QuestActor;

	UPROPERTY()
	FObjectiveDetails ObjectiveData;
	
};
