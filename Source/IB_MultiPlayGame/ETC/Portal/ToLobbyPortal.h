#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToLobbyPortal.generated.h"

class UBoxComponent;

UCLASS()
class IB_MULTIPLAYGAME_API AToLobbyPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AToLobbyPortal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<UStaticMeshComponent> PortalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> BoardingTriggerBox;



	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
