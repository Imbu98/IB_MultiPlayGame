#include "IB_RPGPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/QuestLogComponent.h"
#include"../../Components/QuestComponent.h"
#include "../../Components/QuestGiverComponent.h"
#include "../../Character/IB_MainChar.h"
#include "../../WidgetController/InventoryWidgetController.h"
#include "../../Widget/W_RPGSystemWidget.h"
#include "../../Widget/W_QuestGiver.h"
#include "../../Widget/W_QuestLog.h"
#include "../../Widget/W_LocationNotify.h"
#include "../../Widget/W_QuestNotification.h"
#include "../../Widget/W_QuestRewards.h"
#include "../../Widget/W_CannonWidget.h"
#include "../../Widget/W_Overlay.h"
#include "../../Input/RPGSystemsInputComponents.h"
#include "IB_RPGPlayerState.h"
#include "IB_RPGAbilitySystemComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../../ETC/Cannon/CannonSpawnManager.h"
#include "../../ETC/Cannon/Cannon.h"

#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Kismet\GameplayStatics.h"

DEFINE_LOG_CATEGORY(Imbu);

AIB_RPGPlayerController::AIB_RPGPlayerController()
{
	bReplicates=true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);
	

	QuestLogComponent = CreateDefaultSubobject<UQuestLogComponent>("QuestLogComponent");
	QuestLogComponent->SetIsReplicated(true);

	QuestComponent=CreateDefaultSubobject<UQuestComponent>("QuestComponent");
	QuestComponent->SetIsReplicated(true);


	/*bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;*/
}

void AIB_RPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIB_RPGPlayerController, InventoryComponent);
	DOREPLIFETIME(AIB_RPGPlayerController, QuestLogComponent);
	DOREPLIFETIME(AIB_RPGPlayerController, QuestComponent);
	
}

void AIB_RPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (URPGSystemsInputComponents* RPGInputComp = Cast<URPGSystemsInputComponents>(InputComponent))
	{
		RPGInputComp->BindAbililtyActions(RPGInputConfig, this, &AIB_RPGPlayerController::AbilityInputPressed, &AIB_RPGPlayerController::AbilityInputReleased);
	}
	//if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	//{
	//	Subsystem->AddMappingContext(IB_IMC, 0);
	//}

	//// Set up action bindings
	//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	//{
	//	// Setup mouse input events
	//	EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AIB_RPGPlayerController::OnInputStarted);
	//	EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AIB_RPGPlayerController::OnSetDestinationTriggered);
	//	EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AIB_RPGPlayerController::OnSetDestinationReleased);
	//	EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AIB_RPGPlayerController::OnSetDestinationReleased);

	//	/* Setup touch input events
	//	EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ATopdownTestPlayerController::OnInputStarted);
	//	EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ATopdownTestPlayerController::OnTouchTriggered);
	//	EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ATopdownTestPlayerController::OnTouchReleased);
	//	EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ATopdownTestPlayerController::OnTouchReleased);*/
	//}
	//else
	//{
	//	
	//}

}


UInventoryComponent* AIB_RPGPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

void AIB_RPGPlayerController::SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	if (IsValid(RPGAbilitySystemComp))
	{
		RPGAbilitySystemComp->SetDynamicProjectile(ProjectileTag, AbilityLevel);
	 }
}

void AIB_RPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->bOwnerLocallyControlled = IsLocalController();
	}
	
	
	if (IsLocalController())
	{
		ClientSwitchWidget();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
				ServerSpawnCannonRequest();
		}, 2.0f, false); // 약간의 지연
		
	}
}

void AIB_RPGPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (IsValid(GetRPGAbilitySystemComponent()))
	{
		RPGAbilitySystemComp->AbilityInputPressed(InputTag);
	}
}

void AIB_RPGPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (IsValid(GetRPGAbilitySystemComponent()))
	{
		RPGAbilitySystemComp->AbilityInputReleased(InputTag);
	}
}

UIB_RPGAbilitySystemComponent* AIB_RPGPlayerController::GetRPGAbilitySystemComponent()
{
	if (!IsValid(RPGAbilitySystemComp))
	{
		if (const AIB_RPGPlayerState* RPGPlayerState = GetPlayerState<AIB_RPGPlayerState>())
		{
			RPGAbilitySystemComp = RPGPlayerState->GetIB_RPGAbilitySystemComponent();
		}
	}

	return RPGAbilitySystemComp;
}

void AIB_RPGPlayerController::DisplayQuestLog()
{
	if (WBP_QuestLogClass)
	{
		if (WBP_QuestLog = CreateWidget<UW_QuestLog>(this, WBP_QuestLogClass))
		{
			WBP_QuestLog->IB_RPGPlayerController = this;
			WBP_QuestLog->AddToViewport();
		}
	}
	
}

