#include "QuestComponent.h"
#include "../Character/IB_MainChar.h"
#include "../IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "../Widget/W_QuestNotification.h"
#include "Kismet/KismetMathLibrary.h"


UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentQuests.SetNum(MaxQuestAcceptable);

	GetQuestDetails();

	if (AIB_RPGPlayerController* IB_PC = Cast<AIB_RPGPlayerController>(GetOwner()))
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

void UQuestComponent::OnObjectiveIDHeard(FString ObjectiveID,int32 Value)
{
	if (ObjectiveID.IsEmpty()) return;

	if (GetOwner()->HasAuthority())
	{
		// for Server
		HandleObjectiveIDHeard(ObjectiveID, Value);

		ClientOnObjectiveIDHeard(ObjectiveID);
		return;
	}

	
}

void UQuestComponent::ClientOnObjectiveIDHeard_Implementation(const FString& ObjectiveID, int32 Value)
{
	if (ObjectiveID.IsEmpty()) return;
	HandleObjectiveIDHeard(ObjectiveID, Value);
}

void UQuestComponent::HandleObjectiveIDHeard(const FString& ObjectiveID, int32 Value)
{
	if (ObjectiveID.IsEmpty()) return;

	
	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		if (!CurrentQuests[i].CurrentStageObjectiveProgress.Find(ObjectiveID)) continue;

		if (int32* FoundValue = CurrentQuests[i].CurrentStageObjectiveProgress.Find(ObjectiveID))
		{
			// Value가 0보다 크면 1, 0보다 작으면 -1, 나머지는 0
			if (UKismetMathLibrary::SignOfInteger(Value) > 0)
			{
				int32 Index = *FoundValue;

				FObjectiveDetails Details = GetObjectiveDataByID(ObjectiveID).GetValue();

				if (Index < Details.Quantity)
				{

					Index += Value;
					CurrentQuests[i].CurrentStageObjectiveProgress.Add(ObjectiveID, Index);
					IsObjectiveComplete(ObjectiveID);
				}

			}
			else
			{
				int32 Index = *FoundValue + Value;
				CurrentQuests[i].CurrentStageObjectiveProgress.Add(ObjectiveID, Index);
			}
			break;
		}
	}
	OnObjectiveHeardDelegate.Broadcast();
}


void UQuestComponent::GetQuestDetails()
{
	if (!IsValid(DT_QuestData))return;

	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		if (CurrentQuests[i].QuestID.IsNone()) continue;

		CurrentQuests[i].QuestDetails = *DT_QuestData->FindRow<FQuestDetails>(CurrentQuests[i].QuestID, TEXT(""));
		if (!CurrentQuests[i].QuestDetails.Stages.IsEmpty())
		{
			int CurrentStage = CurrentQuests[i].CurrentStage;
			CurrentQuests[i].CurrentStageDetails = CurrentQuests[i].QuestDetails.Stages[CurrentStage];

			CurrentQuests[i].CurrentStageObjectiveProgress.Empty();
			for (FObjectiveDetails& Objectives : CurrentQuests[i].CurrentStageDetails.Objectives)
			{
				CurrentQuests[i].CurrentStageObjectiveProgress.Add(Objectives.ObjectiveID, 0);
			}
		}

		// 미리 인벤토리에 아이템이 들어가있을때
		for (FObjectiveDetails& ObjectiveDetail : CurrentQuests[i].CurrentStageDetails.Objectives)
		{
			if (ObjectiveDetail.Type == EObjectiveType::Collect)
			{
				if (UInventoryComponent* InventoryComponent = GetOwner()->GetComponentByClass<UInventoryComponent>())
				{
					// 서버에 바인딩된 함수가 아니라 client에서 함수 실행중
					OnObjectiveIDHeard(ObjectiveDetail.ObjectiveID, InventoryComponent->QueryInventory(ObjectiveDetail.ObjectiveID));
				}
			}
		}
		CurrentQuests[i].IsCompleted = AreObjectivesComplete();
	}
	
}

TOptional<FObjectiveDetails> UQuestComponent::GetObjectiveDataByID(FString ObjectiveID)
{
	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		if (CurrentQuests[i].QuestID.IsNone()) return TOptional<FObjectiveDetails>();

		for (const FObjectiveDetails& StageDetails : CurrentQuests[i].CurrentStageDetails.Objectives)
		{
			if (StageDetails.ObjectiveID.Equals(ObjectiveID))
			{
				return StageDetails;
			}
		}
	}
	return TOptional<FObjectiveDetails>();
}

//server
void UQuestComponent::IsObjectiveComplete(FString ObjectiveID)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerIsObjectiveComplete(ObjectiveID);
		return;
	}

	FObjectiveDetails ObjectiveDetails = GetObjectiveDataByID(ObjectiveID).GetValue();

	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		if (CurrentQuests[i].CurrentStageObjectiveProgress.Find(ObjectiveID))
		{
			if (int32* FoundValue = CurrentQuests[i].CurrentStageObjectiveProgress.Find(ObjectiveID))
			{
				if (*FoundValue >= ObjectiveDetails.Quantity)
				{
					if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(this->GetOwner()))
					{
						IB_RPGPlayerController->ClientDisplayNotification(ObjectiveDetails);
						CurrentQuests[i].IsCompleted = AreObjectivesComplete();
					}

				}
			}
		}
	}
	
}


void UQuestComponent::ServerIsObjectiveComplete_Implementation(const FString& ObjectiveID)
{
	if (ObjectiveID.IsEmpty()) return;

	IsObjectiveComplete(ObjectiveID);
	
}

bool UQuestComponent::AreObjectivesComplete()
{
	bool LocalAllComplete = true;

	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		for (FObjectiveDetails ObjectiveDetails : CurrentQuests[i].CurrentStageDetails.Objectives)
		{
			const FObjectiveDetails Details = GetObjectiveDataByID(ObjectiveDetails.ObjectiveID).GetValue();

			if (CurrentQuests[i].CurrentStageObjectiveProgress.Find(Details.ObjectiveID))
			{
				if (int32* FoundValue = CurrentQuests[i].CurrentStageObjectiveProgress.Find(Details.ObjectiveID))
				{
					if (*FoundValue >= Details.Quantity)
					{

					}
					else
					{
						LocalAllComplete = false;
						return LocalAllComplete;
					}
				}
			}
		}
	}
	return LocalAllComplete;
}


void UQuestComponent::ServerSetOnObjectiveIdCalledDelegate_Implementation(AIB_MainChar* PlayerChar)
{
	if (IsValid(PlayerChar) && GetOwner()->HasAuthority())
	{
		PlayerChar->OnObjectiveIdCalledDelegate.AddUObject(this, &UQuestComponent::OnObjectiveIDHeard);
	}
	
}

