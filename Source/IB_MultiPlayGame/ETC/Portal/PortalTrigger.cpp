#include "PortalTrigger.h"
#include "../../IB_Framework/IB_GameInstanceSubSystem.h"

#include "Components/BoxComponent.h"


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
	if (!HasAuthority()) return; // 서버만 처리

	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		APlayerController* PC = Cast<APlayerController>(Pawn->GetController());

		if (PC)
		{
			Server_TravelToDungeon(PC);
		}
	}
}

void APortalTrigger::Server_TravelToDungeon_Implementation(APlayerController* PlayerController)
{
	if (!PlayerController) return;

	UIB_GameInstanceSubSystem* Manager = GetGameInstance()->GetSubsystem<UIB_GameInstanceSubSystem>();
	if (!Manager) return;

	FString TravelURL = Manager->RequestDungeonURL(DungeonName);
	PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute,true);
	
}
