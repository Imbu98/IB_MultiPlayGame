#pragma once

#include "GameplayTagContainer.h"
#include "../../Inventory/ItemTypes.h"
#include "CoreMinimal.h"
#include "SpawnedItemSetProperty.generated.h"

USTRUCT(BlueprintType)
struct FItemProperty : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ItemTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh=nullptr;

};

USTRUCT(BlueprintType)
struct FItemTypeProbability : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameplayTag ItemTag;
	UPROPERTY(EditAnywhere)
	float ItemTypeProbability=0.f;

};

USTRUCT(BlueprintType)
struct FItemRarityProbability : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EItemRarity ItemRarity=EItemRarity::None;
	UPROPERTY(EditAnywhere)
	float ItemRarityProbability=0.f;

};