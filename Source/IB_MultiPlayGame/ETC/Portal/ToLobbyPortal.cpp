#include "ToLobbyPortal.h"


#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GameInstance.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"


AToLobbyPortal::AToLobbyPortal()
{

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(DefaultSceneRoot);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(GetRootComponent());

	BoardingTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardingTriggerBox"));
	BoardingTriggerBox->SetupAttachment(GetRootComponent());

	BoardingTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AToLobbyPortal::OnComponentBeginOverlap);

}

void AToLobbyPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

void AToLobbyPortal::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (HasAuthority()) return;

	if (OtherActor && OtherActor != this)
	{
		// 트리거한 액터가 플레이어 캐릭터인지 확인
		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
		if (PlayerCharacter)
		{
			if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(PlayerCharacter->GetController()))
			{

				if (UIB_GameInstance* IB_GameInstance = Cast<UIB_GameInstance>(GetGameInstance()))
				{
					IB_GameInstance->FindLobbySession();
				}

			}
		}
	}
}

