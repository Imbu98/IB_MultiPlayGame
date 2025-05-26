#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../QuestSystem/QuestStructure.h"
#include "../DefineDelegates.h"
#include "QuestComponent.generated.h"

class AIB_MainChar;
class UW_QuestNotification;

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
	UPROPERTY(Replicated)
	TArray<FObjectiveProgressEntry> ReplicatedObjectiveProgressArray;
	UPROPERTY()
	bool IsCompleted;

	UPROPERTY(EditAnywhere, Category = "Custom Values | DataTable")
	UDataTable* DT_QuestData;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widget")
	TSubclassOf<UW_QuestNotification> WBP_QuestNotification;

	UPROPERTY()
	TObjectPtr<UW_QuestNotification> WBP_QuestNotificationClass;

	

	UFUNCTION()
	void OnObjectiveIDHeard(FString ObjectiveID);
	UFUNCTION()
	void GetQuestDetails();
	UFUNCTION()
	TOptional<FObjectiveDetails> GetObjectiveDataByID(FString ObjectiveID);
	UFUNCTION(Server,Reliable)
	void ServerSetOnObjectiveIdCalledDelegate(AIB_MainChar* PlayerChar);
	UFUNCTION(Client, Reliable)
	void ClientOnObjectiveIDHeard(const FString& ObjectiveID);
	UFUNCTION()
	void IsObjectiveComplete(FString ObjectiveID);


	
};
