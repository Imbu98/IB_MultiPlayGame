#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../IB_GAS/AbilitySystem/IB_RPGAbilityTypes.h"
#include "IB_BlueprintFunctionLibrary.generated.h"

class UIB_CharacterClassInfo;
class UProjectile_Info;
class UWeapon_Info;
class AIB_MainChar;
class AIB_RPGPlayerController;


UCLASS()
class IB_MULTIPLAYGAME_API UIB_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// AbilitySystem FunctionLibaray//
	UFUNCTION(BlueprintPure)
	static UIB_CharacterClassInfo* GetCharacterClassDefaultInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static UProjectile_Info* GetProjectileInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static UWeapon_Info* GetWeaponInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void ApplyDamageEffect(const FDamageEffectInfo& DamageEffectInfo);

	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag);

public:
	// QuestSystem FunctionLibaray//
	UFUNCTION(BlueprintPure)
	static AIB_MainChar* GetMyPlayerCharacter(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure)
	static AGameModeBase* GetMyGameMode(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure)
	static AIB_RPGPlayerController* GetMyPlayerController(const UObject* WorldContextObject);
	
	
};

template <typename T>
T* UIB_BlueprintFunctionLibrary::GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),FString(""));
}
