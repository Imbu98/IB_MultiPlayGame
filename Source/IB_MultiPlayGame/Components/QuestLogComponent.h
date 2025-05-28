#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestLogComponent.generated.h"

class AQuest_Base;
class UQuestComponent;


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestLogComponent();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	UPROPERTY(EditAnywhere,Replicated)
	TArray<FName> CurrentActiveQuests;
	UPROPERTY(EditAnywhere)
	TArray<FName> CompletedQuests;
	UPROPERTY(EditAnywhere)
	FName CurrentTrackedQuest;
	UPROPERTY(EditAnywhere)
	TArray<UQuestComponent*> CurrentQuests;
	UPROPERTY(Replicated)
	bool IsAlreadyOnQuest = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> DT_QuestData;


public:
	UFUNCTION()
	void AddNewQuest(FName QuestId);
	UFUNCTION(Server,Reliable)
	void ServerAddNewQuest(FName QuestId);
	UFUNCTION()
	void CompleteQuest(const FName& QuestId);
	UFUNCTION(Server, Reliable)
	void ServerCompleteQuest(const FName& QuestId);
	UFUNCTION()
	bool QuaryActiveQuest(const FName& QuestId);
	UFUNCTION()
	void TrackQuest(FName QuestId);
	UFUNCTION()
	UQuestComponent* GetQuestActor(FName QuestId);
	UFUNCTION()
	void TurnInQuest(const FName& QuestId);


		
	
};
