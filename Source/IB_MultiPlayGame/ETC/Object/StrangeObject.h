#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/InteractInterface.h"
#include "StrangeObject.generated.h"

class UBoxComponent;

UCLASS()
class IB_MULTIPLAYGAME_API AStrangeObject : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	AStrangeObject();

	UPROPERTY(EditAnywhere, meta = (AllowAccess = true))
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(EditAnywhere, meta = (AllowAccess = true))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere, meta = (AllowAccess = true))
	TObjectPtr<UBoxComponent> OverlapBoxComponent;

	UFUNCTION(BlueprintCallable)
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(EditAnywhere, Replicated)
	TObjectPtr<AActor> InteractedActor;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Interaction")
	class UWidgetComponent* WBP_Interact;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Interaction")
	class UMaterialInstance* InteractOverlayMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Quest")
	FString ObjectiveID;

protected:
	virtual void BeginPlay() override;

	virtual FString InteractWith_Implementation(APlayerController* PlayerController) override;


};
