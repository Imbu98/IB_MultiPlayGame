#include "Cannon.h"

#include "GameFramework\Character.h"
#include "Kismet\GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components\WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"


ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	bNetUseOwnerRelevancy = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SetRootComponent(DefaultSceneRoot);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetIsReplicated(true);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetIsReplicated(true);

	CannonCartMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonCartMesh");
	CannonCartMesh->SetupAttachment(GetRootComponent());
	CannonCartMesh->SetIsReplicated(true);

	CannonBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonBodyMesh");
	CannonBodyMesh->SetupAttachment(CannonCartMesh);
	CannonBodyMesh->SetIsReplicated(true);

	CannonMuzzle = CreateDefaultSubobject<USceneComponent>("CannonMuzzle");
	CannonMuzzle->SetupAttachment(CannonBodyMesh);

	BoardingTriggerBox = CreateDefaultSubobject<UBoxComponent>("BoardingTriggerBox");
	BoardingTriggerBox->SetupAttachment(DefaultSceneRoot);
	BoardingTriggerBox->SetIsReplicated(true);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystemComponent");
	ParticleSystemComponent->SetupAttachment(BoardingTriggerBox);
	ParticleSystemComponent->SetIsReplicated(true);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(DefaultSceneRoot);
	WidgetComponent->SetIsReplicated(true);

	BoardingTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	BoardingTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

}

void ACannon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Owner: %s, HasAuthority: %d"),*GetOwner()->GetName(), HasAuthority());
		MulticastSetMesh(CannonBodyMesh->GetStaticMesh(),CannonCartMesh->GetStaticMesh());
	}

}

void ACannon::BeginDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("BeginDestroy called on %s"), *GetName());
	Super::BeginDestroy();
}


void ACannon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FString ACannon::InteractWith_Implementation(APlayerController* PlayerController)
{
	return FString();
}

void ACannon::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy: %s"), *NewController->GetName());
}


void ACannon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
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
					 InteractWidget에 만든 함수 사용 (예: SetInteractionText)
					InteractWidget->SetInteractionText(FText::FromString(TEXT("Press E to fire")));
				}*/
			}
			WidgetComponent->SetVisibility(true);
		}
	}
}
void ACannon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
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
}

void ACannon::MulticastSetMesh_Implementation(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh)
{
	if (HasAuthority())
	{
		if (CannonBodyMesh && CannonCartMesh&& BoardingTriggerBox)
		{
			CannonBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CannonCartMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BoardingTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	if (InCannonBodyMesh && InCannonCartMesh && CannonBodyMesh && CannonCartMesh)
	{
		CannonBodyMesh->SetStaticMesh(InCannonBodyMesh);
		CannonCartMesh->SetStaticMesh(InCannonCartMesh);
	}
}
