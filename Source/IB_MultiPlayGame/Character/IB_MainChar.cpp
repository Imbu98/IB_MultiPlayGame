#include "IB_MainChar.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


#include "IB_MultiPlayGame/IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerState.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/Data/IB_CharacterClassInfo.h"
#include "IB_MultiPlayGame/Widget/W_Overlay.h"
#include "../IB_Framework/IB_GameInstance.h"
#include "../IB_Framework/IB_GAS/IB_RPGAbilitySystemComponent.h"
#include "IB_NPCBase.h"
#include "IB_MultiPlayGame/ETC/Object/StrangeObject.h"
#include "../ETC/BaseSpawnedItem/BaseSpawnedItem.h"
#include "../Interfaces/InteractInterface.h"

#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


AIB_MainChar::AIB_MainChar()
{
	bReplicates=true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	DynamicProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	DynamicProjectileSpawnPoint->SetupAttachment(GetRootComponent());

}

void AIB_MainChar::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIB_MainChar, CharacterState);
	DOREPLIFETIME(AIB_MainChar, LookatActor);
	DOREPLIFETIME(AIB_MainChar, QuestObjectiveId);


}

USceneComponent* AIB_MainChar::GetDynamicSpawnPoint_Implementation()
{
	return DynamicProjectileSpawnPoint;
}

void AIB_MainChar::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		InitOverlay();
	}

	if (HasAuthority())
	{
		BroadCastInitialValues();
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
		{
			BroadCastInitialValues();
		},0.5f,false);
			
	}
}

void AIB_MainChar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		InitAbilityActorInfo();
	}
}

void AIB_MainChar::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AIB_MainChar::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (OverlayWidgetRef)
	{
		OverlayWidgetRef->SetHealthBar(CurrentHealth, MaxHealth);
	}
}

void AIB_MainChar::OnManaChanged(float CurrentMana, float MaxMana)
{
	if (OverlayWidgetRef)
	{
		OverlayWidgetRef->SetManaBar(CurrentMana, MaxMana);
	}
	
}

void AIB_MainChar::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AIB_MainChar::InitAbilityActorInfo()
{
	if (AIB_RPGPlayerState* IB_RPGPlayerState=GetPlayerState<AIB_RPGPlayerState>())
	{
		IB_RPGAbilitySystemComponent = IB_RPGPlayerState->GetIB_RPGAbilitySystemComponent();
		IB_RPGAttributeSet = IB_RPGPlayerState->GetIB_RPGAttributeSet();

		if (IsValid(IB_RPGAbilitySystemComponent))
		{
			IB_RPGAbilitySystemComponent->InitAbilityActorInfo(IB_RPGPlayerState,this);
			BindCallbacksToDependencies();

			if (HasAuthority())
			{
				InitClassDefaults();
			}
		}
	}
	
}

void AIB_MainChar::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,"[AIB_MainChar::InitClassDefaults] : Character Tag is Null");
	}
	else if (UIB_CharacterClassInfo* ClassInfo = UIB_BlueprintFunctionLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FIB_CharacterClassDefaultInfo* SelectedClassInfo  =  ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (IsValid(IB_RPGAbilitySystemComponent))
			{
				IB_RPGAbilitySystemComponent->AddCharacterAbilities(SelectedClassInfo->StartingAbilities);
				IB_RPGAbilitySystemComponent->AddCharacterPassiveAbilities(SelectedClassInfo->StartingPassives);
				IB_RPGAbilitySystemComponent->InitializeDefaultAttributes(SelectedClassInfo->DefaultAttributes);
			}
		}
	}
}

void AIB_MainChar::SetNPCActor_Implementation(AActor* NPCActor)
{
	if (NPCActor)
	{
		if (!HasAuthority())
		{
			LookatActor = NPCActor;
			ServerSetNPCActor(LookatActor);
			return;
		}
		LookatActor = NPCActor;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("ServerSet LookatActor : %s"), *LookatActor->GetName()));
	}
	else
	{
		if (!HasAuthority())
		{
			LookatActor = nullptr;
			ServerSetNPCActor(NPCActor);
			return;
		}
		LookatActor = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("ServerSet LookatActor : nullptr")));
	}
}

void AIB_MainChar::ServerSetNPCActor_Implementation(AActor* NPCActor)
{
		IInteractInterface::Execute_SetNPCActor(this,NPCActor);
	
}

