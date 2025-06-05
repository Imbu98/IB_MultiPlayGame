#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../../Interfaces/InteractInterface.h"
#include "Cannon.generated.h"

class UBoxComponent;

UCLASS()
class IB_MULTIPLAYGAME_API ACannon : public APawn , public IInteractInterface
{
	GENERATED_BODY()


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ACannon();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FString InteractWith_Implementation(APlayerController* PlayerController) override;

public:
	FORCEINLINE  USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE  UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UStaticMeshComponent> CannonBodyMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UStaticMeshComponent> CannonCartMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<USceneComponent> CannonMuzzle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> BoardingTriggerBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom Values | Components")
	class UParticleSystemComponent* ParticleSystemComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Material")
	UMaterialInterface* ItemOverlayMaterial;

public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	virtual void BeginPlay() override;

	
};
