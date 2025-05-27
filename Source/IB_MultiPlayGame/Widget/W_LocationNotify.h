#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_LocationNotify.generated.h"

class UTextBlock;

UCLASS()
class IB_MULTIPLAYGAME_API UW_LocationNotify : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_LocationName;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = Widget, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WAM_FlyIn;

public:
	UPROPERTY()
	FText LocationName;
	
};
