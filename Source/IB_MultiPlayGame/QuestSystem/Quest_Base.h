//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "QuestStructure.h"
//#include "../DefineDelegates.h"
//#include "Quest_Base.generated.h"
//
//UCLASS()
//class IB_MULTIPLAYGAME_API AQuest_Base : public AActor
//{
//	GENERATED_BODY()
//	
//public:	
//	AQuest_Base();
//
//protected:
//	virtual void BeginPlay() override;
//
//	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
//
//public:
//	UPROPERTY(Replicated)
//	FName QuestID;
//	UPROPERTY()
//	FQuestDetails QuestDetails;
//	UPROPERTY()
//	int32 CurrentStage;
//	UPROPERTY()
//	FStageDetails CurrentStageDetails;
//	UPROPERTY()
//	TMap<FString, int32> CurrentStageObjectiveProgress;
//	UPROPERTY()
//	bool IsCompleted;
//	UPROPERTY(EditAnywhere, Category= "Custom Values | DataTable")
//	UDataTable* DT_QuestData;
//
//	
//
//public:
//	UFUNCTION()
//	void OnObjectiveIDHeard(FString ObjectiveID);
//	UFUNCTION()
//	void GetQuestDetails();
//	UFUNCTION()
//	TOptional<FObjectiveDetails> GetObjectiveDataByID(FString ObjectiveID);
//
//	
//
//	
//	
//	
//};
