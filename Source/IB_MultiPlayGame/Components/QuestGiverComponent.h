#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interfaces/InteractInterface.h"
#include "../QuestSystem/QuestStructure.h"
#include "QuestGiverComponent.generated.h"

class UW_QuestGiver;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UQuestGiverComponent : public UActorComponent , public IInteractInterface
{
	GENERATED_BODY()

public:	
	UQuestGiverComponent();

protected:
	virtual void BeginPlay() override;

	virtual FString InteractWith_Implementation(APlayerController* CharacterController) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest",meta =(AllowPrivateAccess=true))
	FDataTableRowHandle QuestRowHandle;

public:
	UPROPERTY()
	bool bAlreadyQuest=false;
	

};
