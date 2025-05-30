#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../QuestSystem/QuestStructure.h"
#include "../DefineDelegates.h"
#include "QuestComponent.generated.h"

class AIB_MainChar;
class UW_QuestNotification;

USTRUCT(BlueprintType)
struct FActiveQuestData
{
	GENERATED_BODY()

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
	UPROPERTY()
	bool IsCompleted;

	bool operator==(const FActiveQuestData& Other) const
	{
		return QuestID == Other.QuestID;
	}

	FActiveQuestData()
		: QuestID(NAME_None)
		, QuestDetails()                // 기본 생성자 호출
		, CurrentStage(0)
		, CurrentStageDetails()         // 기본 생성자 호출
		, CurrentStageObjectiveProgress() // 빈 맵 초기화
		, IsCompleted(false)
	{
	}
};

DECLARE_MULTICAST_DELEGATE(FOnObjectiveHeard);

USTRUCT()
struct FObjectiveProgressEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FString ObjectiveID;

	UPROPERTY()
	int32 Progress;
};

UCLASS(Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestComponent();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


public:
	/*UPROPERTY()
	FName QuestID;
	UPROPERTY()
	FQuestDetails QuestDetails;*/
	UPROPERTY()
	TArray<FActiveQuestData> CurrentQuests;
	
	
	
	UPROPERTY(Replicated)
	TArray<FObjectiveProgressEntry> ReplicatedObjectiveProgressArray;
	
	UPROPERTY()
	int32 MaxQuestAcceptable=15;

	UPROPERTY(EditAnywhere, Category = "Custom Values | DataTable")
	UDataTable* DT_QuestData;

	FOnObjectiveHeard OnObjectiveHeardDelegate;

	

	UFUNCTION()
	void OnObjectiveIDHeard(FString ObjectiveID, int32 Value=1);
	UFUNCTION()
	void GetQuestDetails();
	UFUNCTION()
	TOptional<FObjectiveDetails> GetObjectiveDataByID(FString ObjectiveID);
	UFUNCTION(Server,Reliable)
	void ServerSetOnObjectiveIdCalledDelegate(AIB_MainChar* PlayerChar);
	UFUNCTION(Client, Reliable)
	void ClientOnObjectiveIDHeard(const FString& ObjectiveID, int32 Value = 1);
	UFUNCTION()
	void IsObjectiveComplete(FString ObjectiveID);
	UFUNCTION(Server,Reliable)
	void ServerIsObjectiveComplete(const FString& ObjectiveID);
	UFUNCTION()
	bool AreObjectivesComplete();

	void HandleObjectiveIDHeard(const FString& ObjectiveID, int32 Value = 1);

	
};
