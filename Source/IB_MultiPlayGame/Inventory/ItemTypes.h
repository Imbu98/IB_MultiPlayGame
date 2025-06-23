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

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ItemQuantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float  ArmorDefense;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float  WeaponAttackPower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FConsumableProps ConsumableProps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemRarity ItemRarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bStackable;

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ItemID;
		Ar << ItemQuantity;
		Ar << WeaponAttackPower;
		Ar << ArmorDefense;
		Ar << Weight;
		Ar << bStackable;
	
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
		, ArmorDefense(0.f)
		, WeaponAttackPower(0.f)
		, Weight(0.f)
		, Icon(nullptr)
		, Description(FText::FromString(TEXT("")))
		, ConsumableProps()
		, ItemRarity(EItemRarity::None)
		, bStackable(false)
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
	TMap<EItemRarity, float > WeaponAttackPowerMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EItemRarity, float> WeaponWeightMap;

};

USTRUCT(BlueprintType)
struct FArmorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ArmorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EItemRarity, float> ArmorDefenseMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EItemRarity,float> ArmorWeightMap;
};

