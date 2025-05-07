#include "IB_RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "../../Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "../../WidgetController/InventoryWidgetController.h"
#include "Blueprint/UserWidget.h"
#include "../../Widget/W_RPGSystemWidget.h"
#include "../../Input/RPGSystemsInputComponents.h"
#include "IB_RPGPlayerState.h"
#include "IB_RPGAbilitySystemComponent.h"


AIB_RPGPlayerController::AIB_RPGPlayerController()
{
	bReplicates=true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);
}

void AIB_RPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (URPGSystemsInputComponents* RPGInputComp = Cast<URPGSystemsInputComponents>(InputComponent))
	{
		RPGInputComp->BindAbililtyActions(RPGInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
	}

}

UInventoryComponent* AIB_RPGPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

void AIB_RPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIB_RPGPlayerController,InventoryComponent);
}

void AIB_RPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->bOwnerLocallyControlled = IsLocalController();
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

void AIB_RPGPlayerController::CreateInventoryWidget()
{
	if (UUserWidget* Widget = CreateWidget<UW_RPGSystemWidget>(this, InventoryWidgetClass))
	{
		InventoryWidget = Cast<UW_RPGSystemWidget>(Widget);
		InventoryWidget->SetWidgetController(GetInventoryWidgetController());
		InventoryWidgetController->BroadcastInitialValues();

		InventoryWidget->AddToViewport(0);
	}
}


