#pragma once

#include "CoreMinimal.h"
#include "IB_BaseChar.h"
#include "GameplayTags.h"
#include "AbilitySystemInterface.h"
#include "IB_MainChar.generated.h"

class UIB_RPGAbilitySystemComponent;
class UIB_RPGAttributeSet;

UENUM(BlueprintType)
enum class IB_CharCycle : uint8
{
	Idle=0,
	Walk =1,
	Run=2,
};

UCLASS()
class IB_MULTIPLAYGAME_API AIB_MainChar : public AIB_BaseChar ,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AIB_MainChar();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
 
	UPROPERTY(BlueprintReadWrite,Replicated)
	IB_CharCycle CharacterState;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server,Reliable)
	void ServerSetCharacterState(IB_CharCycle NewState);


	
	virtual void OnHealthChanged(float CurrentHealth, float MaxHealth) override;


	virtual void OnManaChanged(float CurrentMana, float MaxMana) override;

public:

	virtual void NotifyControllerChanged() override;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContext;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	IB_CharCycle CharacterCycle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> WBP_Overlay;
	
	UPROPERTY()
	class UW_Overlay* OverlayWidgetRef;



protected:
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	virtual void InitAbilityActorInfo() override;

	void BindCallbacksToDependencies() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void InitClassDefaults() override;

private:
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TObjectPtr<UIB_RPGAbilitySystemComponent> IB_RPGAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TObjectPtr<UIB_RPGAttributeSet> IB_RPGAttributeSet;


	UFUNCTION(BlueprintCallable)
	void BroadCastInitialValues();
	UFUNCTION()
	void InitOverlay();
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	
};


