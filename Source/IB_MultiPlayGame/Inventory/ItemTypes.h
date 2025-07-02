#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class ESlotTypes : uint8
{
	Slot_None = 0,
	Slot_Equippable = 1,
	Slot_Consumable = 2,
	Slot_ETC = 3,
	Slot_PlayerInfo = 4,
};

UENUM(BlueprintType)
enum class EItemTypes : uint8
{
	Item_None=0,
	Item_Equippable = 1,
	Item_Consumable = 2,
	Item_ETC=3,
};

UENUM(BlueprintType)
enum class EItemParts : uint8
{
	None,
	Weapon,
	Helmet,
	Chest,
	Gloves,
	Pants,
	Boots,
	ForLastItemIndex,
};

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
	EItemParts ItemParts;

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

		uint8 PartsByte = static_cast<uint8>(ItemParts);
		Ar.SerializeBits(&PartsByte, 8);
		if (Ar.IsLoading())
		{
			ItemParts = static_cast<EItemParts>(PartsByte);
		}

		bOutSuccess = true;
		return true;
	}

	bool operator==(const FMasterItemDefinition& Other) const
	{
		return ItemTag == Other.ItemTag; 
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
		, ItemParts(EItemParts::None)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemParts WeaponParts;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemParts ArmorParts;
};

