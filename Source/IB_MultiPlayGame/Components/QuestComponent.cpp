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
	DOREPLIFETIME(UQuestComponent, IsCompleted);
	

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

	
	if (!CurrentStageObjectiveProgress.Find(ObjectiveID)) return;

	if (int32* FoundValue = CurrentStageObjectiveProgress.Find(ObjectiveID))
	{
		// Value가 0보다 크면 1, 0보다 작으면 -1, 나머지는 0
		if (UKismetMathLibrary::SignOfInteger(Value) > 0)
		{
			int32 Index = *FoundValue;
			FObjectiveDetails Details = GetObjectiveDataByID(ObjectiveID).GetValue();
			if (Index < Details.Quantity)
			{

				Index += Value;
				CurrentStageObjectiveProgress.Add(ObjectiveID, Index);
				 IsObjectiveComplete(ObjectiveID);
			}

		}
		else
		{
			int32 Index = *FoundValue + Value;
			CurrentStageObjectiveProgress.Add(ObjectiveID, Index);
	
		}
	}

}


void UQuestComponent::GetQuestDetails()
{
	if (!IsValid(DT_QuestData))return;
	
	if (QuestID.IsNone()) return;

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
	// 미리 인벤토리에 아이템이 들어가있을때
	for (FObjectiveDetails& ObjectiveDetail : CurrentStageDetails.Objectives)
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
	IsCompleted = AreObjectivesComplete();
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

void UQuestComponent::IsObjectiveComplete(FString ObjectiveID)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerIsObjectiveComplete(ObjectiveID);
		return;
	}
	FObjectiveDetails ObjectiveDetails = GetObjectiveDataByID(ObjectiveID).GetValue();
	if (CurrentStageObjectiveProgress.Find(ObjectiveID))
	{
		if (int32* FoundValue = CurrentStageObjectiveProgress.Find(ObjectiveID))
		{
			if (*FoundValue >= ObjectiveDetails.Quantity)
			{
					if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(this->GetOwner()))
					{
						IB_RPGPlayerController->ClientDisplayNotification(ObjectiveDetails);
						IsCompleted = AreObjectivesComplete();
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

	for (FObjectiveDetails ObjectiveDetails : CurrentStageDetails.Objectives)
	{
		FObjectiveDetails Details = GetObjectiveDataByID(ObjectiveDetails.ObjectiveID).GetValue();

		if (CurrentStageObjectiveProgress.Find(Details.ObjectiveID))
		{
			if (int32* FoundValue = CurrentStageObjectiveProgress.Find(Details.ObjectiveID))
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
	return LocalAllComplete;
}


void UQuestComponent::ServerSetOnObjectiveIdCalledDelegate_Implementation(AIB_MainChar* PlayerChar)
{
	if (IsValid(PlayerChar) && GetOwner()->HasAuthority())
	{
		PlayerChar->OnObjectiveIdCalledDelegate.AddUObject(this, &UQuestComponent::OnObjectiveIDHeard);
	}
	
}

