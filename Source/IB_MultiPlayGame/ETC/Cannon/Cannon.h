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

public:
	UFUNCTION(NetMulticast,Reliable)
	void MulticastSetMesh(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh);
	
};
