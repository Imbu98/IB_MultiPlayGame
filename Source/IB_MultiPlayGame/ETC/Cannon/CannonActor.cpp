#include "CannonActor.h"

#include "GameFramework\Character.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "IB_MultiPlayGame/Components/CannonSpawnComponent.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Kismet/GameplayStatics.h"


ACannonActor::ACannonActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	bNetUseOwnerRelevancy = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SetRootComponent(DefaultSceneRoot);

	CannonCartMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonCartMesh");
	CannonCartMesh->SetupAttachment(GetRootComponent());
	CannonCartMesh->SetIsReplicated(true);

	CannonBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonBodyMesh");
	CannonBodyMesh->SetupAttachment(CannonCartMesh);
	CannonBodyMesh->SetIsReplicated(true);
	
	BoardingTriggerBox = CreateDefaultSubobject<UBoxComponent>("BoardingTriggerBox");
	BoardingTriggerBox->SetupAttachment(DefaultSceneRoot);
	BoardingTriggerBox->SetIsReplicated(true);

	CannonBlockingBox = CreateDefaultSubobject<UBoxComponent>("CannonBlockingBox");
	CannonBlockingBox->SetupAttachment(DefaultSceneRoot);
	CannonBlockingBox->SetIsReplicated(true);

	BoardingTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	BoardingTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

}

void ACannonActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MulticastSetMesh(CannonBodyMesh->GetStaticMesh(),CannonCartMesh->GetStaticMesh());
	}
}

FString ACannonActor::InteractWith_Implementation(APlayerController* PlayerController)
{
	if (!HasAuthority()) return FString();
	if (PlayerController!=GetOwner()) return FString();
	
	if (AIB_RPGPlayerController* IBRPGlayerController = Cast<AIB_RPGPlayerController>(PlayerController))
	{
		if (UCannonSpawnComponent* CannonSpawnComponent= IBRPGlayerController->GetCannonSpawnComponent())
		{
			CannonSpawnComponent->SpawnOwnedCannonPawn(IBRPGlayerController);
			SetLifeSpan(0.1f);
			return ObjectiveID;
		}
	}
	return "";
}

void ACannonActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (OtherActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_SetNPCActor(OtherActor, this);
		}
	}
	if (ItemOverlayMaterial && CannonBodyMesh&& CannonCartMesh)
	{
		CannonBodyMesh->SetOverlayMaterial(ItemOverlayMaterial);
		CannonCartMesh->SetOverlayMaterial(ItemOverlayMaterial);
	}
	if (WidgetComponent)
	{
		UUserWidget* Widget = WidgetComponent->GetUserWidgetObject();
		if (Widget)
		{
			/* Cast to your custom widget class
			if (UInteractWidget* InteractWidget = Cast<UInteractWidget>(Widget))
			{
				 InteractWidget�� ���� �Լ� ��� (��: SetInteractionText)
				InteractWidget->SetInteractionText(FText::FromString(TEXT("Press E to fire")));
			}*/
		}
		WidgetComponent->SetVisibility(true);
	}

}

void ACannonActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{

		if (OtherActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_SetNPCActor(OtherActor, nullptr);
		}
	}
	if (ItemOverlayMaterial && CannonBodyMesh && CannonCartMesh)
	{
		CannonBodyMesh->SetOverlayMaterial(nullptr);
		CannonCartMesh->SetOverlayMaterial(nullptr);
	}
	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
}

void ACannonActor::MulticastSetMesh_Implementation(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh)
{
	if (HasAuthority())
	{
		if (CannonBodyMesh && CannonCartMesh && BoardingTriggerBox)
		{
			CannonBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CannonCartMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BoardingTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			return;
		}
	}
	if (InCannonBodyMesh && InCannonCartMesh && CannonBodyMesh && CannonCartMesh)
	{
		// Is local player
		if (GetOwner())
		{
			UE_LOG(LogTemp, Warning, TEXT("Owner: %s, HasAuthority: %d"), *GetOwner()->GetName(), HasAuthority());
			CannonBodyMesh->SetStaticMesh(InCannonBodyMesh);
			CannonCartMesh->SetStaticMesh(InCannonCartMesh);
		}
		else
		{
			CannonBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CannonCartMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BoardingTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UE_LOG(LogTemp, Warning, TEXT("Cannon has No Owner"));
		}

	}
}