void AIB_RPGPlayerController::ClientDisplayLocationNotification_Implementation(const FText& LocationName)
{
	if (LocationName.IsEmpty()) return;

	if (WBP_LocationNotifyClass)
	{
		if (WBP_LocationNotify = CreateWidget<UW_LocationNotify>(this, WBP_LocationNotifyClass))
		{
			WBP_LocationNotify->LocationName = LocationName;
			WBP_LocationNotify->AddToViewport(0);
		}
	}
}

UAbilitySystemComponent* AIB_RPGPlayerController::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

UInventoryWidgetController* AIB_RPGPlayerController::GetInventoryWidgetController()
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		if (IsValid(InventoryWidgetController))
		{
			InventoryWidgetController->SetOwningActor(this);
			InventoryWidgetController->BindCallBacksToDependencies();
		}
		
	}
	return InventoryWidgetController;
}

// on client
void AIB_RPGPlayerController::CreateInventoryWidget()
{
	if (InventoryWidgetClass)
	{
		if (UUserWidget* Widget = CreateWidget<UW_RPGSystemWidget>(this, InventoryWidgetClass))
		{
			InventoryWidget = Cast<UW_RPGSystemWidget>(Widget);
			InventoryWidget->SetWidgetController(GetInventoryWidgetController());
			InventoryWidgetController->BroadcastInitialValues();
			InventoryWidget->AddToViewport(0);
		}
	}

}

void AIB_RPGPlayerController::ClientDisplayQuest_Implementation(FQuestDetails QuestDetails,FName QuestID)
{
	if (this->IsLocalController() && !this->GetPawn()->HasAuthority())
	{
		WBP_QuestGiverWidget = CreateWidget<UW_QuestGiver>(this, WBP_QuestGiverWidgetClass);
		if (WBP_QuestGiverWidget)
		{
			WBP_QuestGiverWidget->QuestDetails = QuestDetails;
			WBP_QuestGiverWidget->QuestID = QuestID;
			WBP_QuestGiverWidget->InventoryComponent = InventoryComponent;
			
			WBP_QuestGiverWidget->AddToViewport();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("RPGPlayerController Not Local Controller, and HasAuthority"));
	}
}

void AIB_RPGPlayerController::ClientDisplayRewards_Implementation(FQuestDetails QuestDetails, FName QuestID)
{
	if (this->IsLocalController() && !this->GetPawn()->HasAuthority())
	{
		WBP_QuestRewards = CreateWidget<UW_QuestRewards>(this, WBP_QuestRewardsClass);
		if (WBP_QuestRewards)
		{
			WBP_QuestRewards->QuestDetails = QuestDetails;
			WBP_QuestRewards->QuestID = QuestID;
			WBP_QuestRewards->InventoryComponent = InventoryComponent;
			WBP_QuestRewards->AddToViewport();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("RPGPlayerController Not Local Controller, and HasAuthority"));
	}
}
void AIB_RPGPlayerController::ClientDisplayNotification_Implementation(const FObjectiveDetails& ObjectiveDetails)
{
	if (this->IsLocalController() && !this->GetPawn()->HasAuthority())
	{
		
		if (WBP_QuestNotificationClass)
		{
			if (WBP_QuestNotificationWidget = CreateWidget<UW_QuestNotification>(this, WBP_QuestNotificationClass))
			{
				WBP_QuestNotificationWidget->ObjectiveText = ObjectiveDetails.Description;
				WBP_QuestNotificationWidget->AddToViewport(0);
			}

			
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("RPGPlayerController Not Local Controller, and HasAuthority"));
	}
}

void AIB_RPGPlayerController::ServerSpawnCannonRequest_Implementation()
{
	if(ACannonSpawnManager* CannonSpawnManager = Cast<ACannonSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACannonSpawnManager::StaticClass())))
	{
		CannonSpawnManager->SpawnOwnedCannon(this);

	}
}


void AIB_RPGPlayerController::SwitchController()
{
	if (!HasAuthority())
	{
		ServerSwitchController();
		return;
	}
	
		// 지금 controlled pawn이 ib_char일 때
		if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(GetPawn()))
		{
			CachedIB_MainChar = IB_MainChar;
			if (OwningCannon)
			{
				if (IsOnCannon == false)
				{
				// possess to cannon
				IsOnCannon = true;
				Possess(OwningCannon);
				
					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
						{
							ClientSwitchWidget();
							ClientSwitchInputMapping(IsOnCannon, CachedIB_MainChar, OwningCannon);
						}, 0.2f, false);
				}
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AIB_RPGPlayerController::SwitchController : No OwningCannon"));
			}
		}
		// possess to IB_MainChar
		else if (ACannon* Cannon = Cast<ACannon>(GetPawn()))
		{
			IsOnCannon = false;
			Possess(CachedIB_MainChar);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					ClientSwitchWidget();
					ClientSwitchInputMapping(IsOnCannon, CachedIB_MainChar, OwningCannon);
				}, 0.5f, false);
			
			
		}
}

