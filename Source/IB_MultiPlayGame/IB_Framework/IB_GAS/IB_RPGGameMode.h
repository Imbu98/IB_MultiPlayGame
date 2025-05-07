#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "IB_RPGGameMode.generated.h"

class UIB_CharacterClassInfo;

UCLASS()
class IB_MULTIPLAYGAME_API AIB_RPGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UIB_CharacterClassInfo* GetCharacterClassDefaultInfo() const;

private:

	UPROPERTY(EditDefaultsOnly,Category="Custom Values|Class Defaults")
	TObjectPtr<UIB_CharacterClassInfo> ClassDefaults;
	
	
	
};
