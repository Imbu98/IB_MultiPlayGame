#pragma once

#include "CoreMinimal.h"
#include "../IB_GAS/IB_RPGGameMode.h"
#include "IB_RPGLobbyGameMode.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API AIB_RPGLobbyGameMode : public AIB_RPGGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	
};
