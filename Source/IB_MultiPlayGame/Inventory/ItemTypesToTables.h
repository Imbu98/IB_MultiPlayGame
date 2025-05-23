#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ItemTypesToTables.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API UItemTypesToTables : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag,TObjectPtr<UDataTable>> TagsToTables;
	
	
};
