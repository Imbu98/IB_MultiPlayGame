#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IB_BlueprintFunctionLibrary.generated.h"

class UIB_CharacterClassInfo;

UCLASS()
class IB_MULTIPLAYGAME_API UIB_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static UIB_CharacterClassInfo* GetCharacterClassDefaultInfo(const UObject* WorldContextObject);

	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag);
	
	
};

template <typename T>
T* UIB_BlueprintFunctionLibrary::GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),FString(""));
}
