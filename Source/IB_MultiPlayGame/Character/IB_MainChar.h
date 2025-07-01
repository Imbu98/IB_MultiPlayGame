#pragma once

#include "CoreMinimal.h"
#include "IB_BaseChar.h"
#include "GameplayTags.h"
#include "AbilitySystemInterface.h"
#include "../Interfaces/RPGAbilitySystemInterface.h"
#include "../Interfaces/InteractInterface.h"
#include "IB_MainChar.generated.h"

class UIB_RPGAbilitySystemComponent;
class UIB_RPGAttributeSet;
class UW_Overlay;

UENUM(BlueprintType)
enum class EIB_CharCycle : uint8
{
	Idle=0,
	Walk =1,
	Run=2,
};

UENUM(BlueprintType)
enum class EInteractObjective : uint8
{
	None=0,
	NPC = 1,
	Item = 2,
	Object = 3,
	Cannon=4,
	Others=5,
};


UCLASS()
class IB_MULTIPLAYGAME_API AIB_MainChar : public AIB_BaseChar ,public IAbilitySystemInterface , public IRPGAbilitySystemInterface , public IInteractInterface
{
	GENERATED_BODY()
	
public:
	AIB_MainChar();

	virtual USceneComponent* GetDynamicSpawnPoint_Implementation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
 
	UPROPERTY(BlueprintReadWrite,Replicated)
	EIB_CharCycle CharacterState;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server,Reliable)
	void ServerSetCharacterState(EIB_CharCycle NewState);

	virtual void OnHealthChanged(float CurrentHealth, float MaxHealth) override;


	virtual void OnManaChanged(float CurrentMana, float MaxMana) override;

	virtual void SetNPCActor_Implementation(AActor* NPCActor) override;

	UFUNCTION(Server, Reliable)
	void ServerSetNPCActor(AActor* NPCActor);

	UFUNCTION(BlueprintCallable)
	void BroadCastInitialValues();

	virtual void InitAbilityActorInfo() override;

public:

	virtual void NotifyControllerChanged() override;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_JumpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_OpenInventoryAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IB_OpenPlayerInfoAction;
	

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EIB_CharCycle CharacterCycle;

	
	UPROPERTY(BlueprintReadWrite,Replicated)
	AActor* LookatActor;

	// For Quest
	FOnObjectiveIdCalled OnObjectiveIdCalledDelegate;

	UPROPERTY(Replicated)
	FString QuestObjectiveId;

	UPROPERTY()
	TObjectPtr<AIB_RPGPlayerController> CachedOwningPlayerController;

	UPROPERTY(Replicated)
	bool IsCanMove=true;

protected:
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void MoveStop();

	UFUNCTION()
	void PlayerInteraction();

	UFUNCTION(Server,Reliable)
	void SereverPlayerInteraction();

	UFUNCTION()
	void PlayerAttack();
	
	UFUNCTION(Server,Reliable)
	void ServerPlayerAttack();

	UFUNCTION()
	void OpenInventory();

	UFUNCTION()
	void OpenPlayerInfoTab();

	

	void BindCallbacksToDependencies() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void InitClassDefaults() override;

	UFUNCTION()
	EInteractObjective DetermineInteractObjective(AActor* InteractObjective);


private:
	UPROPERTY(VisibleAnywhere,meta= (AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> DynamicProjectileSpawnPoint;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TObjectPtr<UIB_RPGAbilitySystemComponent> IB_RPGAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TObjectPtr<UIB_RPGAttributeSet> IB_RPGAttributeSet;

	

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	UFUNCTION(NetMulticast,Reliable)
	void MulticastPlayMontage(UAnimMontage* ToPlayMontage);
	
};


