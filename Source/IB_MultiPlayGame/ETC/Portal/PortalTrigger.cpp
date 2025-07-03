#include "PortalTrigger.h"
#include "../../IB_Framework/IB_GameInstanceSubSystem.h"

#include "Components/BoxComponent.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GameInstance.h"


APortalTrigger::APortalTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(DefaultSceneRoot);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(GetRootComponent());

	BoardingTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardingTriggerBox"));
	BoardingTriggerBox->SetupAttachment(GetRootComponent());

	BoardingTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APortalTrigger::OnComponentBeginOverlap);

	
}

void APortalTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void APortalTrigger::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return; // ������ ó��

	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		APlayerController* PC = Cast<APlayerController>(Pawn->GetController());

		UIB_GameInstance* GI = Cast<UIB_GameInstance>(GetGameInstance());
		if (GI)
		{
			GI->FindOrCreateDungeonSession(DungeonID, PC);
		}
	}
}

