#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LocationMarker.generated.h"

class UBoxComponent;
class UW_LocationNotify;

UCLASS()
class IB_MULTIPLAYGAME_API ALocationMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	ALocationMarker();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> OverlapBoxComponent;

	UPROPERTY(EditAnywhere,Category="Custom Values | Quest")
	FText LocationName;
	UPROPERTY(EditAnywhere, Category = "Custom Values | Quest")
	FString ObjectiveID;

	

public:
	UFUNCTION(BlueprintCallable)
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	




	

	
	
};
