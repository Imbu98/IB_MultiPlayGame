#include "LocationMarker.h"
#include "Components/BoxComponent.h"
#include "../../Character/IB_MainChar.h"
#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"

ALocationMarker::ALocationMarker()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootSceneComponent");
	SetRootComponent(SceneComponent);

	OverlapBoxComponent = CreateDefaultSubobject<UBoxComponent>("OverlapBoxComponent");
	OverlapBoxComponent->SetupAttachment(GetRootComponent());

	OverlapBoxComponent->OnComponentBeginOverlap.Clear();
	OverlapBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	OverlapBoxComponent->OnComponentEndOverlap.Clear();
	OverlapBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

}

void ALocationMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALocationMarker::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (ObjectiveID.IsEmpty()) return;

	if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(OtherActor))
	{
		if (IB_MainChar->OnObjectiveIdCalledDelegate.IsBound())
		{
			int32 QuestSuccessDefaultValue = 1;
			IB_MainChar->OnObjectiveIdCalledDelegate.Broadcast(ObjectiveID, QuestSuccessDefaultValue);
		}
		if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(IB_MainChar->GetController()))
		{
			if (IB_RPGPlayerController->IsLocalController())
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("IB_RPGPlayerController Is LocalController"));
				return;
			}

			IB_RPGPlayerController->ClientDisplayLocationNotification(LocationName);
		}
	}
}

void ALocationMarker::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

