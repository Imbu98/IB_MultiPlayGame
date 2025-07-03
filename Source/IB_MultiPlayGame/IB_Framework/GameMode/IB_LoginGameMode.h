#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "IB_LoginGameMode.generated.h"

class UW_LogInWidget;

UCLASS()
class IB_MULTIPLAYGAME_API AIB_LoginGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
