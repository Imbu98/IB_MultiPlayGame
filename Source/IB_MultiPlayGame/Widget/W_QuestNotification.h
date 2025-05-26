#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_QuestNotification.generated.h"

class UTextBlock;

UCLASS()
class IB_MULTIPLAYGAME_API UW_QuestNotification : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(EditAnywhere,meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ObjectiveText;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = Widget, meta = (BindWidgetAnim))
	UWidgetAnimation* WAM_Appear;
	UPROPERTY()
	FText ObjectiveText;
	
};
