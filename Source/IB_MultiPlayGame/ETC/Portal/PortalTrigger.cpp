#include "PortalTrigger.h"
#include "../../IB_Framework/IB_GameInstanceSubSystem.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GameInstance.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"


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
	if (!HasAuthority) return;

	if (OtherActor && OtherActor != this)
	{
		// 트리거한 액터가 플레이어 캐릭터인지 확인
		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
		if (PlayerCharacter)
		{
			if (UIB_GameInstance* IB_GameInstance = Cast<UIB_GameInstance>(GetGameInstance()))
			{
				IB_GameInstance->RequestCreateDungeonSession(TargetDungeonName);
			}
		}
	}
}