void AIB_MainChar::BindCallbacksToDependencies()
{
	if (IsValid(IB_RPGAbilitySystemComponent)&&IsValid(IB_RPGAttributeSet))
	{
		IB_RPGAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(IB_RPGAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
			OnHealthChanged(Data.NewValue,IB_RPGAttributeSet->GetMaxHealth());	
			});

		IB_RPGAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(IB_RPGAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged(Data.NewValue,IB_RPGAttributeSet->GetMaxMana());
			});
	}
}

void AIB_MainChar::PlayerInteraction()
{
	if (!HasAuthority())
	{

		SereverPlayerInteraction();
		return;
	}
	if (LookatActor&&LookatActor->Implements<UInteractInterface>())
	{
		if (APlayerController* PC = Cast<APlayerController>(this->GetController()))
		{
			EInteractObjective ObjectiveType = DetermineInteractObjective(LookatActor);

			switch (ObjectiveType)
			{
				// if Class IB_NPCBase
			case EInteractObjective::NPC:
			{
				if (OnObjectiveIdCalledDelegate.IsBound())
				{
					QuestObjectiveId = IInteractInterface::Execute_InteractWith(LookatActor, PC);
					int32 QuestSuccessDefaultValue = 1;
					OnObjectiveIdCalledDelegate.Broadcast(QuestObjectiveId, QuestSuccessDefaultValue);
				}
				break;	
			}
			// if Class StrangeObject
			case EInteractObjective::Object:
			{
				if (OnObjectiveIdCalledDelegate.IsBound())
				{
					QuestObjectiveId = IInteractInterface::Execute_InteractWith(LookatActor, PC);
					int32 QuestSuccessDefaultValue = 1;
					OnObjectiveIdCalledDelegate.Broadcast(QuestObjectiveId, QuestSuccessDefaultValue);
				}
				break;
			}

			case EInteractObjective::Item:
			{
				IInteractInterface::Execute_InteractWith(LookatActor, PC);
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Serever Interact with Item")));
				break;
			}
				default:
					break;
			}
			
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Interact")));
}

void AIB_MainChar::SereverPlayerInteraction_Implementation()
{
	PlayerInteraction();
}

void AIB_MainChar::BroadCastInitialValues()
{
	if (IsValid(IB_RPGAttributeSet))
	{
		OnHealthChanged(IB_RPGAttributeSet->GetHealth(),IB_RPGAttributeSet->GetMaxHealth());
		OnManaChanged(IB_RPGAttributeSet->GetMana(),IB_RPGAttributeSet->GetMaxMana());
	}
}

void AIB_MainChar::InitOverlay()
{
	if(WBP_Overlay)
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			OverlayWidgetRef = CreateWidget<UW_Overlay>(PC,WBP_Overlay);
		
			if (OverlayWidgetRef)
			{
				OverlayWidgetRef->AddToViewport(0);
			}
		}
	}
}
EInteractObjective AIB_MainChar::DetermineInteractObjective(AActor* InteractObjective)
{
	if (AIB_NPCBase* NPCBase = Cast<AIB_NPCBase>(InteractObjective))
	{
		return EInteractObjective::NPC;
	}
	else if (AStrangeObject* Object = Cast<AStrangeObject>(InteractObjective))
	{
		return EInteractObjective::Object;
	}
	else if (ABaseSpawnedItem* Item = Cast<ABaseSpawnedItem>(InteractObjective))
	{
		return EInteractObjective::Item;
	}
	return EInteractObjective::None;

}

UAbilitySystemComponent* AIB_MainChar::GetAbilitySystemComponent() const
{
	return IB_RPGAbilitySystemComponent;
}



void AIB_MainChar::ServerSetCharacterState_Implementation(EIB_CharCycle NewState)
{
	CharacterState=NewState;
}


void AIB_MainChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AIB_MainChar::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AIB_MainChar::MoveStop);
		EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&AIB_MainChar::Look);
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Triggered,this,&ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Completed,this,&ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AIB_MainChar::PlayerInteraction);
	}
}

void AIB_MainChar::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
	if (HasAuthority())
	{
		CharacterState=EIB_CharCycle::Walk;
	}
	else
	{
		
		ServerSetCharacterState(EIB_CharCycle::Walk);
	}
	
}

void AIB_MainChar::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIB_MainChar::MoveStop()
{
	if (HasAuthority())
	{
		CharacterState = EIB_CharCycle::Idle;
	}
	else
	{

		ServerSetCharacterState(EIB_CharCycle::Idle);
	}
}




