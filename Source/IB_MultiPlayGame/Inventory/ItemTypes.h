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
	FName ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FConsumableProps ConsumableProps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemRarity ItemRarity;

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ItemID;
		Ar << ItemQuantity;
		Ar << AbilityLevel;
		Ar << Weight;
	
		ItemTag.NetSerialize(Ar, Map, bOutSuccess);

		uint8 RarityByte = static_cast<uint8>(ItemRarity);
		Ar.SerializeBits(&RarityByte, 8);
		if (Ar.IsLoading())
		{
			ItemRarity = static_cast<EItemRarity>(RarityByte);
		}

		bOutSuccess = true;
		return true;
	}

	

	FMasterItemDefinition() :
		ItemTag(FGameplayTag::EmptyTag)
		, ItemQuantity(0)
		, ItemID(NAME_None)
		, AbilityLevel(0)
		,Weight(0.f)
		,Icon(nullptr)
		,Description(FText::FromString(TEXT("")))
		,ConsumableProps()
		,ItemRarity(EItemRarity::None)
	{

	}
};

template<>
struct TStructOpsTypeTraits<FMasterItemDefinition> : public TStructOpsTypeTraitsBase2<FMasterItemDefinition>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag WeaponTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageAbilityLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;
};

USTRUCT(BlueprintType)
struct FArmorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag WeaponTag;

	UPROPERTY()
	EItemRarity Rarity;

	UPROPERTY()
	int32 DefenseAbilityLevel;

	UPROPERTY()
	float Weight;
};

