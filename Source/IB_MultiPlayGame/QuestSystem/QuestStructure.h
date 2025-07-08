#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IB_MultiPlayGame/Inventory/ItemTypes.h"
#include "QuestStructure.generated.h"

USTRUCT(BlueprintType)
struct FItemRewards
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMasterItemDefinition QuestRewardItemDefinition;
};

UENUM()
enum class EObjectiveType : int32
{
	None=0,
	Location = 1,
	Kill = 2,
	Interact = 3,
	Collect = 4
};



USTRUCT(BlueprintType)
struct FObjectiveDetails
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ObjectiveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveType Type=EObjectiveType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ObjectiveID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOptional = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 bonusXP = 0;


};

USTRUCT(BlueprintType)
struct FStageDetails
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FObjectiveDetails> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XPReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemRewards> ItemRewards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrencyRewards = 0;
};

USTRUCT(BlueprintType)
struct FQuestDetails : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText LogDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TrackingDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMainQuest = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStageDetails> Stages;

	FQuestDetails()
		: QuestName(FText::GetEmpty())
		, LogDescription(FText::GetEmpty())
		, TrackingDescription(FText::GetEmpty())
		, IsMainQuest(false)
		, Stages() 
	{

	}
};

