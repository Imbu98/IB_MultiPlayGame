#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class IB_MULTIPLAYGAME_API IInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Interaction(AActor* SourceActor);

	UFUNCTION(BlueprintNativeEvent)
	void LookAt();

	UFUNCTION(BlueprintNativeEvent)
	FString InteractWith(APlayerController* PlayerController);

	UFUNCTION(BlueprintNativeEvent)
	void SetNPCActor(AActor* NPCActor);
	
	
};
