#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	None,
	Common,
	Rare,
	Epic,
	Legendary,
};

USTRUCT(BlueprintType)
struct FConsumableProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> ItemEffectClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float ItemEffectLevel;

	
	FConsumableProps()
		: ItemEffectClass(nullptr)
		, ItemEffectLevel(0.0f)
	{
	}

};

USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemQuantity;

	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FConsumableProps ConsumableProps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemRarity ItemRarity;


	FMasterItemDefinition() :
		ItemTag(FGameplayTag::EmptyTag)
		, ItemQuantity(0)
		,ItemName(FText::FromString(TEXT("")))
		,Icon(nullptr)
		,Description(FText::FromString(TEXT("")))
		,ConsumableProps()
		,ItemRarity(EItemRarity::None)
	{

	}
};


