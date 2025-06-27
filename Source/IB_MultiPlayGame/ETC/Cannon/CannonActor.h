#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IB_MultiPlayGame/Interfaces/InteractInterface.h"
#include "CannonActor.generated.h"

class UBoxComponent;

UCLASS()
class IB_MULTIPLAYGAME_API ACannonActor : public AActor ,public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ACannonActor();

protected:
	virtual void BeginPlay() override;

	virtual FString InteractWith_Implementation(APlayerController* PlayerController) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<UStaticMeshComponent> CannonBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UStaticMeshComponent> CannonCartMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> BoardingTriggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> CannonBlockingBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Material")
	UMaterialInterface* ItemOverlayMaterial;

protected:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UFUNCTION(NetMulticast,Reliable)
	void MulticastSetMesh(UStaticMesh* InCannonBodyMesh, UStaticMesh* InCannonCartMesh);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Quest")
	FString ObjectiveID;
	
	
};
