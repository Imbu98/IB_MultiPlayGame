#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IB_MultiPlayGame/Components/InventoryComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "../../Interfaces/InventoryInterface.h"
#include "../../Interfaces/RPGAbilitySystemInterface.h"
#include "../../QuestSystem/QuestStructure.h"
#include "IB_RPGPlayerController.generated.h"

class UInventoryWidgetController;
class UW_RPGSystemWidget;
class URPGInputConfig;
class UIB_RPGAbilitySystemComponent;
class UInputAction;
class UNiagaraSystem;
class UInputMappingContext;
class UQuestLogComponent;
class UQuestComponent;
class UW_QuestGiver;
class UW_QuestLog;
class UW_LocationNotify;
class UW_QuestRewards;
class UW_QuestNotification;

UCLASS()
class IB_MULTIPLAYGAME_API AIB_RPGPlayerController : public APlayerController, public IAbilitySystemInterface , public IInventoryInterface , public IRPGAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AIB_RPGPlayerController();

	virtual void SetupInputComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;

	virtual void SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UInventoryWidgetController* GetInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

protected:

	virtual void BeginPlay() override;

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Input")
	TObjectPtr<UIB_RPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(EditDefaultsOnly,Category= "CustomValues|Input")
	TObjectPtr<URPGInputConfig> RPGInputConfig;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess=true),Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UW_RPGSystemWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Widgets")
	TSubclassOf<UW_RPGSystemWidget> InventoryWidgetClass;

	UIB_RPGAbilitySystemComponent* GetRPGAbilitySystemComponent();

	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UQuestLogComponent> QuestLogComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UQuestComponent> QuestComponent;

	UPROPERTY(EditAnywhere, category = "Custom Values | Widget")
	TSubclassOf<UUserWidget> WBP_QuestLogClass;
	UPROPERTY()
	TObjectPtr<UW_QuestLog> WBP_QuestLog;


	UPROPERTY(EditAnywhere, category = "Custom Values | Widget")
	TSubclassOf<UUserWidget> WBP_QuestGiverWidgetClass;
	UPROPERTY()
	TObjectPtr<UW_QuestGiver> WBP_QuestGiverWidget;

	UPROPERTY(EditAnywhere, category = "Custom Values | Widget")
	TSubclassOf<UUserWidget> WBP_QuestRewardsClass;
	UPROPERTY()
	TObjectPtr<UW_QuestRewards> WBP_QuestRewards;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widget")
	TSubclassOf<UW_LocationNotify> WBP_LocationNotifyClass;
	UPROPERTY()
	TObjectPtr<UW_LocationNotify> WBP_LocationNotify;

	UPROPERTY(EditAnywhere, Category = "Custom Values | Widget")
	TSubclassOf<UW_QuestNotification> WBP_QuestNotificationClass;

	UPROPERTY()
	TObjectPtr<UW_QuestNotification> WBP_QuestNotificationWidget;

public:
	UPROPERTY(Replicated)
	bool bAlreadyQuest;


public:
	UFUNCTION(BlueprintCallable)
	void DisplayQuestLog();

	UFUNCTION(Client, Reliable)
	void ClientDisplayQuest(FQuestDetails QuestDetails, FName QuestID);
	UFUNCTION(Client, Reliable)
	void ClientDisplayRewards(FQuestDetails QuestDetails, FName QuestID);
	UFUNCTION(Client, Reliable)
	void ClientDisplayNotification(const FObjectiveDetails& ObjectiveDetails);

	UFUNCTION(Client,Reliable)
	void ClientDisplayLocationNotification(const FText& LocationName);
	
	
};


//private:
//	UPROPERTY(Replicated)
//	FVector CachedDestination;
//
//	bool bIsTouch; // Is it a touch device
//	float FollowTime; // For how long it has been pressed
//	uint32 bMoveToMouseCursor : 1;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
//	float ShortPressThreshold;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
//	UNiagaraSystem* FXCursor;
//
//	/** MappingContext */
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
//	UInputMappingContext* IB_IMC;
//
//	/** Jump Input Action */
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
//	UInputAction* SetDestinationClickAction;
//
//	void OnInputStarted();
//	void OnSetDestinationTriggered();
//	void OnSetDestinationReleased();
//
//	UFUNCTION(Server, Reliable)
//	void ServerSetDestination(FVector MoveTarget);