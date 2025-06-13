#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalTrigger.generated.h"

class UBoxComponent;

UCLASS()
class IB_MULTIPLAYGAME_API APortalTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	APortalTrigger();
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components", meta = (AllowAccess = true))
	TObjectPtr<UStaticMeshComponent> PortalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Components")
	TObjectPtr<UBoxComponent> BoardingTriggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Dungeon Name")
	FString DungeonName;


public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Server,Reliable)
	void Server_TravelToDungeon(APlayerController* PlayerController);
	
};
