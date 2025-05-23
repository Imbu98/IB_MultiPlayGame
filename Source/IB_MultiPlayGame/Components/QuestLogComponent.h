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

public:	
	UPROPERTY(EditAnywhere)
	TArray<FName> CurrentActiveQuests;
	UPROPERTY(EditAnywhere)
	TArray<FName> CompletedQuests;
	UPROPERTY(EditAnywhere)
	FName CurrentTrackedQuest;
	UPROPERTY(EditAnywhere)
	TArray<UQuestComponent*> CurrentQuests;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AQuest_Base> BP_Quest_Base;

public:
	UFUNCTION()
	void AddNewQuest(FName QuestId);
	UFUNCTION()
	void CompleteQuest(FName QuestId);
	UFUNCTION()
	bool QuaryActiveQuest(FName QuestId);
	UFUNCTION()
	void TrackQuest(FName QuestId);


		
	
};
