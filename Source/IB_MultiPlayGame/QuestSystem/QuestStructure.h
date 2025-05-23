#pragma once

#include "CoreMinimal.h"
#include "QuestStructure.generated.h"

UENUM()
enum class EObjectiveType : int32
{
	Location = 0,
	Kill = 1,
	Interact = 2,
	Collect = 3
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
	EObjectiveType Type;

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
	TMap<FName, int32> ItemRewards;

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
};

