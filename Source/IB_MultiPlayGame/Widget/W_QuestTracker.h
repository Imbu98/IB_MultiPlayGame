#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DefineDelegates.h"
#include "W_QuestTracker.generated.h"

class UTextBlock;
class UVerticalBox;
class UQuestComponent;
class UW_QuestLogEntry_Objectives;
class UW_QuestLog;
class AIB_RPGPlayerController;

UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestTracker : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	

public:
		
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UTextBlock> Textblock_QuestName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Objectives;

	UPROPERTY()
	TObjectPtr<UQuestComponent> QuestComponent;

	UPROPERTY()
	TObjectPtr<AIB_RPGPlayerController> IB_RPGPlayerController;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widgets")
	TSubclassOf<UUserWidget> WBP_QuestLogEntry_ObjectivesClass;

	UPROPERTY()
	TObjectPtr<UW_QuestLogEntry_Objectives> WBP_QuestLogEntry_Objectives;

public:
	UFUNCTION()
	void Update(UQuestComponent* Quest);
	UFUNCTION()
	void QuestCompleted(UQuestComponent* Quest);

	
};
