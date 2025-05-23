#include "IB_NPCBase.h"
#include "../Components/QuestGiverComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Interfaces/InteractInterface.h"
#include "Net/UnrealNetwork.h"

AIB_NPCBase::AIB_NPCBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

	QuestGiverComponent = CreateDefaultSubobject<UQuestGiverComponent>("QuestGiverComponent");
	QuestGiverComponent->SetIsReplicated(true);

	WBP_Interact = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WBP_Interact->SetupAttachment(GetMesh());

	if (WBP_Interact)
	{
		WBP_Interact->SetVisibility(false);
	}
}

FString AIB_NPCBase::InteractWith_Implementation(APlayerController* CharacterController)
{
	if (IsValid(CharacterController))
	{
		if (IsValid(QuestGiverComponent) && QuestGiverComponent	->Implements<UInteractInterface>())
		{
			return IInteractInterface::Execute_InteractWith(QuestGiverComponent, CharacterController);
		}
		return "";
	}
	return "";
}

void AIB_NPCBase::LookAt_Implementation()
{

}

void AIB_NPCBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIB_NPCBase, InteractedActor);
}

void AIB_NPCBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (OtherActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_SetNPCActor(OtherActor, this);
		}
		if (WBP_Interact)
		{
			WBP_Interact->SetVisibility(true);
		}
		if (InteractOverlayMaterial)
		{
			GetMesh()->SetOverlayMaterial(InteractOverlayMaterial);
		}
	}
	if (!HasAuthority())
	{
		return;
	}
	InteractedActor = OtherActor;

}

void AIB_NPCBase::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (OtherActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_SetNPCActor(OtherActor, nullptr);
		}
		if (WBP_Interact)
		{
			WBP_Interact->SetVisibility(false);
		}

		GetMesh()->SetOverlayMaterial(nullptr);
	}
	if (!HasAuthority())
	{
		return;
	}
	InteractedActor = nullptr;

}

void AIB_NPCBase::OnRep_InteractedActor()
{

}
