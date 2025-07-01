#include "CannonPawn.h"

#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../../Character/IB_MainChar.h"
#include "../../Widget/W_CannonWidget.h"

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
#include "IB_MultiPlayGame/Components/CannonSpawnComponent.h"
#include "Net/UnrealNetwork.h"


ACannonPawn::ACannonPawn()
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

	

}

void ACannonPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MulticastSetMesh(CannonBodyMesh->GetStaticMesh(),CannonCartMesh->GetStaticMesh());
	}
}

void ACannonPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACannonPawn, IB_RPGPlayerController);
	DOREPLIFETIME(ACannonPawn, IB_MainChar);
	DOREPLIFETIME(ACannonPawn, CurrentCannonPower);
	DOREPLIFETIME(ACannonPawn, IsOnCharging);
	
	
}

void ACannonPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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


void ACannonPawn::ClientSetCannonInfo_Implementation(AIB_RPGPlayerController* IB_PlayerController, AIB_MainChar* MainChar)
{
	IB_RPGPlayerController = IB_PlayerController;
	IB_MainChar = MainChar;

	if (!HasAuthority())
	{
		ServerSetCannonInfo(IB_PlayerController, MainChar);
	}
}
void ACannonPawn::ServerSetCannonInfo_Implementation(AIB_RPGPlayerController* IB_PlayerController,
	AIB_MainChar* MainChar)
{
	IB_RPGPlayerController = IB_PlayerController;
	IB_MainChar = MainChar;
}

void ACannonPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FRotator NewRotation = FRotator(0.f, 0.f, 0.f); // Pitch, Yaw, Roll
	SetActorRotation(NewRotation);
}

void ACannonPawn::UnPossessed()
{
	Super::UnPossessed();

}

void ACannonPawn::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
void ACannonPawn::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ACannonPawn::MulticastSetMesh_Implementation(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh)
{
	if (InCannonBodyMesh && InCannonCartMesh && CannonBodyMesh && CannonCartMesh)
	{
		// Is local player
		if (GetOwner())
		{
			UE_LOG(LogTemp, Warning, TEXT("Owner: %s, HasAuthority: %d"), *GetOwner()->GetName(), HasAuthority());
			CannonBodyMesh->SetStaticMesh(InCannonBodyMesh);
			CannonCartMesh->SetStaticMesh(InCannonCartMesh);
		}
	}
}

void ACannonPawn::CannonRotation(const FInputActionValue& Value)
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


void ACannonPawn::CannonCameraMove(const FInputActionValue& Value)
{
	if (IsOnCharging)
	{
		return;
	}
	FVector2D Input = CameraMoveSpeed * Value.Get<FVector2D>();

	FVector CameraOffset = FVector(Input.Y, Input.X, 0) * 200.f * GetWorld()->GetDeltaSeconds();
	FollowCamera->AddLocalOffset(CameraOffset, true);
}

void ACannonPawn::ChargeCannonPower()
{
	if (!HasAuthority())
	{
		ServerChargeCannonPower(CurrentCannonPower,MaxCannonPower,ChargePowerSpeed);
		return;
	}
}

void ACannonPawn::ServerChargeCannonPower_Implementation(float CurrentPower, const float MaxPower, const float CharagePower)
{
	IsOnCharging = true;
	CurrentPower += CharagePower * GetWorld()->GetDeltaSeconds();
	if (CurrentPower >= MaxPower)
	{
		CurrentPower = MaxPower;
	}
	CurrentCannonPower = CurrentPower;
	UE_LOG(LogTemp, Warning, TEXT("ServerShootChar Executed! Power: %f"), CurrentCannonPower);

	//ClientSetCannonPower(CurrentPower);
}

void ACannonPawn::ClientSetCannonPower_Implementation(const float CurrentPower)
{
	//CurrentCannonPower = CurrentPower;
}

void ACannonPawn::ShootChar()
{
	if (!HasAuthority())
	{
		ServerShootChar(CurrentCannonPower);
	}
}

void ACannonPawn::ServerShootChar_Implementation(const float InCannonPowner)
{
	if (!HasAuthority()) return;

	if (IB_MainChar)
	{
		if (CannonMuzzle != nullptr)
		{
			//float ApplyWeight = IBChar->InventoryComponents->InventoryWeightAmount;

			FVector ForwardVector = CannonMuzzle->GetForwardVector() * (InCannonPowner);
			//FVector UpVector = CannonMuzzle->GetUpVector()*(CurrentCannonPower);
			FVector ShootingVector = (ForwardVector * 100.f);//(ApplyWeight/50.f);
			IB_MainChar->SetActorLocation(CannonMuzzle->GetComponentLocation());
			IB_MainChar->LaunchCharacter(ShootingVector, true, true);
			CannonTakeOff();
			//IB_MainChar->IsFlying = true;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					//this->IB_MainChar->PlayFlyingAnimation();
				}, 0.1f, false);
		

			CurrentCannonPower = 0.0f;
			IsOnCharging = false;
			ClientSetCannonProperty(CurrentCannonPower, IsOnCharging);
		}
	}
}

void ACannonPawn::ClientSetCannonProperty_Implementation(const float InCannonPowner, const bool InCannonCharging)
{
	IsOnCharging = InCannonCharging;
	CurrentCannonPower = InCannonPowner;
}

void ACannonPawn::OnRep_CannonShootPower()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerShootChar Executed! Power: %f"), CurrentCannonPower);
	// �� �� : UI�����
	//UpdateChargeBar();
}

void ACannonPawn::CannonTakeOff()
{
	if (IB_RPGPlayerController)
	{
		if (UCannonSpawnComponent* CannonSpawnComponent =  IB_RPGPlayerController->GetCannonSpawnComponent())
		{
			CannonSpawnComponent->SpawnOwnedCannonActor(IB_RPGPlayerController);
			ServerSwithchController();
			Destroy();
		}
	}
	
}


void ACannonPawn::ServerSwithchController_Implementation()
{
	if (IB_RPGPlayerController)
	{
		IB_RPGPlayerController->SwitchController();
	}
}
