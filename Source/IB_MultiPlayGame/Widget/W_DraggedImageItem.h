#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_DraggedImageItem.generated.h"

class UImage;

UCLASS()
class IB_MULTIPLAYGAME_API UW_DraggedImageItem : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(meta=(Bindwidget))
	TObjectPtr<UImage> Img_DraggedImage;

	UPROPERTY()
	TObjectPtr<UTexture2D> ItemImage;
	

};
