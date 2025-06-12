#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"
#include "../../Interfaces/InteractInterface.h"
#include "Cannon.generated.h"

class UBoxComponent;
class AIB_MainChar;
class AIB_RPGPlayerController;

UCLASS()
class IB_MULTIPLAYGAME_API ACannon : public APawn , public IInteractInterface
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ACannon();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FString InteractWith_Implementation(APlayerController* PlayerController) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE  USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE  UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<UStaticMeshComponent> CannonBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UStaticMeshComponent> CannonCartMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<USceneComponent> CannonMuzzle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> BoardingTriggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> CannonBlockingBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	class UParticleSystemComponent* ParticleSystemComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Material")
	UMaterialInterface* ItemOverlayMaterial;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* IMC_Cannon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_CannonCameraMove;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_CannonRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_CannonShoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_TakeOff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CannonRotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargePowerSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOnCharging = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> BoardingActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CannonProperty")
	float MaxCannonPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CannonProperty")
	float CurrentCannonPower = 0;

public:
	UFUNCTION()
	void CannonRotation(const FInputActionValue& Value);
	UFUNCTION()
	void CannonCameraMove(const FInputActionValue& Value);
	UFUNCTION()
	void ChargeCannonPower();
	UFUNCTION()
	void ShootChar();
	UFUNCTION()
	void CannonTakeOff();
	UFUNCTION(Server,Reliable)
	void ServerSwithchController();
	UFUNCTION(Client, Reliable)
	void ClientSetCannonInfo(AIB_RPGPlayerController* IB_PlayerController, AIB_MainChar* MainChar);



public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CameraInputPropety")
	float CameraMoveSpeed;

	UPROPERTY(Replicated)
	TObjectPtr<AIB_MainChar> IB_MainChar;
	UPROPERTY(Replicated)
	TObjectPtr<AIB_RPGPlayerController> IB_RPGPlayerController;
	
public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UFUNCTION(NetMulticast,Reliable)
	void MulticastSetMesh(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh);
	
};
