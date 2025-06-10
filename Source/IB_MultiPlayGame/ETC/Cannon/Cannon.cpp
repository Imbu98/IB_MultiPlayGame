#include "Cannon.h"

#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../../Character/IB_MainChar.h"

#include "GameFramework\Character.h"
#include "Kismet\GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components\WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"


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


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


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

	CannonBlockingBox = CreateDefaultSubobject<UBoxComponent>("CannonBlockingBox");
	CannonBlockingBox->SetupAttachment(DefaultSceneRoot);
	CannonBlockingBox->SetIsReplicated(true);
	

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
		
		MulticastSetMesh(CannonBodyMesh->GetStaticMesh(),CannonCartMesh->GetStaticMesh());
	}
}

void ACannon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACannon, IB_RPGPlayerController);
	DOREPLIFETIME(ACannon, IB_MainChar);
}

void ACannon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, ([this, PlayerInputComponent]
	{
			if (!IsValid(IB_RPGPlayerController)) return;
			if (!IsValid(IB_MainChar)) return;

			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(IB_RPGPlayerController->GetLocalPlayer());
			if (Subsystem)
			{
				Subsystem->AddMappingContext(IMC_Cannon, 1);
			}
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				EnhancedInputComponent->BindAction(IA_CannonCameraMove, ETriggerEvent::Triggered, this, &ThisClass::CannonCameraMove);
				EnhancedInputComponent->BindAction(IA_CannonRotation, ETriggerEvent::Triggered, this, &ThisClass::CannonRotation);
				EnhancedInputComponent->BindAction(IA_CannonShoot, ETriggerEvent::Triggered, this, &ThisClass::ChargeCannonPower);
				EnhancedInputComponent->BindAction(IA_CannonShoot, ETriggerEvent::Completed, this, &ThisClass::ShootChar);
				EnhancedInputComponent->BindAction(IA_TakeOff, ETriggerEvent::Triggered, this, &ThisClass::CannonTakeOff);
			}
	}), 1.5f, false);

	

}

FString ACannon::InteractWith_Implementation(APlayerController* PlayerController)
{
	if (!HasAuthority()) return FString();

	if (IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(PlayerController))
	{
		IB_MainChar = Cast<AIB_MainChar>(IB_RPGPlayerController->GetPawn());
		

		if (!IsValid(IB_RPGPlayerController)) return FString();
		if (!IsValid(IB_MainChar)) return FString();

		ClientSetCannonInfo(IB_RPGPlayerController, IB_MainChar);

		IB_RPGPlayerController->OwningCannon = this;
		IB_RPGPlayerController->SwitchController();

		
		return FString();
	}
	return FString();
}


void ACannon::ClientSetCannonInfo_Implementation(AIB_RPGPlayerController* IB_PlayerController, AIB_MainChar* MainChar)
{
	IB_RPGPlayerController = IB_PlayerController;
	IB_MainChar = MainChar;
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
void ACannon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ACannon::MulticastSetMesh_Implementation(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh)
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

void ACannon::CannonRotation(const FInputActionValue& Value)
{
	if (IsOnCharging)
	{
		return;
	}
	float InputValue = Value.Get<float>();
	FRotator NewRotation = CannonBodyMesh->GetRelativeRotation();
	NewRotation.Roll = NewRotation.Roll + InputValue * CannonRotationSpeed * GetWorld()->GetDeltaSeconds();
	CannonBodyMesh->SetRelativeRotation(NewRotation);
}


void ACannon::CannonCameraMove(const FInputActionValue& Value)
{
	if (IsOnCharging)
	{
		return;
	}
	FVector2D Input = CameraMoveSpeed * Value.Get<FVector2D>();

	FVector CameraOffset = FVector(Input.Y, Input.X, 0) * 200.f * GetWorld()->GetDeltaSeconds();
	FollowCamera->AddLocalOffset(CameraOffset, true);
}

void ACannon::ChargeCannonPower()
{
	IsOnCharging = true;
	CurrentCannonPower += ChargePowerSpeed * GetWorld()->GetDeltaSeconds();
	if (CurrentCannonPower >= MaxCannonPower)
	{
		CurrentCannonPower = MaxCannonPower;
	}
	//UpdateChargeBar();

}

void ACannon::ShootChar()
{
	if (IB_MainChar)
	{
		if (CannonMuzzle != nullptr)
		{
			//float ApplyWeight = IBChar->InventoryComponents->InventoryWeightAmount;

			FVector ForwardVector = CannonMuzzle->GetForwardVector() * (CurrentCannonPower);
			//FVector UpVector = CannonMuzzle->GetUpVector()*(CurrentCannonPower);
			FVector ShootingVector = (ForwardVector * 100.f);//(ApplyWeight/50.f);
			IB_MainChar->SetActorLocation(CannonMuzzle->GetComponentLocation());
			IB_MainChar->LaunchCharacter(ShootingVector, true, true);
			if (IB_RPGPlayerController)
			{
				IB_RPGPlayerController->SwitchController();
			}
			//IB_MainChar->IsFlying = true;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					//this->IB_MainChar->PlayFlyingAnimation();
				}, 0.1f, false);


			CurrentCannonPower = 0.0f;
			IsOnCharging = false;

		}
	}
	//UpdateChargeBar();
}

void ACannon::CannonTakeOff()
{
	ServerSwithchController();
}

void ACannon::ServerSwithchController_Implementation()
{
	if (IB_RPGPlayerController)
	{
		IB_RPGPlayerController->SwitchController();
	}
}
