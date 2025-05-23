#include "QuestComponent.h"
#include "../Character/IB_MainChar.h"
#include "../IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Net/UnrealNetwork.h"


UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AIB_RPGPlayerController* IB_PC= Cast<AIB_RPGPlayerController>(GetOwner()))
	{
		if (IB_PC->IsLocalController())
		{
			if (AIB_MainChar* PlayerChar = UIB_BlueprintFunctionLibrary::GetMyPlayerCharacter(GetWorld()))
			{
				if (!PlayerChar->IsLocallyControlled())
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("PlayerChar is not localplayer")));
				}
				else
				{
					ServerSetOnObjectiveIdCalledDelegate(PlayerChar);
					
				}

			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Server QuestBase has authority")));
			return;
		}
		
	}

}

void UQuestComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UQuestComponent,ReplicatedObjectiveProgressArray);

}

void UQuestComponent::OnObjectiveIDHeard(FString ObjectiveID)
{
	if (CurrentStageObjectiveProgress.Find(ObjectiveID))
	{

		if (int32* FoundValue = CurrentStageObjectiveProgress.Find(ObjectiveID))
		{
			int32 Index = *FoundValue;
			FObjectiveDetails Details = GetObjectiveDataByID(ObjectiveID).GetValue();
			if (Index < Details.Quantity)
			{
				Index = +1;
				CurrentStageObjectiveProgress.Add(ObjectiveID, Index);
			}

		}
	}
	else
	{
		return;
	}
}

void UQuestComponent::GetQuestDetails()
{
	if (!IsValid(DT_QuestData))
	{
		return;
	}
	QuestDetails = *DT_QuestData->FindRow<FQuestDetails>(QuestID, TEXT(""));
	{
		if (!QuestDetails.Stages.IsEmpty())
		{
			CurrentStageDetails = QuestDetails.Stages[CurrentStage];
			CurrentStageObjectiveProgress.Empty();
			for (FObjectiveDetails& Objectives : CurrentStageDetails.Objectives)
			{
				CurrentStageObjectiveProgress.Add(Objectives.ObjectiveID, 0);
				
			}
		}
	}
	UpdateReplicatedArray();
}

TOptional<FObjectiveDetails> UQuestComponent::GetObjectiveDataByID(FString ObjectiveID)
{
	for (const FObjectiveDetails& StageDetails : CurrentStageDetails.Objectives)
	{
		if (StageDetails.ObjectiveID.Equals(ObjectiveID))
		{
			return StageDetails;
		}
	}
	return TOptional<FObjectiveDetails>();
}

void UQuestComponent::ServerSetOnObjectiveIdCalledDelegate_Implementation(AIB_MainChar* PlayerChar)
{
	PlayerChar->OnObjectiveIdCalledDelegate.AddUObject(this, &UQuestComponent::OnObjectiveIDHeard);
}

void UQuestComponent::UpdateReplicatedArray()
{
	ReplicatedObjectiveProgressArray.Empty();
	for (auto& Elem : CurrentStageObjectiveProgress)
	{
		FObjectiveProgressEntry Entry;
		Entry.ObjectiveID = Elem.Key;
		Entry.Progress = Elem.Value;
		ReplicatedObjectiveProgressArray.Add(Entry);
	}
}

void UQuestComponent::OnRep_ObjectiveProgress()
{
	CurrentStageObjectiveProgress.Empty();
	for (auto& Entry : ReplicatedObjectiveProgressArray)
	{
		CurrentStageObjectiveProgress.Add(Entry.ObjectiveID, Entry.Progress);
	}
}