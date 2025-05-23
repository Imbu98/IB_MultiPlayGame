#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../QuestSystem/QuestStructure.h"
#include "../DefineDelegates.h"
#include "QuestComponent.generated.h"

class AIB_MainChar;

USTRUCT()
struct FObjectiveProgressEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FString ObjectiveID;

	UPROPERTY()
	int32 Progress;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestComponent();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


public:
	UPROPERTY()
	FName QuestID;
	UPROPERTY()
	FQuestDetails QuestDetails;
	UPROPERTY()
	int32 CurrentStage;
	UPROPERTY()
	FStageDetails CurrentStageDetails;
	UPROPERTY()
	TMap<FString, int32> CurrentStageObjectiveProgress;
	UPROPERTY(ReplicatedUsing = OnRep_ObjectiveProgress)
	TArray<FObjectiveProgressEntry> ReplicatedObjectiveProgressArray;
	UPROPERTY()
	bool IsCompleted;

	UPROPERTY(EditAnywhere, Category = "Custom Values | DataTable")
	UDataTable* DT_QuestData;
	

	UFUNCTION()
	void OnObjectiveIDHeard(FString ObjectiveID);
	UFUNCTION()
	void GetQuestDetails();
	UFUNCTION()
	TOptional<FObjectiveDetails> GetObjectiveDataByID(FString ObjectiveID);
	UFUNCTION(Server,Reliable)
	void ServerSetOnObjectiveIdCalledDelegate(AIB_MainChar* PlayerChar);
	UFUNCTION()
	void OnRep_ObjectiveProgress();
	UFUNCTION()
	void UpdateReplicatedArray();
		
	
};