void AIB_RPGPlayerController::ClientSwitchWidget_Implementation()
{
	if (ACannon* Cannon = Cast<ACannon>(GetPawn()))
	{
		if (WBP_CannonWidgetClass)
		{
			if (WBP_CannonWidget = CreateWidget<UW_CannonWidget>(this, WBP_CannonWidgetClass))
			{
				WBP_CannonWidget->AddToViewport();
			}
		}
		if (WBP_OverlayWidget)
		{
			WBP_OverlayWidget->RemoveFromParent();
		}
		
	}
	else if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(GetPawn()))
	{
		if (WBP_OverlayWidgetClass)
		{
			WBP_OverlayWidget = CreateWidget<UW_Overlay>(this, WBP_OverlayWidgetClass);

			if (WBP_OverlayWidget)
			{
				WBP_OverlayWidget->AddToViewport(0);
				HandleCharValues(IB_MainChar);
				ServerInitCharValues(IB_MainChar);
			}
		}
		if (WBP_CannonWidget)
		{
			WBP_CannonWidget->RemoveFromParent();
		}
	}
}

void AIB_RPGPlayerController::ServerInitCharValues_Implementation(AIB_MainChar* MainChar)
{
	if (MainChar)
	{
		HandleCharValues(MainChar);
	}
}
void AIB_RPGPlayerController::HandleCharValues(AIB_MainChar* MainChar)
{
	if (MainChar)
	{
		MainChar->InitAbilityActorInfo();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this,MainChar]()
			{
				MainChar->BroadCastInitialValues();
			}, 0.3f, false);
		
	}
}

void AIB_RPGPlayerController::ServerSwitchController_Implementation()
{
	SwitchController();
}

void AIB_RPGPlayerController::ClientSwitchInputMapping_Implementation(bool OnCannon, AIB_MainChar* IBMainChar, ACannon* Cannon)
{
	if (!IsValid(Cannon)) return;
	if (!IsValid(IBMainChar)) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
		if (Subsystem != nullptr)
		{
			if (OnCannon == true)
			{
				Subsystem->RemoveMappingContext(IBMainChar->IMC_Default);
				Subsystem->AddMappingContext(Cannon->IMC_Cannon, 1);
				//IBPlayerController->ClosePlayerWidget();
			}
			else if (OnCannon ==false)
			{
				Subsystem->RemoveMappingContext(Cannon->IMC_Cannon);
				Subsystem->AddMappingContext(IBMainChar->IMC_Default, 0);
				//IBPlayerController->OpenPlayerWidget();
			}
		}

}




//void AIB_RPGPlayerController::OnInputStarted()
//{
//	StopMovement();
//}
//
//void AIB_RPGPlayerController::OnSetDestinationTriggered()
//{
//
//	 We flag that the input is being pressed
//	FollowTime += GetWorld()->GetDeltaSeconds();
//
//	 We look for the location in the world where the player has pressed the input
//	FHitResult Hit;
//	bool bHitSuccessful = false;
//	if (bIsTouch)
//	{
//		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
//	}
//	else
//	{
//		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
//	}
//
//	 If we hit a surface, cache the location
//	if (bHitSuccessful)
//	{
//		CachedDestination = Hit.Location;
//	}
//
//	 Move towards mouse pointer or touch
//	APawn* ControlledPawn = GetPawn();
//	if (ControlledPawn != nullptr)
//	{
//		FVector DebugLocation = ControlledPawn->GetActorLocation();
//		FVector WorldDirection = (CachedDestination - DebugLocation).GetSafeNormal();
//		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
//	}
//}
//
//void AIB_RPGPlayerController::OnSetDestinationReleased()
//{
//	if (!HasAuthority())
//	{
//		ServerSetDestination(CachedDestination);
//		return;
//	}
//	 If it was a short press
//	if (FollowTime <= ShortPressThreshold)
//	{
//
//		 We move there and spawn some particles
//		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
//		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
//	}
//
//	FollowTime = 0.f;
//
//}
//
//void AIB_RPGPlayerController::ServerSetDestination_Implementation(FVector MoveTarget)
//{
//	OnSetDestinationReleased();
//}