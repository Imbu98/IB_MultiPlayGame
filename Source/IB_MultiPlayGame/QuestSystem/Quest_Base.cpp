#include "Quest_Base.h"
#include "../IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../Character/IB_MainChar.h"
#include "Net/UnrealNetwork.h"

AQuest_Base::AQuest_Base()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}




void AQuest_Base::BeginPlay()
{
	Super::BeginPlay();

	GetQuestDetails();

	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Server QuestBase has authority")));
		return;
	}
	else if(!HasAuthority())
	{
		if (AIB_MainChar* PlayerChar = UIB_BlueprintFunctionLibrary::GetMyPlayerCharacter(GetWorld()))
		{
			if (!PlayerChar->IsLocallyControlled())
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("PlayerChar is not localplayer")));
			}
			else
			{
				PlayerChar->OnObjectiveIdCalledDelegate.AddUObject(this, &AQuest_Base::OnObjectiveIDHeard);
			}

		}
	}
	
}

void AQuest_Base::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQuest_Base, QuestID);

}

void AQuest_Base::OnObjectiveIDHeard(FString ObjectiveID)
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

void AQuest_Base::GetQuestDetails()
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
}

TOptional<FObjectiveDetails> AQuest_Base::GetObjectiveDataByID(FString ObjectiveID)
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

//FObjectiveDetails AQuest_Base::GetObjectiveDataByID(FString ObjectiveID)
//{
//	for (FObjectiveDetails StageDetails : CurrentStageDetails.Objectives)
//	{
//		if (StageDetails.ObjectiveID.Equals(ObjectiveID))
//		{
//			return StageDetails;
//		}
//	}
//}




